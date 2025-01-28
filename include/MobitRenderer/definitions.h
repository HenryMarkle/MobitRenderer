#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <map>
#include <filesystem>
#include <unordered_set>

#include <raylib.h>

#include <MobitRenderer/managed.h>
#include <MobitRenderer/quad.h>
#include <MobitRenderer/vec.h>

namespace mr {

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
  Color color;
  const TileDefType type;
  const uint8_t width, height, buffer;
  const int8_t rnd;
  const std::vector<int8_t> specs, specs2, specs3;
  const bool multilayer;
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

  inline const Color get_color() const noexcept { return color; }
  inline void set_color(Color c) noexcept { color = c; }

  inline int get_width() const noexcept { return width; }
  inline int get_height() const noexcept { return height; }
  inline int get_buffer() const noexcept { return buffer; }
  
  inline int get_buffered_width() const noexcept { return width + 2*buffer; }
  inline int get_buffered_height() const noexcept { return height + 2*buffer; }

  inline int calculate_width(int scale = 20) const { return (width + (buffer * 2)) * scale; }
  inline int calculate_height(int scale = 20) const { return (height + (buffer * 2)) * scale; }

  inline ivec2 get_head_offset() const noexcept { return head_offset; }

  inline const std::vector<int8_t> &get_specs() const { return specs; }
  inline const std::vector<int8_t> &get_specs2() const { return specs2; }
  inline const std::vector<int8_t> &get_specs3() const { return specs3; }

  inline bool is_multilayer() const noexcept { return multilayer; }

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

struct PropDefCategory {
  std::string name;
  Color color;
};

enum class PropType : uint8_t { 
  standard, 
  varied_standard, 
  soft, 
  varied_soft,
  colored_soft,
  soft_effect,
  decal,
  varied_decal,
  _long,
  rope,
  antimatter
};

class PropDef {

protected:
  
  std::filesystem::path texture_path;
  
  bool loaded;
  Texture2D texture;

public:

  const uint8_t depth; // 0 - 29
  const PropType type;
  const std::string name;
  const std::unordered_set<std::string> tags;

  inline const std::filesystem::path &get_texture_path() const noexcept { return texture_path; }
  inline void set_texture_path(const std::filesystem::path &path) noexcept { texture_path = path; }
  
  inline bool is_loaded() const noexcept { return loaded; }
  void load_texture();
  void unload_texture();
  inline void reload_texture() { unload_texture(); load_texture(); }

  PropDef &operator=(PropDef&&) noexcept = delete;
  PropDef &operator=(PropDef const&) = delete;

  PropDef(uint8_t depth, std::string &&name, PropType type);
  PropDef(uint8_t depth, std::string &&name, PropType type, std::unordered_set<std::string> &&tags);
  PropDef(PropDef&&) noexcept = delete;
  PropDef(PropDef const&) = delete;
  
  virtual ~PropDef();
};

enum class PropColorTreatment { standard, bevel };

class Standard : public PropDef {

public:

  const uint16_t width, height;
  const std::vector<uint8_t> repeat;
  const PropColorTreatment color_treatment;
  const int bevel;

  Standard &operator=(Standard&&) noexcept = delete;
  Standard &operator=(Standard const&) = delete;

  Standard(
    uint8_t depth, 
    std::string &&name, 
    uint16_t width, 
    uint16_t height, 
    std::vector<uint8_t> &&repeat,
    PropColorTreatment color_treatment,
    int bevel
  );
  Standard(
    uint8_t depth, 
    std::string &&name, 
    std::unordered_set<std::string> &&tags,
    uint16_t width, 
    uint16_t height, 
    std::vector<uint8_t> &&repeat,
    PropColorTreatment color_treatment,
    int bevel
  );

  Standard(Standard&&) noexcept = delete;
  Standard(Standard const&) = delete;
};

class VariedStandard : public PropDef {

public:

  const uint16_t width, height;
  const std::vector<uint8_t> repeat;
  const uint8_t variations;
  const bool random;
  const PropColorTreatment color_treatment;
  const bool colorize;
  const int bevel;

  VariedStandard &operator=(VariedStandard&&) noexcept = delete;
  VariedStandard &operator=(VariedStandard const&) = delete;

  VariedStandard(
    uint8_t depth, 
    std::string &&name, 
    uint16_t width, 
    uint16_t height, 
    std::vector<uint8_t> &&repeat,
    uint8_t variations,
    bool random,
    PropColorTreatment color_treatment,
    bool colorize,
    int bevel
  );
  VariedStandard(
    uint8_t depth, 
    std::string &&name, 
    std::unordered_set<std::string> &&tags,
    uint16_t width, 
    uint16_t height, 
    std::vector<uint8_t> &&repeat,
    uint8_t variations,
    bool random,
    PropColorTreatment color_treatment,
    bool colorize,
    int bevel
  );

  VariedStandard(VariedStandard&&) noexcept = delete;
  VariedStandard(VariedStandard const&) = delete;
};

class Decal : public PropDef {

public:

