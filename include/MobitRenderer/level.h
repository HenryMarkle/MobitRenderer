#pragma once

#include <math.h>
#include <cstdint>
#include <string>
#include <vector>
#include <filesystem>

#include <raylib.h>

#include <MobitRenderer/matrix.h>

namespace mr {

typedef uint16_t levelsize;
typedef uint32_t levelpixelsize;

class LevelCamera {

private:

  int   top_left_angle,  top_right_angle,  bottom_right_angle,  bottom_left_angle;
  float top_left_radius, top_right_radius, bottom_right_radius, bottom_left_radius;

  Vector2 position;

  Vector2 _tl_origin, _tr_origin, _br_origin, _bl_origin;
  Vector2 _tl_point,  _tr_point,  _br_point,  _bl_point;

  Rectangle _outer_rect, _inner_rect;

  inline void _calculate_tl(int radius_scale = 100) noexcept {
    _tl_origin = position;
    
    const float scaled_radius = top_left_radius * radius_scale;
    const float rotated_angle = (top_left_angle - 90) * PI / 180.0f;

    _tl_point = Vector2{
      _tl_origin.x + scaled_radius * (float)cos(rotated_angle),
      _tl_origin.y + scaled_radius * (float)sin(rotated_angle)
    }; 
  }
  inline void _calculate_tr(int radius_scale = 100) noexcept {
    _tr_origin = Vector2{position.x + pixel_width, position.y};
    
    const float scaled_radius = top_right_radius * radius_scale;
    const float rotated_angle = (top_right_angle - 90) * PI / 180.0f;

    _tr_point = Vector2{
      _tr_origin.x + scaled_radius * (float)cos(rotated_angle),
      _tr_origin.y + scaled_radius * (float)sin(rotated_angle)
    }; 
  }
  inline void _calculate_br(int radius_scale = 100) noexcept {
    _br_origin = Vector2 {position.x + pixel_width, position.y + pixel_height};
    
    const float scaled_radius = bottom_right_radius * radius_scale;
    const float rotated_angle = (bottom_right_angle - 90) * PI / 180.0f;

    _br_point = Vector2{
      _br_origin.x + scaled_radius * (float)cos(rotated_angle),
      _br_origin.y + scaled_radius * (float)sin(rotated_angle)
    }; 
  }
  inline void _calculate_bl(int radius_scale = 100) noexcept {
    _bl_origin = Vector2 {position.x, position.y + pixel_height};
    
    const float scaled_radius = bottom_left_radius * radius_scale;
    const float rotated_angle = (bottom_left_angle - 90) * PI / 180.0f;

    _bl_point = Vector2{
      _bl_origin.x + scaled_radius * (float)cos(rotated_angle),
      _bl_origin.y + scaled_radius * (float)sin(rotated_angle)
    }; 
  }
  void _calculate_rects() noexcept;

public:

  static const int pixel_width = 1400;
  static const int pixel_height = 800;

  inline const Vector2 &get_position() const noexcept { return position; }
  inline void set_position(Vector2 v) noexcept { 
    position = v; 

    _calculate_tl();
    _calculate_tr();
    _calculate_br();
    _calculate_bl();
    _calculate_rects();
  }

  inline const int get_top_left_angle() const noexcept { return top_left_angle; }
  inline const int get_top_right_angle() const noexcept { return top_right_angle; }
  inline const int get_bottom_right_angle() const noexcept { return bottom_right_angle; }
  inline const int get_bottom_left_angle() const noexcept { return bottom_left_angle; }
  
  inline void set_top_left_angle(int angle) noexcept { top_left_angle = angle; _calculate_tl(); _calculate_rects(); }
  inline void set_top_right_angle(int angle) noexcept { top_right_angle = angle; _calculate_tr(); _calculate_rects(); }
  inline void set_bottom_right_angle(int angle) noexcept { bottom_right_angle = angle; _calculate_br(); _calculate_rects(); }
  inline void set_bottom_left_angle(int angle) noexcept { bottom_left_angle = angle; _calculate_bl(); _calculate_rects(); }

