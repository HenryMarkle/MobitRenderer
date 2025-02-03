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
#include <MobitRenderer/quad.h>
#include <MobitRenderer/dex.h>

/// @brief This namespace groups draw functions that require special shaders.
/// None of the contained functions should load definition textures; it is 
/// the responsibility of the caller to do that.
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

void draw_tile_as_prop(
    const TileDef *def,
    const shaders *_shaders,
    const Quad &quad
) noexcept;

/// @brief Draws a preview of a prop.
/// @param def The definition of the prop.
/// @param settings The configuration of the prop.
/// @param _shaders The shaders manager class.
/// @param quad The target quad vertices of the prop. 
void draw_prop_preview(
    const PropDef *def,
    const PropSettings *settings,
    const shaders *_shaders,
    const Quad &quad
) noexcept;

/// @brief Draws a preview of a prop.
/// @param prop The prop instance of course. 
/// @param _shaders The shaders manager class.
void draw_prop_preview(
    const Prop *prop,
    const shaders *_shaders
) noexcept;

/// @brief Draws a preview of a standard prop.
/// @param def The definition of the prop.
/// @param settings The configuration of the prop.
/// @param _shaders The shaders manager class.
/// @param quad The target quad vertices of the prop. 
void draw_standard_prop_preview(
    const Standard *def,
    const PropSettings *settings,
    const shaders *_shaders,
    const Quad &quad
) noexcept;

/// @brief Draws a preview of a varied standard prop.
/// @param def The definition of the prop.
/// @param settings The configuration of the prop.
/// @param _shaders The shaders manager class.
/// @param quad The target quad vertices of the prop. 
void draw_varied_standard_prop_preview(
    const VariedStandard *def,
    const PropSettings *settings,
    const shaders *_shaders,
    const Quad &quad
) noexcept;

/// @brief Draws a preview of a soft standard prop.
/// @param def The definition of the prop.
/// @param settings The configuration of the prop.
/// @param _shaders The shaders manager class.
/// @param quad The target quad vertices of the prop. 
void draw_soft_prop_preview(
    const Soft *def,
    const PropSettings *settings,
    const shaders *_shaders,
    const Quad &quad
) noexcept;

/// @brief Draws a preview of a varied soft prop.
/// @param def The definition of the prop.
/// @param settings The configuration of the prop.
/// @param _shaders The shaders manager class.
/// @param quad The target quad vertices of the prop. 
void draw_varied_soft_prop_preview(
    const VariedSoft *def,
    const PropSettings *settings,
    const shaders *_shaders,
    const Quad &quad
) noexcept;

/// @brief Draws a preview of a colored soft prop.
/// @param def The definition of the prop.
/// @param settings The configuration of the prop.
/// @param _shaders The shaders manager class.
/// @param quad The target quad vertices of the prop. 
void draw_colored_soft_prop_preview(
    const ColoredSoft *def,
    const PropSettings *settings,
    const shaders *_shaders,
    const Quad &quad
) noexcept;

/// @brief Draws a preview of a soft effect prop.
/// @param def The definition of the prop.
/// @param settings The configuration of the prop.
/// @param _shaders The shaders manager class.
/// @param quad The target quad vertices of the prop. 
void draw_soft_effect_prop_preview(
    const SoftEffect *def,
    const PropSettings *settings,
    const shaders *_shaders,
    const Quad &quad
) noexcept;

/// @brief Draws a preview of a (simple) decal prop.
/// @param def The definition of the prop.
/// @param settings The configuration of the prop.
/// @param _shaders The shaders manager class.
/// @param quad The target quad vertices of the prop. 
void draw_decal_prop_preview(
    const Decal *def,
    const PropSettings *settings,
    const shaders *_shaders,
    const Quad &quad
) noexcept;

/// @brief Draws a preview of a varied decal prop.
/// @param def The definition of the prop.
/// @param settings The configuration of the prop.
/// @param _shaders The shaders manager class.
/// @param quad The target quad vertices of the prop. 
void draw_varied_decal_prop_preview(
    const VariedDecal *def,
    const PropSettings *settings,
    const shaders *_shaders,
    const Quad &quad
) noexcept;

/// @brief Draws a preview of a long prop.
/// @param def The definition of the prop.
/// @param settings The configuration of the prop.
/// @param _shaders The shaders manager class.
/// @param quad The target quad vertices of the prop. 
void draw_long_prop_preview(
    const Long *def,
    const PropSettings *settings,
    const shaders *_shaders,
    const Quad &quad
) noexcept;

/// @brief Draws a preview of a rope prop.
/// @param def The definition of the prop.
/// @param settings The configuration of the prop.
/// @param _shaders The shaders manager class.
/// @param quad The target quad vertices of the prop. 
void draw_rope_prop_preview(
    const Rope *def,
    const PropSettings *settings,
    const shaders *_shaders,
    const Quad &quad
) noexcept;

/// @brief Draws a preview of a antimatter prop.
/// @param def The definition of the prop.
/// @param settings The configuration of the prop.
/// @param _shaders The shaders manager class.
/// @param quad The target quad vertices of the prop. 
void draw_antimatter_prop_preview(
    const Antimatter *def,
    const PropSettings *settings,
    const shaders *_shaders,
    const Quad &quad
) noexcept;

};