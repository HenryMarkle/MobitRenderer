#pragma once

#include "MobitRenderer/definitions.h"
#include <MobitRenderer/matrix.h>
#include <memory>
#include <raylib.h>

namespace mr {

/// @brief Draws a geo cell based on the provided type.
/// @param type Determines the shape of the cell.
/// @param x The matrix' X coordinate.
/// @param y The matrix' Y coordinate.
/// @param scale The size of the cell.
/// @param color The color of the cell.
/// @note Scale affects the X and Y coordinates by multiplying them by scale (x
/// * scale, y * scale).
/// @attention This function must be called in a drawing context (after
/// BeginDrawing() and before EndDrawing()).
void draw_mtx_geo_type(GeoType type, int x, int y, float scale, Color color);

/// @brief Draws a geo cell based on the provided type.
/// @param type Determines the shape of the cell.
/// @param x The screen's X coordinate.
/// @param y The screen's Y coordinate.
/// @param scale The size of the cell.
/// @param color The color of the cell.
/// @attention This function must be called in a drawing context (after
/// BeginDrawing() and before EndDrawing()).
void draw_geo_type(GeoType type, int x, int y, float scale, Color color);

/// @brief Draws a geo cell feature.
/// @param features The cell's features
/// @param x The matrix' X coordinate.
/// @param y The matrix' Y coordinate.
/// @param scale The size of the cell.
/// @param color The color of the cell.
/// @note Scale affects the X and Y coordinates by multiplying them by scale (x
/// * scale, y * scale).
/// @attention This function must be called in a drawing context (after
/// BeginDrawing() and before EndDrawing()).
void draw_mtx_geo_feature(GeoFeature features, int x, int y, float scale,
                          Color color);

/// @brief Draws a geo cell feature.
/// @param features The cell's features
/// @param x The screen's X coordinate.
/// @param y The screen's Y coordinate.
/// @param scale The size of the cell.
/// @param color The color of the cell.
/// @attention This function must be called in a drawing context (after
/// BeginDrawing() and before EndDrawing()).
void draw_geo_feature(GeoFeature features, int x, int y, float scale,
                      Color color);

/// @brief Draws a tile cell from the top left corner.
/// @param cell The tile cell.
/// @param x The matrix' X coordinates.
/// @param Y The matrix' Y coordinates.
/// @param scale The scale of the tile.
/// @param color The color of the tile.
/// @note Scake affects the X and Y coordinates by multiplying them by scale.
/// @attention This must be called in a drawing context (after BeginDrawing()
/// and before EndDrawing()).
void draw_mtx_tile(std::shared_ptr<TileDef> def, int x, int y, float scale,
                   Color color);

/// @brief Draws a tile cell from the origin (x - origin.x, y - origin.y).
/// @param cell The tile cell.
/// @param x The matrix' X coordinates.
/// @param Y The matrix' Y coordinates.
/// @param scale The scale of the tile.
/// @param color The color of the tile.
/// @note Scake affects the X and Y coordinates by multiplying them by scale.
/// @attention This must be called in a drawing context (after BeginDrawing()
/// and before EndDrawing()).
void draw_mtx_tile_from_origin(std::shared_ptr<TileDef> def, int x, int y,
                               float scale, Color color);

/// @brief Draws a tile cell from the top left corner.
/// @param cell The tile cell.
/// @param x The matrix' X coordinates.
/// @param Y The matrix' Y coordinates.
/// @param scale The scale of the tile.
/// @param color The color of the tile.
/// @attention This must be called in a drawing context (after BeginDrawing()
/// and before EndDrawing()).
void draw_tile(std::shared_ptr<TileDef> def, int x, int y, float scale,
               Color color);

/// @brief Draws a tile cell from the origin (x - origin.x, y - origin.y).
/// @param cell The tile cell.
/// @param x The matrix' X coordinates.
/// @param Y The matrix' Y coordinates.
/// @param scale The scale of the tile.
/// @param color The color of the tile.
/// @attention This must be called in a drawing context (after BeginDrawing()
/// and before EndDrawing()).
void draw_tile_from_origin(std::shared_ptr<TileDef> def, int x, int y,
                           float scale, Color color);
}; // namespace mr
