#include <filesystem>

#include <string>

#include <MobitRenderer/definitions.h>

// To be used in unordered maps and sets
namespace std {
    template<>
    struct hash<mr::MaterialDef> {
        std::size_t operator()(const mr::MaterialDef& t) const {
            return std::hash<std::string>{}(t.get_name());
        }
    };
};

namespace mr {
  MaterialDef::MaterialDef(
    std::string const &name, 
    Color color,
    MaterialRenderType type
  ) :
    name(name),
    color(color),
    type(type) 
  { }

  MaterialDef::MaterialDef(
    std::string const &name, 
    std::string const &category, 
    Color color,
    MaterialRenderType type
  ) :
    name(name),
    category(category),
    color(color),
    type(type) 
  { }

  bool CustomMaterialDef::are_textures_loaded() const noexcept {
    return 
      (texture_params == nullptr || main_texture.is_loaded())
      &&
      (block_params == nullptr || block_texture.is_loaded())
      &&
      (slope_params == nullptr || slope_texture.is_loaded())
      &&
      (floor_params == nullptr || floor_texture.is_loaded());
  }

  void CustomMaterialDef::reload_textures() {
    if (texture_params != nullptr) {
      main_texture = texture(main_texture_path.string().c_str());
    }

    if (block_params != nullptr) {
      block_texture = texture(block_texture_path.string().c_str());
    }

    if (slope_params != nullptr) {
      slope_texture = texture(slope_texture_path.string().c_str());
    }

    if (floor_params != nullptr) {
      floor_texture = texture(floor_texture_path.string().c_str());
    }
  }

  MaterialDefTexture::MaterialDefTexture(
    int width, 
    int height, 
    std::vector<int> repeat,
    std::unordered_set<std::string> tags
  ) : 
    width(width), 
    height(height), 
    repeat(repeat), 
    tags(tags) 
  {}

  MaterialDefBlock::MaterialDefBlock(
    std::vector<int> repeat,
    int rnd, 
    int buffer,
    std::unordered_set<std::string> tags
  ) :
    repeat(repeat),
    rnd(rnd),
    buffer(buffer),
    tags(tags) 
  {}

  MaterialDefSlope::MaterialDefSlope(
    std::vector<int> repeat,
    int rnd,
    int buffer,
    std::unordered_set<std::string> tags
  ) :
    repeat(repeat),
    rnd(rnd),
    buffer(buffer),
    tags(tags) 
  {}

  MaterialDefFloor::MaterialDefFloor(
    std::vector<int> repeat,
    int rnd,
    int buffer,
    std::unordered_set<std::string> tags
  ) :
    repeat(repeat),
    rnd(rnd),
    buffer(buffer),
    tags(tags) 
  {}

  CustomMaterialDef::CustomMaterialDef(
    std::string const &name, 
    Color color,
    MaterialDefTexture *texture_params,
    MaterialDefBlock *block_params,
    MaterialDefSlope *slope_params,
    MaterialDefFloor *floor_params
  ) : 
    MaterialDef(
      name, 
      color, 
      MaterialRenderType::custom_unified
    ),
    texture_params(texture_params),
    block_params(block_params),
    slope_params(slope_params),
    floor_params(floor_params) 
  {}

  CustomMaterialDef::~CustomMaterialDef() {}
};