  Decal &operator=(Decal&&) noexcept = delete;
  Decal &operator=(Decal const&) = delete;

  Decal(uint8_t depth, std::string &&name);
  Decal(uint8_t depth, std::string &&name, std::unordered_set<std::string> &&tags);
  Decal(Decal&&) noexcept = delete;
  Decal(Decal const&) = delete;

};

class VariedDecal : public PropDef {

public:

  /// @brief The width of a single variation in pixels.
  const uint32_t pixel_width;
  
  /// @brief The height of a single variation in pixels.
  const uint32_t pixel_height;
  
  const uint8_t variations;
  const bool random;

  VariedDecal &operator=(VariedDecal&&) noexcept = delete;
  VariedDecal &operator=(VariedDecal const&) = delete;

  VariedDecal(
    uint8_t depth, 
    std::string &&name, 
    uint32_t pixel_width,
    uint32_t pixel_height,
    uint8_t variations,
    bool random
  );
  VariedDecal(
    uint8_t depth, 
    std::string &&name, 
    std::unordered_set<std::string> &&tags,
    uint32_t pixel_width,
    uint32_t pixel_height,
    uint8_t variations,
    bool random
  );

  VariedDecal(VariedDecal&&) noexcept = delete;
  VariedDecal(VariedDecal const&) = delete;
};

class Soft : public PropDef {

public:

  const bool round, self_shade;
  const uint32_t smooth_shading; // cannot be zero
  const float contour_exp, highlight_border, depth_affect_hilites, shadow_border;


  Soft &operator=(Soft&&) noexcept = delete;
  Soft &operator=(Soft const&) = delete;

  Soft(
    uint8_t depth, 
    std::string &&name,
    uint32_t smooth_shading,
    float contour_exp, 
    float highlight_border, 
    float depth_affect_hilites, 
    float shadow_border,
    bool round, 
    bool self_shade
  );
  Soft(
    uint8_t depth, 
    std::string &&name, 
    std::unordered_set<std::string> &&tags,
    uint32_t smooth_shading,
    float contour_exp, 
    float highlight_border, 
    float depth_affect_hilites, 
    float shadow_border,
    bool round, 
    bool self_shade
  );
  Soft(Soft&&) noexcept = delete;
  Soft(Soft const&) = delete;
};

class VariedSoft : public PropDef {

public:

  const bool round, self_shade, colorize, random;
  const uint8_t variations;
  const uint32_t pixel_width, pixel_height;
  const uint32_t smooth_shading; // cannot be zero
  const float contour_exp, highlight_border, depth_affect_hilites, shadow_border;

  VariedSoft &operator=(VariedSoft&&) noexcept = delete;
  VariedSoft &operator=(VariedSoft const&) = delete;

  VariedSoft(
    uint8_t depth, 
    std::string &&name, 
    uint32_t pixel_width,
    uint32_t pixel_height,
    uint8_t variations,
    bool random,
    bool colorize,
    uint32_t smooth_shading,
    float contour_exp, 
    float highlight_border, 
    float depth_affect_hilites, 
    float shadow_border,
    bool round, 
    bool self_shade
  );
  VariedSoft(
    uint8_t depth, 
    std::string &&name, 
    std::unordered_set<std::string> &&tags,
    uint32_t pixel_width,
    uint32_t pixel_height,
    uint8_t variations,
    bool random,
    bool colorize,
    uint32_t smooth_shading,
    float contour_exp, 
    float highlight_border, 
    float depth_affect_hilites, 
    float shadow_border,
    bool round, 
    bool self_shade
  );

  VariedSoft(VariedSoft&&) noexcept = delete;
  VariedSoft(VariedSoft const&) = delete;

};

class ColoredSoft : public PropDef {

public:

  const bool round, self_shade, colorize;
  const uint32_t smooth_shading; // cannot be zero
  const uint32_t pixel_width, pixel_height;
  const float contour_exp, highlight_border, depth_affect_hilites, shadow_border;

  ColoredSoft &operator=(ColoredSoft&&) noexcept = delete;
  ColoredSoft &operator=(ColoredSoft const&) = delete;

  ColoredSoft(
    uint8_t depth, 
    std::string &&name, 
    uint32_t pixel_width,
    uint32_t pixel_height,
    bool colorize,
    uint32_t smooth_shading,
    float contour_exp, 
    float highlight_border, 
    float depth_affect_hilites, 
    float shadow_border,
    bool round, 
    bool self_shade
  );
  ColoredSoft(
    uint8_t depth, 
    std::string &&name, 
    std::unordered_set<std::string> &&tags,
    uint32_t pixel_width,
    uint32_t pixel_height,
    bool colorize,
    uint32_t smooth_shading,
    float contour_exp, 
    float highlight_border, 
    float depth_affect_hilites, 
    float shadow_border,
    bool round, 
    bool self_shade
  );
  ColoredSoft(ColoredSoft&&) noexcept = delete;
  ColoredSoft(ColoredSoft const&) = delete;

};

class SoftEffect : public PropDef {
  public:

  SoftEffect &operator=(SoftEffect&&) noexcept = delete;
  SoftEffect &operator=(SoftEffect const&) = delete;

