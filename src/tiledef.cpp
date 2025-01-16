#include <string>
#include <filesystem>

#include <raylib.h>

#include <MobitRenderer/definitions.h>


// To be used in unordered maps and sets
namespace std {
template <> struct hash<mr::TileDef> {
  std::size_t operator()(const mr::TileDef &t) const {
    return std::hash<std::string>{}(t.get_name());
  }
};
}; // namespace std

namespace mr {
const std::string &TileDef::get_name() const { return name; }

TileDefType TileDef::get_type() const { return type; }

int TileDef::get_width() const { return width; }
int TileDef::get_height() const { return height; }
int TileDef::get_buffer() const { return buffer; }

ivec2 TileDef::get_head_offset() const { return head_offset; }

const std::vector<int8_t> &TileDef::get_specs() const { return specs; }
const std::vector<int8_t> &TileDef::get_specs2() const { return specs2; }
const std::vector<int8_t> &TileDef::get_specs3() const { return specs3; }

const std::vector<uint8_t> &TileDef::get_repeat() const { return repeat; }

const std::vector<std::string> &TileDef::get_tags() const { return tags; }
int TileDef::get_rnd() const { return rnd; }

void TileDef::set_texture_path(std::filesystem::path path) noexcept { texture_path = path; }
const std::filesystem::path &TileDef::get_texture_path() const noexcept { return texture_path; }

void TileDef::reload_texture() {
  unload_texture();
  texture = LoadTexture(texture_path.string().c_str());
}
void TileDef::unload_texture() {
  if (texture.id != 0) {
    UnloadTexture(texture);
    texture = {0};
  }
}

const Texture2D &TileDef::get_texture() const { return texture; }

// TileDef &TileDef::operator=(TileDef &&other) noexcept {
//   if (this == &other)
//     return *this;
//   if (texture.id != 0)
//     UnloadTexture(texture);

//   name = std::move(other.name);
//   type = other.type;

//   width = other.width;
//   height = other.height;
//   buffer = other.buffer;
//   rnd = other.rnd;

//   specs = std::move(other.specs);
//   specs2 = std::move(other.specs2);
//   specs3 = std::move(other.specs3);
//   repeat = std::move(other.repeat);
//   tags = std::move(other.tags);

//   head_offset = other.head_offset;

//   texture = other.texture;
//   texture_path = std::move(other.texture_path);

//   other.texture = Texture2D{0};
//   other.width = 0;
//   other.height = 0;
//   other.buffer = 0;
//   other.rnd = 0;

//   return *this;
// }

// TileDef::TileDef(TileDef &&other) noexcept {
//   if (texture.id != 0)
//     UnloadTexture(texture);

//   name = std::move(other.name);
//   type = other.type;

//   width = other.width;
//   height = other.height;
//   buffer = other.buffer;
//   rnd = other.rnd;

//   specs = std::move(other.specs);
//   specs2 = std::move(other.specs2);
//   specs3 = std::move(other.specs3);
//   repeat = std::move(other.repeat);
//   tags = std::move(other.tags);

//   head_offset = other.head_offset;

//   texture = other.texture;
//   texture_path = std::move(other.texture_path);

//   other.texture = Texture2D{};
//   other.width = 0;
//   other.height = 0;
//   other.buffer = 0;
//   other.rnd = 0;
// }

TileDef::TileDef(
  std::string name, TileDefType type, uint8_t width,
  uint8_t height, uint8_t buffer, int8_t rnd,
  std::vector<std::string> tags, std::vector<int8_t> specs,
  std::vector<int8_t> specs2, std::vector<int8_t> specs3,
  std::vector<uint8_t> repeat
  ) : 
    name(name), 
    type(type), 
    width(width), 
    height(height), 
    buffer(buffer),
    rnd(rnd), 
    tags(tags), 
    specs(specs), 
    specs2(specs2), 
    specs3(specs3),
    repeat(repeat), 
    texture_path(""), 
    texture(Texture2D{0}), 
    head_offset(mr::ivec2{(int)(width / 2), (int)(height / 2)}) {}

TileDef::~TileDef() {
  unload_texture();
}

}; // namespace mr
