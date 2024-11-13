#pragma once

#include <cstdint>
#include <string>

#include <raylib.h>

namespace mr {

class rendertexture {
private:
  RenderTexture2D rt_;
  bool isloaded;

public:
  /// @brief Returns a temprary, read-only rederence to the rendertexture.
  /// @warning Do not copy the returned reference.
  const RenderTexture2D &get() const noexcept;
  bool is_loaded() const noexcept;
  void unload();
  RenderTexture2D release();

  rendertexture &operator=(rendertexture &&) noexcept;
  rendertexture &operator=(const rendertexture &) = delete;

  rendertexture();
  rendertexture(const rendertexture &) = delete;
  rendertexture(rendertexture &&) noexcept;
  explicit rendertexture(uint16_t width, uint16_t height);

  ~rendertexture();
};

class image {
private:
  Image img_;
  bool isloaded;

public:
  /// @brief Returns a temprary, read-only rederence to the image.
  /// @warning Do not copy the returned reference.
  const Image &get() const noexcept;
  bool is_loaded() const noexcept;
  void unload();
  Image release() noexcept;

  image &operator=(image &&) noexcept;
  image &operator=(const image &) = delete;

  image();
  image(const image &) = delete;
  image(image &&) noexcept;
  explicit image(const std::string &);
  explicit image(const char *);

  ~image();
};

class texture {
private:
  Texture2D texture_;
  bool isloaded;

public:
  /// @brief Returns a temprary, read-only rederence to the texture.
  /// @warning Do not copy the returned reference.
  const Texture2D &get() const noexcept;
  const Texture2D *get_ptr() const noexcept;

  bool is_loaded() const noexcept;
  void unload();

  /// @brief Releases ownership of the texture and returns it.
  Texture2D release() noexcept;

  texture &operator=(texture &&) noexcept;
  texture &operator=(const texture &) = delete;

  texture();
  texture(const texture &) = delete;
  texture(texture &&) noexcept;

  explicit texture(const std::string &);
  explicit texture(const char *);
  explicit texture(const Image *);
  explicit texture(const Image &);
  explicit texture(const image &);

  ~texture();
};

typedef texture unique_texture;
typedef rendertexture unique_rendertexture;
typedef image unique_image;

}; // namespace mr