  SoftEffect(uint8_t depth, std::string &&name);
  SoftEffect(uint8_t depth, std::string &&name, std::unordered_set<std::string> &&tags);
  SoftEffect(SoftEffect&&) noexcept = delete;
  SoftEffect(SoftEffect const&) = delete;
};

class Long : public PropDef {

public:

  Long &operator=(Long&&) noexcept = delete;
  Long &operator=(Long const&) = delete;

  Long(uint8_t depth, std::string &&name);
  Long(uint8_t depth, std::string &&name, std::unordered_set<std::string> &&tags);
  Long(Long&&) noexcept = delete;
  Long(Long const&) = delete;
};

class Rope : public PropDef {

public:

  const int segment_length;
  const int collision_depth;
  const float segment_radius;
  const float gravity;
  const float friction, air_friction;
  const bool stiff;
  const float edge_direction;
  const float rigid;
  const float self_push, source_push;

  Rope &operator=(Rope&&) noexcept = delete;
  Rope &operator=(Rope const&) = delete;

  Rope(uint8_t depth, std::string &&name,
    int segment_length, int collision_depth, float segment_radius, float gravity, float friction, float air_friction,
    bool stiff, float edge_direction, float rigid, float self_push, float source_push
  );

  Rope(uint8_t depth, std::string &&name, std::unordered_set<std::string> &&tags,
    int segment_length, int collision_depth, float segment_radius, float gravity, float friction, float air_friction,
    bool stiff, float edge_direction, float rigid, float self_push, float source_push
  );

  Rope(Rope&&) noexcept = delete;
  Rope(Rope const&) = delete;

};

class Antimatter : public PropDef {

public:

  const float contour_exp;

  Antimatter &operator=(Antimatter&&) noexcept = delete;
  Antimatter &operator=(Antimatter const&) = delete;

  Antimatter(uint8_t depth, std::string &&name, float contour_exp);
  Antimatter(uint8_t depth, std::string &&name, std::unordered_set<std::string> &&tags, float contour_exp);
  Antimatter(Antimatter&&) noexcept = delete;
  Antimatter(Antimatter const&) = delete;

};

enum class RopeRelease : int8_t { left = -1, none = 0, right = 1 };

struct PropSettings {
  int render_order, seed, render_time;

  int variation;

  int custom_depth;
  bool apply_color;
  
  RopeRelease release;
  float thickness;
  std::vector<Vector2> segments;

  PropSettings(int render_order, int seed, int render_time);

  static PropSettings standard(int render_order, int seed, int render_time);
  static PropSettings varied_standard(int render_order, int seed, int render_time, int variation);
  static PropSettings long_(int render_order, int seed, int render_time);
  static PropSettings soft(
    int render_order, 
    int seed, 
    int render_time, 
    int custom_depth
  );
  static PropSettings varied_soft(
    int render_order, 
    int seed, 
    int render_time, 
    int variation, 
    int custom_depth, 
    bool apply_color = false
  );
  static PropSettings colored_soft(
    int render_order, 
    int seed, 
    int render_time
  );
  static PropSettings soft_effect(
    int render_order, 
    int seed, 
    int render_time, 
    int custom_depth
  );
  static PropSettings decal(
    int render_order, 
    int seed, 
    int render_time, 
    int custom_depth
  );
  static PropSettings varied_decal(
    int render_order, 
    int seed, 
    int render_time, 
    int variation, 
    int custom_depth
  );
  static PropSettings rope(
    int render_order, 
    int seed, 
    int render_time, 
    RopeRelease release,
    std::vector<Vector2> const &segments,
    float thickness,
    bool apply_color = false
  );
  static PropSettings rope(
    int render_order, 
    int seed, 
    int render_time, 
    RopeRelease release,
    std::vector<Vector2> &&segments,
    float thickness,
    bool apply_color = false
  );
  static PropSettings antimatter(
    int render_order, 
    int seed, 
    int render_time, 
    int custom_depth
  );
};

struct Prop {
  
// private:

  std::string und_name;
  PropDef *prop_def;
  TileDef *tile_def;
  Quad quad;
  std::unique_ptr<PropSettings> settings;

// public:

//   inline const std::string &get_name() const noexcept { return und_name; }
//   inline void define(PropDef *def) noexcept { prop_def = def; tile_def = nullptr; }
//   inline void define(TileDef *def) noexcept { prop_def = nullptr; tile_def = def; }
//   inline Quad &get_quad() noexcept { return quad; }
//   inline PropSettings *get_settings() noexcept { return settings.get(); }
//   inline void set_settings(PropSettings &&_settings) { settings = std::make_unique<PropSettings>(_settings); }
//   inline PropDef *get_prop_def() noexcept { return prop_def; }
//   inline TileDef *get_tile_def() noexcept { return tile_def; }

  Prop(std::string const&, Quad const&);
  Prop(std::string&&, Quad const&);
  Prop(PropDef*, Quad const&);
  Prop(TileDef*, Quad const&);
};

}; // namespace mr
