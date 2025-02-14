#include <cstdint>
#include <memory>
#include <queue>
#include <string>

#include <raylib.h>

#include <MobitRenderer/definitions.h>
#include <MobitRenderer/matrix.h>
#include <MobitRenderer/state.h>
#include <MobitRenderer/level.h>
#include <MobitRenderer/atlas.h>
#include <MobitRenderer/dex.h>
#include <MobitRenderer/draw.h>
#include <MobitRenderer/sdraw.h>

namespace mr::sdraw {

void draw_tile_prevs_layer(
    const shaders* _shaders,
    Matrix<GeoCell> const &geomtx,
    Matrix<TileCell> const &tilemtx,
    uint8_t layer,
    float scale
) {
  if (layer > 2) return;

  // terrible names. I know.

  const float sxy = scale * 0.25f;
  const float ss = scale * 0.5f;
  const Shader &shader = _shaders->ink();

  for (uint16_t x = 0; x < tilemtx.get_width(); x++) {
    for (uint16_t y = 0; y < tilemtx.get_height(); y++) {
      const auto *cell = tilemtx.get_const_ptr(x, y, layer);
      if (cell == nullptr) {
        continue;
      }

      switch (cell->type) {
        case TileType::head:
        {
          auto *def = cell->tile_def;
          if (def == nullptr) {
            break;
          }
          auto &texture = def->get_loaded_texture();
          if (!def->is_texture_loaded()) {
            break;
          }

          BeginShaderMode(shader);
          SetShaderValueTexture(shader, GetShaderLocation(shader, "texture0"), texture);

          if (def->is_multilayer()) {
            mr::draw::draw_tile_prev_from_origin(
              def, 
              x*scale, 
              y*scale, 
              scale,
              def->get_color(),
              0
            );
          }
          else {
            mr::draw::draw_tile_prev_from_origin(
              def, 
              x*scale, 
              y*scale, 
              scale,
              def->get_color()
            );
          }

          EndShaderMode();
        }
        break;

        case TileType::material:
        {
          auto *def = cell->material_def;
          if (def == nullptr) break;

          auto &geocell = geomtx.get_const(x, y, layer);

          mr::draw::draw_geo_shape(
            geocell.type, 
            (x * scale) + sxy,
            (y * scale) + sxy,
            ss,
            def->get_color()
          );
        }
        break;
      }
    
      cell = tilemtx.get_const_ptr(x, y, layer - 1);

      if (
        cell != nullptr && 
        cell->type == TileType::head && 
        cell->tile_def != nullptr && 
        !cell->tile_def->get_specs2().empty()
      ) {
        auto *def = cell->tile_def;
        auto &texture = def->get_loaded_texture();
        if (!def->is_texture_loaded()) continue;

        BeginShaderMode(shader);
        SetShaderValueTexture(shader, GetShaderLocation(shader, "texture0"), texture);
        mr::draw::draw_tile_prev_from_origin(
          def, 
          x*scale, 
          y*scale, 
          scale,
          def->get_color()
        );
        EndShaderMode();
      }

      cell = tilemtx.get_const_ptr(x, y, layer - 2);

      if (
        cell != nullptr && 
        cell->type == TileType::head && 
        cell->tile_def != nullptr && 
        !cell->tile_def->get_specs3().empty()
      ) {
        auto *def = cell->tile_def;
        auto &texture = def->get_loaded_texture();
        if (!def->is_texture_loaded()) continue;

        BeginShaderMode(shader);
        SetShaderValueTexture(shader, GetShaderLocation(shader, "texture0"), texture);
        mr::draw::draw_tile_prev_from_origin(
          def, 
          x*scale, 
          y*scale, 
          scale,
          def->get_color()
        );
        EndShaderMode();
      }
   
    }
  }
}

void mtx_patch_tile_prev_from_origin(
  const TileDef *def,
  const shaders* _shaders,
  int x, int y,
  float scale
) noexcept {
  if (def == nullptr || !def->is_texture_loaded() || _shaders == nullptr) return;

  auto offset = def->get_head_offset();

  const auto &texture = def->get_texture();
  const auto &shader = _shaders->ink();

  BeginShaderMode(shader);
  SetShaderValueTexture(shader, GetShaderLocation(shader, "texture0"), texture);

  DrawTexturePro(
    texture,
    def->get_preview_rectangle(),
    Rectangle{
      (x - offset.x) * scale,
      (y - offset.y) * scale,
      scale, 
      scale
    },
    Vector2{0, 0},
    0,
    def->get_color()
  );

  EndShaderMode();
}

};