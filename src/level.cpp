#if defined(_WIN32) || defined(_WIN64)
  #define WIN32_LEAN_AND_MEAN
#endif

#include <cstdint>
#include <vector>
#include <string>
#include <filesystem>
#include <exception>

#include <raylib.h>

#include <MobitRenderer/level.h>
#include <MobitRenderer/matrix.h>

using std::string;
using std::filesystem::path;
using std::filesystem::exists;

namespace mr {

LevelCamera::LevelCamera() : 
  position({0,0}),
  top_left_angle(0),
  top_right_angle(0),
  bottom_right_angle(0),
  bottom_left_angle(0),
  top_left_radius(0),
  top_right_radius(0),
  bottom_right_radius(0),
  bottom_left_radius(0),
  _tl_origin({0, 0}),
  _tr_origin({0, 0}),
  _br_origin({0, 0}),
  _bl_origin({0, 0}),
  _tl_point({0, 0}),
  _tr_point({0, 0}),
  _br_point({0, 0}),
  _bl_point({0, 0})
{}

LevelCamera::LevelCamera(Vector2 pos) : 
  position(pos),
  top_left_angle(0),
  top_right_angle(0),
  bottom_right_angle(0),
  bottom_left_angle(0),
  top_left_radius(0),
  top_right_radius(0),
  bottom_right_radius(0),
  bottom_left_radius(0),
  _tl_origin({0, 0}),
  _tr_origin({0, 0}),
  _br_origin({0, 0}),
  _bl_origin({0, 0}),
  _tl_point({0, 0}),
  _tr_point({0, 0}),
  _br_point({0, 0}),
  _bl_point({0, 0})
{
  _calculate_tl();
  _calculate_tr();
  _calculate_br();
  _calculate_bl();
}

BufferGeos::BufferGeos() : 
  left(6), top(3), right(6), bottom(5) 
{}

BufferGeos::BufferGeos(
  levelsize left, 
  levelsize top, 
  levelsize right, 
  levelsize bottom
) :
  left(left), top(top), right(right), bottom(bottom) 
{}

const string &Level::get_name() const noexcept { return name; }
void Level::set_name(string new_name) {
  name = new_name;
  path = directory_path / (name + ".txt");
  lightmap_path = directory_path / (name + ".png");
}

const path &Level::get_path() const noexcept { return path; }
void Level::set_path(std::filesystem::path p) {
  if (!exists(p)) throw std::invalid_argument("directory path does not exist");

  auto parent = p.parent_path();

  name = p.stem().string();
  
  path = p;
  directory_path = parent;
  lightmap_path = parent / (name + ".png");
}
const path &Level::get_directory() const noexcept { return directory_path; }

void Level::set_directory(std::filesystem::path p) {
  if (!exists(p)) throw std::invalid_argument("directory path does not exist");

  directory_path = p;
  path = p / (name + ".txt");
  lightmap_path = p / (name + ".png");
}

const path &Level::get_lightmap_path() const noexcept { return lightmap_path; }

levelsize Level::get_width() const noexcept { return width; }
levelsize Level::get_height() const noexcept { return height; }

levelpixelsize Level::get_pixel_width() const noexcept { return pxwidth; }
levelpixelsize Level::get_pixel_height() const noexcept { return pxheight; }

Matrix<GeoCell> &Level::get_geo_matrix() { return geo_matrix; }
Matrix<TileCell> &Level::get_tile_matrix() { return tile_matrix; }
std::vector<Effect> &Level::get_effects() { return effects; }

void Level::load_lightmap(const Texture2D &texture) {
  if (lightmap.id != 0)
    UnloadRenderTexture(lightmap);

  lightmap = LoadRenderTexture(texture.width, texture.height);

  BeginTextureMode(lightmap);
  ClearBackground(WHITE);
  DrawTexture(texture, 0, 0, WHITE);
  EndTextureMode();
}
void Level::unload_lightmap() {
  if (lightmap.id != 0) {
    UnloadRenderTexture(lightmap);
    lightmap.id = 0;
  }
}
void Level::begin_lightmap_mode() { BeginTextureMode(lightmap); }
void Level::end_lightmap_mode() { EndTextureMode(); }

const Matrix<GeoCell> &Level::get_const_geo_matrix() const {
  return geo_matrix;
}
const Matrix<TileCell> &Level::get_const_tile_matrix() const {
  return tile_matrix;
}
const std::vector<Effect> &Level::get_const_effects() const {
  return effects;
}

void Level::resize(int16_t left, int16_t top, int16_t right,
                      int16_t bottom) {
  if (left == 0 && top == 0 && right == 0 && bottom == 0)
    return;
  if (-left == width || -right == width || -top == height || -bottom == height)
    return;

  // Update size

  width = width + left + right;
  height = height + top + bottom;
  
  pxwidth = width * 20;
  pxheight = height * 20;

  // Resize matrices

  geo_matrix.resize(left, top, right, bottom);
  tile_matrix.resize(left, top, right, bottom);

  for (auto &effect : effects) {
    effect.matrix.resize(left, top, right, bottom);
  }

  // Resize lightmap

  if (lightmap.id != 0) {
    RenderTexture2D new_lightmap = LoadRenderTexture(width, height);

    BeginTextureMode(new_lightmap);
    ClearBackground(BLACK);
    DrawTexture(lightmap.texture, 0, 0, WHITE);
    EndTextureMode();

    UnloadRenderTexture(lightmap);

    lightmap = new_lightmap;
  }
}

Level::Level(uint16_t width, uint16_t height)
    : width(width), height(height), pxwidth(width * 20), pxheight(height * 20), geo_matrix(width, height),
      tile_matrix(width, height), water(-1), front_water(false), light(true),
      terrain(true), lightmap(RenderTexture2D{0}) {}

Level::Level(uint16_t width, uint16_t height,

                Matrix<GeoCell> &&geo_matrix,
                Matrix<TileCell> &&tile_matrix,
                std::vector<Effect> &&ffects,
                std::vector<LevelCamera> &&cameras,

                int8_t water, bool light, bool terrain, bool front_water)
    : width(width), height(height), pxwidth(width * 20), pxheight(height * 20), water(water), front_water(front_water),
      light(light), terrain(terrain), tile_matrix(std::move(tile_matrix)),
      geo_matrix(std::move(geo_matrix)), lightmap(RenderTexture2D{0}) {}

Level::~Level() {
  if (lightmap.id != 0)
    UnloadRenderTexture(this->lightmap);
}

};

