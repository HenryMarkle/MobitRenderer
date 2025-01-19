#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <filesystem>

#include <raylib.h>

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
  const std::vector<std::string> tags;
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

  inline const std::vector<std::string> &get_tags() const noexcept { return tags; }
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
          uint8_t buffer, int8_t rnd, std::vector<std::string> tags,
          std::vector<int8_t> specs, std::vector<int8_t> specs2,
          std::vector<int8_t> specs3, std::vector<uint8_t> repeat);

  ~TileDef();
};

/// TODO: More fields to add
class MaterialDef {
private:
  const std::string name;
  const std::string category;
  const Color color;

public:
  const std::string &get_name() const;

  MaterialDef(std::string const &name, std::string const &category,
              Color color);
};
}; // namespace mr
