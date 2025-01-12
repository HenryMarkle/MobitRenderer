#if defined(_WIN32) || defined(_WIN64)
  #define WIN32_LEAN_AND_MEAN
#endif

#include <cstdint>
#include <vector>

#include <raylib.h>

#include <MobitRenderer/level.h>
#include <MobitRenderer/matrix.h>

mr::levelsize mr::Level::get_width() const noexcept { return width; }
mr::levelsize mr::Level::get_height() const noexcept { return height; }

mr::levelpixelsize mr::Level::get_pixel_width() const noexcept { return pxwidth; }
mr::levelpixelsize mr::Level::get_pixel_height() const noexcept { return pxheight; }

mr::Matrix<mr::GeoCell> &mr::Level::get_geo_matrix() { return geo_matrix; }
mr::Matrix<mr::TileCell> &mr::Level::get_tile_matrix() { return tile_matrix; }
std::vector<mr::Effect> &mr::Level::get_effects() { return effects; }

void mr::Level::load_lightmap(const Texture2D &texture) {
  if (lightmap.id != 0)
    UnloadRenderTexture(lightmap);

  lightmap = LoadRenderTexture(texture.width, texture.height);

  BeginTextureMode(lightmap);
  ClearBackground(WHITE);
  DrawTexture(texture, 0, 0, WHITE);
  EndTextureMode();
}
void mr::Level::unload_lightmap() {
  if (lightmap.id != 0) {
    UnloadRenderTexture(lightmap);
    lightmap.id = 0;
  }
}
void mr::Level::begin_lightmap_mode() { BeginTextureMode(lightmap); }
void mr::Level::end_lightmap_mode() { EndTextureMode(); }

const mr::Matrix<mr::GeoCell> &mr::Level::get_const_geo_matrix() const {
  return geo_matrix;
}
const mr::Matrix<mr::TileCell> &mr::Level::get_const_tile_matrix() const {
  return tile_matrix;
}
const std::vector<mr::Effect> &mr::Level::get_const_effects() const {
  return effects;
}

void mr::Level::resize(int16_t left, int16_t top, int16_t right,
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

mr::Level::Level(uint16_t width, uint16_t height)
    : width(width), height(height), pxwidth(width * 20), pxheight(height * 20), geo_matrix(width, height),
      tile_matrix(width, height), water(-1), front_water(false), light(true),
      terrain(true), lightmap(RenderTexture2D{0}) {}

mr::Level::Level(uint16_t width, uint16_t height,

                 mr::Matrix<mr::GeoCell> &&geo_matrix,
                 mr::Matrix<mr::TileCell> &&tile_matrix,
                 std::vector<mr::Effect> &&ffects,
                 std::vector<mr::LevelCamera> &&cameras,

                 int8_t water, bool light, bool terrain, bool front_water)
    : width(width), height(height), pxwidth(width * 20), pxheight(height * 20), water(water), front_water(front_water),
      light(light), terrain(terrain), tile_matrix(std::move(tile_matrix)),
      geo_matrix(std::move(geo_matrix)), lightmap(RenderTexture2D{0}) {}

mr::Level::~Level() {
  if (lightmap.id != 0)
    UnloadRenderTexture(this->lightmap);
}