  inline const float get_top_left_radius() const noexcept { return top_left_radius; }
  inline const float get_top_right_radius() const noexcept { return top_right_radius; }
  inline const float get_bottom_right_radius() const noexcept { return bottom_right_radius; }
  inline const float get_bottom_left_radius() const noexcept { return bottom_left_radius; }

  inline void set_top_left_radius(float radius) noexcept { top_left_radius = radius; _calculate_tl(); _calculate_rects(); }
  inline void set_top_right_radius(float radius) noexcept { top_right_radius = radius; _calculate_tr(); _calculate_rects(); }
  inline void set_bottom_right_radius(float radius) noexcept { bottom_right_radius = radius; _calculate_br(); _calculate_rects(); }
  inline void set_bottom_left_radius(float radius) noexcept { bottom_left_radius = radius; _calculate_bl(); _calculate_rects(); }

  inline void set_top_left_point(Vector2 point, int radius_scale = 100) noexcept {
    top_left_radius = Vector2Distance(point, _tl_origin) / radius_scale;
    top_left_angle = bottom_right_angle = Vector2Angle(
      Vector2{_tl_origin.x, _tl_origin.y - 1} - _tl_origin,
      point - _tl_origin
    ) * 180.0f / PI;

    _calculate_tl(); 
    _calculate_rects();
  }
  inline void set_top_right_point(Vector2 point, int radius_scale = 100) noexcept {
    top_right_radius = Vector2Distance(point, _tr_origin) / radius_scale;
    top_right_angle = bottom_right_angle = Vector2Angle(
      Vector2{_tr_origin.x, _tr_origin.y - 1} - _tr_origin,
      point - _tr_origin
    ) * 180.0f / PI;

    _calculate_tr(); 
    _calculate_rects();
  }
  inline void set_bottom_right_point(Vector2 point, int radius_scale = 100) noexcept {
    bottom_right_radius = Vector2Distance(point, _br_origin) / radius_scale;
    bottom_right_angle = Vector2Angle(
      Vector2{_br_origin.x, _br_origin.y - 1} - _br_origin,
      point - _br_origin
    ) * 180.0f / PI;

    _calculate_br(); 
    _calculate_rects();
  }
  inline void set_bottom_left_point(Vector2 point, int radius_scale = 100) noexcept {
    bottom_left_radius = Vector2Distance(point, _bl_origin) / radius_scale;
    bottom_left_angle = Vector2Angle(
      Vector2{_bl_origin.x, _bl_origin.y - 1} - _bl_origin, 
      point - _bl_origin
    ) * 180.0f / PI;

    _calculate_bl(); 
    _calculate_rects();
  }

  inline const Vector2 &get_top_left_origin() const noexcept { return _tl_origin; }
  inline const Vector2 &get_top_right_origin() const noexcept { return _tr_origin; }
  inline const Vector2 &get_bottom_right_origin() const noexcept { return _br_origin; }
  inline const Vector2 &get_bottom_left_origin() const noexcept { return _bl_origin; }

  inline const Vector2 &get_top_left_point() const noexcept { return _tl_point; }
  inline const Vector2 &get_top_right_point() const noexcept { return _tr_point; }
  inline const Vector2 &get_bottom_right_point() const noexcept { return _br_point; }
  inline const Vector2 &get_bottom_left_point() const noexcept { return _bl_point; }

  void reset_angles() noexcept;
  void reset_radius() noexcept;
  void reset() noexcept;

  inline const Rectangle &get_outer_rect() const noexcept { return _outer_rect; }
  inline const Rectangle &get_inner_rect() const noexcept { return _inner_rect; }

  LevelCamera();
  LevelCamera(Vector2);
};

typedef LevelCamera levelcamera;

struct EffectConfig {
  std::string name;
  std::vector<std::string> options;

  /// @brief Determines the type of the choice field.
  uint8_t choice_type;

  /// @brief If choice_type is 0, then choice is an index to options pointing to
  /// the choice; otherwise, choice is just a number.
  uint8_t choice;
};

struct Effect {
  std::string name;
  std::vector<EffectConfig> config;
  Matrix<float> matrix;

