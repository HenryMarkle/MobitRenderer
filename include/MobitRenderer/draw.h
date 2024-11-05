#pragma once

#include <raylib.h>
#include <MobitRenderer/matrix.h>

namespace mr {

    /// @brief Draws a geo cell based on the provided type.
    /// @param type Determines the shape of the cell.
    /// @param x The matrix' X coordinate.
    /// @param y The matrix' Y coordinate.
    /// @param scale The size of the cell.
    /// @param color The color of the cell.
    /// @note Scale affects the X and Y coordinates by multiplying them by scale (x * scale, y * scale).
    /// @attention This function must be called in a drawing context (after BeginDrawing() and before EndDrawing()).
    void draw_mtx_geo_type(GeoType type, int x, int y, float scale, Color color);

    /// @brief Draws a geo cell based on the provided type.
    /// @param type Determines the shape of the cell.
    /// @param x The screen's X coordinate.
    /// @param y The screen's Y coordinate.
    /// @param scale The size of the cell.
    /// @param color The color of the cell.
    /// @attention This function must be called in a drawing context (after BeginDrawing() and before EndDrawing()).
    void draw_geo_type(GeoType type, int x, int y, float scale, Color color);

    /// @brief Draws a geo cell feature.
    /// @param features The cell's features
    /// @param x The matrix' X coordinate.
    /// @param y The matrix' Y coordinate.
    /// @param scale The size of the cell.
    /// @param color The color of the cell.
    /// @note Scale affects the X and Y coordinates by multiplying them by scale (x * scale, y * scale).
    /// @attention This function must be called in a drawing context (after BeginDrawing() and before EndDrawing()).
    void draw_mtx_geo_feature(GeoFeature features, int x, int y, float scale, Color color);

    /// @brief Draws a geo cell feature.
    /// @param features The cell's features
    /// @param x The screen's X coordinate.
    /// @param y The screen's Y coordinate.
    /// @param scale The size of the cell.
    /// @param color The color of the cell.
    /// @attention This function must be called in a drawing context (after BeginDrawing() and before EndDrawing()).
    void draw_geo_feature(GeoFeature features, int x, int y, float scale, Color color);
};