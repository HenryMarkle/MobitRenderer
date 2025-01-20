#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <map>
#include <filesystem>
#include <unordered_set>

#include <raylib.h>

#include <MobitRenderer/managed.h>

namespace mr {

typedef struct IVector2 {
  int x, y;
} ivec2;

struct TileDefCategory {
  std::string name;
  Color color;
};

enum TileDefType : uint8_t {
  box,
  voxel_struct,
  voxel_struct_random_displace_horizontal,
  voxel_struct_random_displace_vertical,
  voxel_struct_rock_type,
  voxel_struct_sand_type
};

class TileDef {
private:
  const std::string name;
  std::string category;
  const TileDefType type;
  const uint8_t width, height, buffer;
  const int8_t rnd;
  const std::vector<int8_t> specs, specs2, specs3;
  const std::vector<uint8_t> repeat;
  const std::unordered_set<std::string> tags;
  std::filesystem::path texture_path;
  Rectangle _preview_rectangle;

  /// Auto-calculated
  const ivec2 head_offset;

  bool _is_texture_loaded;

  Texture2D texture;

public:
  inline const std::string &get_name() const noexcept { return name; }
  inline TileDefType get_type() const noexcept { return type; }

  inline const std::string &get_category() const noexcept { return category; }
  inline void set_category(std::string new_category) noexcept { category = new_category; }

  inline int get_width() const noexcept { return width; }
  inline int get_height() const noexcept { return height; }
  inline int get_buffer() const noexcept { return buffer; }

  inline int calculate_width(int scale = 1) const { return (width + (buffer * 2)) * scale; }
  inline int calculate_height(int scale = 1) const { return (height + (buffer * 2)) * scale; }

  inline ivec2 get_head_offset() const noexcept { return head_offset; }

  inline const std::vector<int8_t> &get_specs() const { return specs; }
  inline const std::vector<int8_t> &get_specs2() const { return specs2; }
  inline const std::vector<int8_t> &get_specs3() const { return specs3; }

  inline const std::vector<uint8_t> &get_repeat() const noexcept { return repeat; }

  inline const std::unordered_set<std::string> &get_tags() const noexcept { return tags; }
  inline int get_rnd() const noexcept { return rnd; }

  /// @brief Retrieves the rectangle to draw the preview section of the texture.
  inline Rectangle get_preview_rectangle() const noexcept { return _preview_rectangle; }

  inline void set_texture_path(std::filesystem::path p) noexcept { texture_path = p; }
  inline const std::filesystem::path &get_texture_path() const noexcept { return texture_path; }

  inline bool is_texture_loaded() const noexcept { return _is_texture_loaded; }
  void reload_texture();
  void unload_texture();
  
  inline const Texture2D &get_texture() const noexcept { return texture; }
  
  /// @brief Loads the tile texture before accessing the texture.
  inline const Texture2D &get_loaded_texture() {
    if (!_is_texture_loaded) reload_texture();
    return texture;
  };

  TileDef &operator=(TileDef &&) noexcept = delete;
  TileDef &operator=(const TileDef &) = delete;

  TileDef(TileDef &&) noexcept = delete;
  TileDef(const TileDef &) = delete;

  TileDef(std::string name, TileDefType type, uint8_t width, uint8_t height,
          uint8_t buffer, int8_t rnd, std::unordered_set<std::string> tags,
          std::vector<int8_t> specs, std::vector<int8_t> specs2,
          std::vector<int8_t> specs3, std::vector<uint8_t> repeat);

  ~TileDef();
};

enum class MaterialRenderType {
  unified,
  tiles,
  pipe,
  invisible,
  large_trash,
  dirt,
  ceramic,
  dense_pipe,
  ridge,
  ceramic_a,
  ceramic_b,
  random_pipes,
  rock,
  rough_rock,
  sandy,
  mega_trash,
  wv,

  custom_unified
};

class MaterialDef {

protected:

  const std::string name;
  std::string category;
  const Color color;
  const MaterialRenderType type;

public:

