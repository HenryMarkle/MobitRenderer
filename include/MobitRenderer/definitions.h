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

  /// Auto-calculated
  const ivec2 head_offset;

  Texture2D texture;

public:
  const std::string &get_name() const noexcept;
  TileDefType get_type() const noexcept;

  const std::string &get_category() const noexcept;
  void set_category(std::string) noexcept;

  int get_width() const;
  int get_height() const;
  int get_buffer() const;

  inline int calculate_width(int scale = 1) const { return (width + (buffer * 2)) * scale; }
  inline int calculate_height(int scale = 1) const { return (height + (buffer * 2)) * scale; }

  ivec2 get_head_offset() const;

  const std::vector<int8_t> &get_specs() const;
  const std::vector<int8_t> &get_specs2() const;
  const std::vector<int8_t> &get_specs3() const;

  const std::vector<uint8_t> &get_repeat() const;

  const std::vector<std::string> &get_tags() const;
  int get_rnd() const;

  void set_texture_path(std::filesystem::path) noexcept;
  const std::filesystem::path &get_texture_path() const noexcept;

  void reload_texture();
  void unload_texture();
  
  const Texture2D &get_texture() const noexcept;

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
