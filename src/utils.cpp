#include <cstdint>

#include <raylib.h>

#include <MobitRenderer/utils.h>
#include <MobitRenderer/definitions.h>
#include <MobitRenderer/matrix.h>

namespace mr::utils {

void unload_shader(Shader &shader) {
  if (shader.id == 0) return;
  UnloadShader(shader);
  shader = Shader{0};
}

void unload_texture(Texture2D &texture) {
  if (texture.id == 0) return;
  UnloadTexture(texture);
  texture = Texture2D{0};
}

void unload_rendertexture(RenderTexture2D &rt) {
  if (rt.id == 0) return;
  UnloadRenderTexture(rt);
  rt = {0};
}

bool is_tile_legal(
  const TileDef *tile, 
  const Matrix<TileCell> &tile_matrix,
  const Matrix<GeoCell> &geo_matrix,
  uint16_t x,
  uint16_t y,
  uint16_t z
) noexcept {
  if (tile == nullptr) return false;
  if (!tile_matrix.is_in_bounds(x, y, z)) return false;

  const auto &specs1 = tile->get_specs();
  const auto &specs2 = tile->get_specs2();
  const auto &specs3 = tile->get_specs3();

  auto specs_size = tile->get_width() * tile->get_height();
  auto origin = tile->get_head_offset();

  if (!specs1.empty() && specs1.size() == specs_size) {
    for (size_t sx = 0; sx < tile->get_width(); sx++) {
      for (size_t sy = 0; sy < tile->get_height(); sy++) {
        int8_t spec = specs1[sy + sx*tile->get_height()];
        if (spec <= -1) continue;
        
        const auto *tcell = tile_matrix.get_const_ptr(x - origin.x + sx, y - origin.y + sy, z);
        const auto *gcell = geo_matrix.get_const_ptr(x - origin.x + sx, y - origin.y + sy, z);
      
        if (gcell == nullptr) continue;
        if (static_cast<int8_t>(gcell->type) != static_cast<uint8_t>(spec)) return false;
        if (tcell == nullptr) continue;
        if (tcell->type != TileType::material && tcell->type != TileType::_default) return false;
      }
    }
  }

  if (z < 2 && !specs2.empty() && specs2.size() == specs_size) {
    for (size_t sx = 0; sx < tile->get_width(); sx++) {
      for (size_t sy = 0; sy < tile->get_height(); sy++) {
        int8_t spec2 = specs2[sy + sx*tile->get_height()];
        if (spec2 <= -1) continue;
        
        const auto *tcell = tile_matrix.get_const_ptr(x - origin.x + sx, y - origin.y + sy, z + 1);
        const auto *gcell = geo_matrix.get_const_ptr(x - origin.x + sx, y - origin.y + sy, z + 1);
      
        if (gcell == nullptr) continue;
        if (static_cast<int8_t>(gcell->type) != static_cast<uint8_t>(spec2)) return false;
        if (tcell == nullptr) continue;
        if (tcell->type != TileType::material && tcell->type != TileType::_default) return false;
      }
    }
  }

  if (z < 1 && !specs3.empty() && specs3.size() == specs_size) {
    for (size_t sx = 0; sx < tile->get_width(); sx++) {
      for (size_t sy = 0; sy < tile->get_height(); sy++) {
        int8_t spec2 = specs3[sy + sx*tile->get_height()];
        if (spec2 <= -1) continue;
        
        const auto *tcell = tile_matrix.get_const_ptr(x - origin.x + sx, y - origin.y + sy, z + 2);
        const auto *gcell = geo_matrix.get_const_ptr(x - origin.x + sx, y - origin.y + sy, z + 2);
      
        if (gcell == nullptr) continue;
        if (static_cast<int8_t>(gcell->type) != static_cast<uint8_t>(spec2)) return false;
        if (tcell == nullptr) continue;
        if (tcell->type != TileType::material && tcell->type != TileType::_default) return false;
      }
    }
  }

  return true;
}

};