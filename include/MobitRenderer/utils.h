#pragma once

#include <filesystem>
#include <cstdint>

#include <raylib.h>
#include <raymath.h>

#include <MobitRenderer/definitions.h>
#include <MobitRenderer/matrix.h>

namespace mr::utils {

void unload_shader(Shader&);
void unload_texture(Texture2D&);
void unload_rendertexture(RenderTexture2D&);

inline int clamp(int v, int min, int max) { if (v < min) return min; if (v > max) return max; return v; }
inline float clamp(float v, float min, float max) { if (v < min) return min; if (v > max) return max; return v; }

/// @brief Checks if a placement of a material is legal 
/// in a given matrix coordinates.
bool is_material_legal(
    const Matrix<TileCell> &tile_matrix,
    const Matrix<GeoCell> &geo_matrix,
    uint16_t x,
    uint16_t y,
    uint16_t z
);

/// @brief Checks if a placement of a tile is legal 
/// in a given matrix coordinates.
bool is_tile_legal(
    const TileDef *tile, 
    const Matrix<TileCell> &tile_matrix,
    const Matrix<GeoCell> &geo_matrix,
    uint16_t x,
    uint16_t y,
    uint16_t z
) noexcept;

void find_file_case_insensitive(std::filesystem::path &path);

};