  inline const std::string &get_name() const noexcept { return name; }
  inline const std::string &get_category() const noexcept { return category; }
  inline void set_category(std::string new_category) noexcept { category = new_category; }
  inline Color get_color() const noexcept { return color; }
  inline MaterialRenderType get_type() const noexcept { return type; }

  MaterialDef(
    std::string const &name, 
    Color color,
    MaterialRenderType type
  );

  MaterialDef(
    std::string const &name, 
    std::string const &category, 
    Color color,
    MaterialRenderType type
  );

  virtual ~MaterialDef() = default;
};

struct MaterialDefTexture {
  uint16_t width, height;
  std::vector<uint8_t> repeat;
  std::unordered_set<std::string> tags;

  MaterialDefTexture(
    uint16_t, 
    uint16_t, 
    std::vector<uint8_t>,
    std::unordered_set<std::string>
  );
};

struct MaterialDefBlock {
  std::vector<uint8_t> repeat;
  int rnd;
  uint8_t buffer;
  std::unordered_set<std::string> tags;

  MaterialDefBlock(
    std::vector<uint8_t>,
    int, 
    uint8_t,
    std::unordered_set<std::string>
  );
};

struct MaterialDefSlope {
  std::vector<uint8_t> repeat;
  int rnd;
  uint8_t buffer;
  std::unordered_set<std::string> tags;

  MaterialDefSlope(
    std::vector<uint8_t>,
    int,
    uint8_t,
    std::unordered_set<std::string>
  );
};

struct MaterialDefFloor {
  std::vector<uint8_t> repeat;
  int rnd;
  uint8_t buffer;
  std::unordered_set<std::string> tags;

  MaterialDefFloor(
    std::vector<uint8_t>,
    int,
    uint8_t,
    std::unordered_set<std::string>
  );
};

class CustomMaterialDef : public MaterialDef {

private:

  const MaterialDefTexture *texture_params;
  const MaterialDefBlock *block_params;
  const MaterialDefSlope *slope_params;
  const MaterialDefFloor *floor_params;

  std::filesystem::path 
    main_texture_path,
    block_texture_path,
    slope_texture_path,
    floor_texture_path;

  texture 
    main_texture,
    block_texture,
    slope_texture,
    floor_texture;

public:

  inline const texture &get_main_texture() const noexcept { return main_texture; }
  inline const texture &get_block_texture() const noexcept { return block_texture; }
  inline const texture &get_slope_texture() const noexcept { return slope_texture; }
  inline const texture &get_floor_texture() const noexcept { return floor_texture; }

  inline void set_textures_dir(const std::filesystem::path &dir) {
    if (texture_params != nullptr) {
      main_texture_path = dir / (name + "Texture.png");
    }

    if (block_params != nullptr) {
      block_texture_path = dir / (name + ".png");
    }

    if (slope_params != nullptr) {
      slope_texture_path = dir / (name + "Slopes.png");
    }

    if (floor_params != nullptr) {
      floor_texture_path = dir / (name + "Floor.png");
    }
  }

  bool are_textures_loaded() const noexcept;

  void reload_textures();
  
  inline void unload_textures() { 
    main_texture.unload(); 
    block_texture.unload();
    slope_texture.unload();
    floor_texture.unload();
  }

  CustomMaterialDef &operator=(CustomMaterialDef&&) noexcept = delete;
  CustomMaterialDef &operator=(CustomMaterialDef const&) = delete;

  CustomMaterialDef(
    std::string const &name, 
    Color color,
    MaterialDefTexture *texture_params = nullptr,
    MaterialDefBlock *block_params = nullptr,
    MaterialDefSlope *slope_params = nullptr,
    MaterialDefFloor *floor_params = nullptr
  );

  CustomMaterialDef(CustomMaterialDef&&) noexcept = delete;
  CustomMaterialDef(CustomMaterialDef const&) = delete;

  virtual ~CustomMaterialDef() override;

};


}; // namespace mr