  Effect &operator=(const Effect &) noexcept = delete;
  Effect(const Effect &) = delete;
};

struct BufferGeos {
  levelsize left, top, right, bottom;

  BufferGeos();
  BufferGeos(
    levelsize left, 
    levelsize top, 
    levelsize right, 
    levelsize bottom
  );
};

/// @brief Stores data of a level project
/// @attention Resizing and destruction requires OpenGL context.
class Level {
private:

  std::string name;
  std::filesystem::path path, directory_path, lightmap_path;

  levelsize width, height;
  levelpixelsize pxwidth, pxheight;

  Matrix<GeoCell> geo_matrix;
  Matrix<TileCell> tile_matrix;
  std::vector<Effect> effects;

  RenderTexture2D lightmap;

public:

  BufferGeos buffer_geos;
  int water;
  bool light, terrain, front_water;
  std::vector<LevelCamera> cameras;
  std::vector<std::shared_ptr<Prop>> props;
  int seed;
  std::string default_material;
  
  /// @brief 0 - 360
  int light_angle;

  /// @brief 1 - 10
  int light_flatness;

  const std::string &get_name() const noexcept;

  /// @note This will update the path, directory 
  /// and the path of the lightmap.
  void set_name(std::string);

  /// @brief Gets the file path of the level.
  const std::filesystem::path &get_path() const noexcept;

  /// @note This will update the name, path, directory 
  /// and the path of the lightmap.
  void set_path(const std::filesystem::path&);

  /// @brief Gets the directory path of the level.
  const std::filesystem::path &get_directory() const noexcept;
  
  /// @note This will update the path, directory 
  /// and the path of the lightmap.
  void set_directory(const std::filesystem::path&);

  /// @brief Gets the file path of the lightmap.
  const std::filesystem::path &get_lightmap_path() const noexcept;

  const RenderTexture2D &get_lightmap() const noexcept { return lightmap; }

  /// @return Retrieves the level width in units of 20 px^2.
  levelsize get_width() const noexcept;
  /// @return Retrieves the level height in units of 20 px^2.
  levelsize get_height() const noexcept;

  /// @return Retrieves the level width in pixels.
  levelpixelsize get_pixel_width() const noexcept;
  /// @return Retrieves the level height in pixels.
  levelpixelsize get_pixel_height() const noexcept;

  Matrix<GeoCell> &get_geo_matrix();
  Matrix<TileCell> &get_tile_matrix();
  std::vector<Effect> &get_effects();

  /// @note Requires a GL context.
  void load_lightmap();
  void load_lightmap(const std::filesystem::path&);

  /// @note Requires a GL context.
  void unload_lightmap();

  const Matrix<GeoCell> &get_const_geo_matrix() const;
  const Matrix<TileCell> &get_const_tile_matrix() const;
  const std::vector<Effect> &get_const_effects() const;

  void resize(int16_t left, int16_t top, int16_t right, int16_t bottom);

  Level() = delete;

  Level(uint16_t width, uint16_t height);

  /// @brief Constructs a level from the provided arguments.
  /// @param width The width of the level in units of 20 pixels.
  /// @param height The height of the level in units of 20 pixels.
  /// @param geo_matrix The geometry matrix.
  /// @param tile_matrix The tile matrix.
  /// @param effects The effects list.
  /// @param cameras The cameras list.
  /// @param lightmap The lightmap texture.
  /// @param water The water level (-1 means no water).
  /// @param light
  /// @param terrain Determines whether the level has a "floor".
  /// @param front_water Determines whether the water should be displayed before
  /// all geometry layers.
  /// @note This constructor does not unload the lightmap texture passed to it.
  Level(uint16_t width, uint16_t height,

        Matrix<GeoCell> &&geo_matrix, Matrix<TileCell> &&tile_matrix,
        std::vector<Effect> &&ffects, std::vector<LevelCamera> &&cameras,

        int8_t water = -1, bool light = true, bool terrain = false,
        bool front_water = false);
  ~Level();
};
}; // namespace mr
