#include <string>
#include <filesystem>
#include <unordered_set>
#include <math.h>

#ifdef IS_DEBUG_BUILD
#include <algorithm>
#include <iostream>
#endif

#include <raylib.h>

#include <MobitRenderer/definitions.h>
#include <MobitRenderer/utils.h>

// To be used in unordered maps and sets
namespace std {
template <> struct hash<mr::TileDef> {
  std::size_t operator()(const mr::TileDef &t) const {
    return std::hash<std::string>{}(t.get_name());
  }
};
}; // namespace std

namespace mr {

void TileDef::load_texture() {
  if (_is_texture_loaded) return;

  auto target_path = texture_path;

  #if defined(__linux__) || defined(__APPLE__)
  {
    const auto &name = texture_path.filename().string();

    auto to_lower = [](const std::string &str) {
      std::string lower_str = str;
      std::transform(lower_str.begin(), lower_str.end(), lower_str.begin(),
                     [](unsigned char c) { return std::tolower(c); });
      return lower_str;
    };

    auto target_lower = to_lower(name);
    auto found = false;

    for (const auto &entry : std::filesystem::directory_iterator(texture_path.parent_path())) { 
      if (entry.is_regular_file()) {
        std::string entry_filename = entry.path().filename().string();
        if (to_lower(entry_filename) == target_lower) {
          target_path = entry.path();
          found = true;
          break;
        }
      }
    }

    if (!found) {
      #ifdef IS_DEBUG_BUILD
      std::cout << "Warning: tile texture not found: " << texture_path << std::endl;
      #endif
      return;
    }
  }
  #endif

  if (!std::filesystem::exists(target_path)) {
    #ifdef IS_DEBUG_BUILD
    std::cout << "Warning: tile texture not found: " << target_path << std::endl;
    #endif

    return;
  }

  if (type != TileDefType::box) {
    auto img = LoadImage(target_path.string().c_str());
    ImageCrop(&img, Rectangle{0, 1, (float)img.width, (float)img.height-1});
    texture = LoadTextureFromImage(img);
    UnloadImage(img);
  } else {
    texture = LoadTexture(target_path.string().c_str());
  }

  _is_texture_loaded = true;
}

void TileDef::unload_texture() {
  if (!_is_texture_loaded) return;
  mr::utils::unload_texture(texture);
  _is_texture_loaded = false;
}

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
  std::string name, TileDefType type, int width,
  int height, int buffer, int rnd,
  std::unordered_set<std::string> tags, std::vector<int> specs,
  std::vector<int> specs2, std::vector<int> specs3,
  std::vector<int> repeat
  ) : 
    name(name),
    category(std::string()),
    color(WHITE),
    type(type), 
    width(width), 
    height(height), 
    buffer(buffer),
    rnd(rnd), 
    tags(tags), 
    specs(specs), 
    specs2(specs2), 
    specs3(specs3),
    multilayer(!specs2.empty() || !specs3.empty()),
    repeat(repeat), 
    texture_path(""), 
    texture(Texture2D{0}), 
    _is_texture_loaded(false),
    head_offset(mr::ivec2{(int)ceil(width / 2.0f) - 1, (int)ceil(height / 2.0f) - 1}) 
  {
    _preview_rectangle = Rectangle{
      0, 
      0, 
      width*16.0f, 
      height*16.0f
    };

    switch (type) {
      case TileDefType::box:
      _preview_rectangle.y = 20 * width * height + 20  *(height + (buffer*2));
      break;

      case TileDefType::voxel_struct_rock_type:
      case TileDefType::voxel_struct_sand_type:
      _preview_rectangle.y = 20 * (height + (buffer * 2));
      break;

      default:
      _preview_rectangle.y = 20 * (height + (buffer * 2)) * repeat.size();
      break;
    }
}

TileDef::~TileDef() {
  unload_texture();
}

}; // namespace mr
