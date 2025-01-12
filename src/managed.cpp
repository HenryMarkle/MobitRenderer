#if defined(_WIN32) || defined(_WIN64)
  #define WIN32_LEAN_AND_MEAN
#endif

#include <cstdint>
#include <string>

#include <raylib.h>

#include <MobitRenderer/managed.h>

namespace mr {

const RenderTexture2D &rendertexture::get() const noexcept { return rt_; }
bool rendertexture::is_loaded() const noexcept { return isloaded; }
void rendertexture::unload() {
  UnloadRenderTexture(rt_);
  isloaded = false;
}
RenderTexture2D rendertexture::release() {
  isloaded = false;

  auto rt = rt_;
  rt_ = RenderTexture2D{0, {0}};
  return rt;
}
rendertexture &rendertexture::operator=(rendertexture &&other) noexcept {
  if (this == &other)
    return *this;

  unload();

  rt_ = other.rt_;
  isloaded = other.isloaded;

  other.rt_ = RenderTexture2D{0};
  other.isloaded = false;

  return *this;
}
rendertexture::rendertexture()
    : rt_({0, {0}}), isloaded(false) {}
rendertexture::rendertexture(rendertexture &&other) noexcept {
  rt_ = other.rt_;
  isloaded = other.isloaded;

  other.rt_ = RenderTexture2D{0};
  other.isloaded = false;
}
rendertexture::rendertexture(uint16_t width, uint16_t height) {
  rt_ = LoadRenderTexture(width, height);
  isloaded = true;
}
rendertexture::~rendertexture() { unload(); }

const Image &image::get() const noexcept { return img_; }
bool image::is_loaded() const noexcept { return isloaded; }
void image::unload() {
  if (!isloaded)
    return;
  UnloadImage(img_);
  isloaded = false;
}
Image image::release() noexcept {
  auto img = img_;
  img_ = Image{nullptr, 0, 0};

  isloaded = false;
  return img;
}
image &image::operator=(image &&other) noexcept {
  if (this == &other)
    return *this;

  unload();

  img_ = other.img_;
  isloaded = other.isloaded;

  other.img_ = Image{
      nullptr,
      0,
      0,
  };
  other.isloaded = false;

  return *this;
}
image::image() : img_({nullptr}), isloaded(false) {}
image::image(image &&other) noexcept {
  img_ = other.img_;
  isloaded = other.isloaded;

  other.img_ = Image{
      nullptr,
      0,
      0,
  };
  other.isloaded = false;
}
image::image(const std::string &path) {
  img_ = LoadImage(path.c_str());
  isloaded = true;
}
image::image(const char *path) {
  img_ = LoadImage(path);
  isloaded = true;
}
image::~image() { unload(); }

const Texture2D &texture::get() const noexcept { return texture_; }
const Texture2D *texture::get_ptr() const noexcept { return &texture_; }
bool texture::is_loaded() const noexcept { return isloaded; }
void texture::unload() {
  if (!isloaded)
    return;

  isloaded = false;
  UnloadTexture(texture_);
}
Texture2D texture::release() noexcept {
  auto texture = texture_;
  texture_ = Texture2D{0};

  isloaded = false;
  return texture;
}
texture &texture::operator=(texture &&other) noexcept {
  if (this == &other)
    return *this;

  unload();

  texture_ = other.texture_;
  isloaded = other.isloaded;

  other.texture_ = Texture2D{0};
  other.isloaded = false;

  return *this;
}
texture::texture() : texture_({0}), isloaded(false) {}
texture::texture(texture &&other) noexcept {
  texture_ = other.texture_;
  isloaded = other.isloaded;

  other.texture_ = Texture2D{0};
  other.isloaded = false;
}
texture::texture(const std::string &path) {
  texture_ = LoadTexture(path.c_str());
  isloaded = true;
}
texture::texture(const char *path) {
  texture_ = LoadTexture(path);
  isloaded = true;
}
texture::texture(const Image *image) {
  texture_ = LoadTextureFromImage(*image);
  isloaded = true;
}
texture::texture(const Image &image) {
  texture_ = LoadTextureFromImage(image);
  isloaded = true;
}
texture::texture(const image &image) {
  texture_ = LoadTextureFromImage(image.get());
  isloaded = true;
}
texture::~texture() { unload(); }

}; // namespace mr
