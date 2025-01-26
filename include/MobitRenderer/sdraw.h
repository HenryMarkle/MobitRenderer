#pragma once

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

namespace mr::sdraw {

/// @brief Draws an entire layer of a tile matrix (previews)
void draw_tile_prevs_layer(
    const shaders* _shaders,
    Matrix<GeoCell> const &geomtx, 
    Matrix<TileCell> const &tilemtx, 
    const TileDex *tiles, 
    const MaterialDex *materials,
    uint8_t layer,
    float scale
);

};