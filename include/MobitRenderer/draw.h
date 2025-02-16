#pragma once

#include <any>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <queue>
#include <string>

#include <raylib.h>

#include <MobitRenderer/definitions.h>
#include <MobitRenderer/matrix.h>
#include <MobitRenderer/level.h>
#include <MobitRenderer/atlas.h>
#include <MobitRenderer/quad.h>
#include <MobitRenderer/dex.h>

namespace mr {

namespace debug {

struct f3_queue_data {
  std::any data;
  bool same_line;
};

class f3 {
private:
  Vector2 cursor;
  Color text, background;
  float font_size;
  Font font;
  uint8_t margin_v, margin_h;
  std::queue<f3_queue_data> queue;

public:
  /// @brief Moves the cursor down by one line, by default.
  void newline(uint32_t lines = 1) noexcept;

  /// @brief Resets cursor position.
  void reset() noexcept;

  void set_font(Font) noexcept;

  void enqueue(std::any, bool same_line = false);

  /// @brief Draws the content of the queue by consuming the queue.
  void print_queue() noexcept;

  void print(const char *, bool same_line = false) noexcept;
  void print(const std::string &str, bool same_line = false) noexcept;
  void print(int number, bool same_line = false) noexcept;
  void print(size_t number, bool same_line = false) noexcept;
  void print(float number, bool same_line = false) noexcept;
  void print(double number, bool same_line = false) noexcept;
  void print(bool boolean, bool same_line = false) noexcept;
  /// @brief Prints a memory address (without derefrencing.)
  void print(void *ptr, bool same_line = false) noexcept;
  void print(Vector2, bool same_line = false) noexcept;

  //

  void print(const char *, Color color, bool same_line = false) noexcept;
  void print(const std::string &str, Color color, bool same_line = false) noexcept;
  void print(int number, Color color, bool same_line = false) noexcept;
  void print(size_t number, Color color, bool same_line = false) noexcept;
  void print(float number, Color color, bool same_line = false) noexcept;
  void print(double number, Color color, bool same_line = false) noexcept;
  void print(bool boolean, Color color, bool same_line = false) noexcept;
  /// @brief Prints a memory address (without derefrencing.)
  void print(void *ptr, Color color, bool same_line = false) noexcept;
  void print(Vector2, Color color, bool same_line = false) noexcept;

  f3(Font font, float font_size, Color text_color, Color background_color);
  ~f3();
};

}; // namespace debug

namespace draw {

void draw_texture(const Texture2D &texture, const IQuad &quad);
void draw_texture(const Texture2D *texture, const IQuad *quad);

void draw_texture(const Texture2D &texture, const Quad &quad);
void draw_texture(const Texture2D *texture, const Quad *quad);

void draw_texture(const Texture2D &texture, const Quad &quad, Color color);

void draw_texture(
  const Texture2D &texture, 
  Rectangle src, 
  const Quad &quad
) noexcept;

void draw_texture(
  const Texture2D &texture, 
  Rectangle src, 
  const Quad &quad, 
  Color color
) noexcept;

void draw_grid(levelsize width, levelsize height, Color color = GRAY, int scale = 20, int thickness = 1) noexcept;
void draw_nested_grid(levelsize width, levelsize height, Color color, int scale = 20, int thickness = 1) noexcept;

void draw_frame(levelpixelsize width, levelpixelsize height, Color color = WHITE) noexcept;
void draw_double_frame(levelpixelsize width, levelpixelsize height) noexcept;
void draw_double_frame(int x, int y, levelpixelsize width, levelpixelsize height) noexcept;

void draw_geo_shape(GeoType type, float x, float y, float scale, Color color);

/// @brief Draws a geo cell based on the provided type.
/// @param cell Determines the shape of the cell.
/// @param x The matrix' X coordinate.
/// @param y The matrix' Y coordinate.
/// @param scale The size of the cell.
/// @param color The color of the cell.
/// @note Scale affects the X and Y coordinates by multiplying them by scale (x
/// * scale, y * scale).
/// @attention This function must be called in a drawing context (after
/// BeginDrawing() and before EndDrawing()).
void draw_mtx_geo_type(GeoCell cell, int x, int y, float scale, Color color);

/// @brief Draws a geo cell based on the provided type.
/// @param cell Determines the shape of the cell.
/// @param x The screen's X coordinate.
/// @param y The screen's Y coordinate.
/// @param scale The size of the cell.
/// @param color The color of the cell.
/// @attention This function must be called in a drawing context (after
/// BeginDrawing() and before EndDrawing()).
void draw_geo_type(GeoCell cell, int x, int y, float scale, Color color);

/// @brief Draws a geo cell feature.
/// @param cell The cell's features
/// @param x The matrix' X coordinate.
/// @param y The matrix' Y coordinate.
/// @param scale The size of the cell.
/// @param color The color of the poles.
/// @param atlas Holds all textures for rendering features.
/// @note Scale affects the X and Y coordinates by multiplying them by scale (x
/// * scale, y * scale).
/// @attention This function must be called in a drawing context (after
/// BeginDrawing() and before EndDrawing()).
void draw_mtx_geo_features(GeoCell cell, int x, int y, float scale, Color color, const GE_Textures &atlas);

/// @brief Draws a geo cell feature.
/// @param cell The cell's features
/// @param x The screen's X coordinate.
/// @param y The screen's Y coordinate.
/// @param scale The size of the cell.
/// @param color The color of the poles.
/// @param atlas Holds all textures for rendering features.
/// @attention This function must be called in a drawing context (after
/// BeginDrawing() and before EndDrawing()).
void draw_geo_features(GeoCell cell, int x, int y, float scale,
                      Color color, const GE_Textures &atlas);

/// @brief Draws a geo cell poles.
/// @param cell The cell's features
/// @param x The matrix' X coordinate.
/// @param y The matrix' Y coordinate.
/// @param scale The size of the cell.
/// @param color The color of the poles.
/// @note Scale affects the X and Y coordinates by multiplying them by scale (x
/// * scale, y * scale).
/// @attention This function must be called in a drawing context (after
/// BeginDrawing() and before EndDrawing()).
void draw_mtx_geo_poles(GeoCell cell, int x, int y, float scale, Color color);

/// @brief Draws a geo cell poles.
/// @param cell The cell's features
/// @param x The screen's X coordinate.
/// @param y The screen's Y coordinate.
/// @param scale The size of the cell.
/// @param color The color of the poles.
/// @attention This function must be called in a drawing context (after
/// BeginDrawing() and before EndDrawing()).
void draw_geo_poles(GeoCell cell, int x, int y, float scale,
                      Color color);

/// @brief Draws the shortcuts of the entrances of the geometry matrix. 
/// @param matrix The geonetry matrix.
/// @param x The matrix' X coordinate of the entrance cell.
/// @param y The matrix' Y coordinate of the entrance cell.
/// @param scale The scale of the geo cells.
/// @param color The color.
void draw_mtx_shortcut_entrances(
  Matrix<GeoCell> const &matrix,
  uint16_t x,
  uint16_t y,
  float scale, 
  Color color
);

/// @brief Draws a tile preview from the top left corner.
/// @param x The matrix' X coordinates.
/// @param Y The matrix' Y coordinates.
/// @param scale The scale of the tile.
/// @param color The color of the tile.
/// @note Scake affects the X and Y coordinates by multiplying them by scale.
/// @attention This must be called in a drawing context (after BeginDrawing()
/// and before EndDrawing()).
void draw_mtx_tile_prev(const TileDef *def, int x, int y, float scale,
                  Color color);

/// @brief Draws a tile preview from the origin (x - origin.x, y - origin.y).
/// @param x The matrix' X coordinates.
/// @param Y The matrix' Y coordinates.
/// @param scale The scale of the tile.
/// @param color The color of the tile.
/// @note Scake affects the X and Y coordinates by multiplying them by scale.
/// @attention This must be called in a drawing context (after BeginDrawing()
/// and before EndDrawing()).
void draw_mtx_tile_prev_from_origin(const TileDef *def, int x, int y,
                              float scale, Color color);

/// @brief Draws a tile preview from the top left corner.
/// @param x The matrix' X coordinates.
/// @param Y The matrix' Y coordinates.
/// @param scale The scale of the tile.
/// @param color The color of the tile.
/// @attention This must be called in a drawing context (after BeginDrawing()
/// and before EndDrawing()).
void draw_tile_prev(const TileDef *def, int x, int y, float scale,
              Color color);

/// @brief Draws a tile preview from the origin (x - origin.x, y - origin.y).
/// @param x The matrix' X coordinates.
/// @param Y The matrix' Y coordinates.
/// @param scale The scale of the tile.
/// @param color The color of the tile.
/// @attention This must be called in a drawing context (after BeginDrawing()
/// and before EndDrawing()).
void draw_tile_prev_from_origin(
  const TileDef *def, 
  int x, 
  int y,
  float scale, 
  Color color
);

/// @brief Draws a tile preview from the origin (x - origin.x, y - origin.y).
/// @param x The matrix' X coordinates.
/// @param Y The matrix' Y coordinates.
/// @param scale The scale of the tile.
/// @param color The color of the tile.
/// @param tile_spec_layer The layer of the tile targeted (if the tile is multi-layered).
/// @attention This must be called in a drawing context (after BeginDrawing()
/// and before EndDrawing()).
void draw_tile_prev_from_origin(
  const TileDef *def, 
  int x, 
  int y,
  float scale,
  Color color,
  uint8_t tile_spec_layer
);

/// @brief Draws a tile texture from the top left corner.
/// @param x The matrix' X coordinates.
/// @param Y The matrix' Y coordinates.
/// @param scale The scale of the tile.
/// @note Scake affects the X and Y coordinates by multiplying them by scale.
/// @attention This must be called in a drawing context (after BeginDrawing()
/// and before EndDrawing()).
void draw_mtx_tile(const TileDef *def, int x, int y, float scale);

/// @brief Draws a tile texture from the origin (x - origin.x, y - origin.y).
/// @param x The matrix' X coordinates.
/// @param Y The matrix' Y coordinates.
/// @param scale The scale of the tile.
/// @note Scake affects the X and Y coordinates by multiplying them by scale.
/// @attention This must be called in a drawing context (after BeginDrawing()
/// and before EndDrawing()).
void draw_mtx_tile_from_origin(const TileDef *def, int x, int y, float scale);

/// @brief Draws a tile texture from the top left corner.
/// @param x The screen's X coordinates.
/// @param Y The screen's Y coordinates.
/// @param scale The scale of the tile.
/// @attention This must be called in a drawing context (after BeginDrawing()
/// and before EndDrawing()).
void draw_tile(const TileDef *def, int x, int y, float scale);

/// @brief Draws a tile texture from the origin (x - origin.x, y - origin.y).
/// @param x The screen's X coordinates.
/// @param Y The screen's Y coordinates.
/// @param scale The scale of the tile.
/// @attention This must be called in a drawing context (after BeginDrawing()
/// and before EndDrawing()).
void draw_tile_from_origin(const TileDef *def, int x, int y, float scale);

/// @brief Draws a tile tinted texture from the top left corner.
/// @param x The matrix' X coordinates.
/// @param Y The matrix' Y coordinates.
/// @param scale The scale of the tile.
/// @param tint The color of the tile.
/// @note Scake affects the X and Y coordinates by multiplying them by scale.
/// @attention This must be called in a drawing context (after BeginDrawing()
/// and before EndDrawing()).
void draw_mtx_tile_tinted(const TileDef *def, int x, int y, float scale, Color tint);

/// @brief Draws a tile tinted texture from the origin (x - origin.x, y - origin.y).
/// @param x The matrix' X coordinates.
/// @param Y The matrix' Y coordinates.
/// @param scale The scale of the tile.
/// @param tint The color of the tile.
/// @note Scake affects the X and Y coordinates by multiplying them by scale.
/// @attention This must be called in a drawing context (after BeginDrawing()
/// and before EndDrawing()).
void draw_mtx_tile_from_origin_tinted(const TileDef *def, int x, int y, float scale, Color tint);

/// @brief Draws a tile tinted texture from the top left corner.
/// @param x The screen's X coordinates.
/// @param Y The screen's Y coordinates.
/// @param scale The scale of the tile.
/// @param tint The color of the tile.
/// @attention This must be called in a drawing context (after BeginDrawing()
/// and before EndDrawing()).
void draw_tile_tinted(const TileDef *def, int x, int y, float scale, Color tint);

/// @brief Draws a tile tinted texture from the origin (x - origin.x, y - origin.y).
/// @param x The screen's X coordinates.
/// @param Y The screen's Y coordinates.
/// @param scale The scale of the tile.
/// @param tint The color of the tile.
/// @attention This must be called in a drawing context (after BeginDrawing()
/// and before EndDrawing()).
void draw_tile_from_origin_tinted(const TileDef *def, int x, int y, float scale, Color tint);

/// @brief Erases a tile from origin.
/// @param x Matrix' X coordinates. 
/// @param y Matrix' Y coordinates. 
void mtx_erase_tile_prev_from_origin(
  const TileDef *def,
  int x, int y,
  float scale = 20.0f
) noexcept;

/// @brief Draws an entire layer of a geometry matrix. 
/// @param matrix A constant reference to the matrix.
/// @param layer The layer (0 - 2).
/// @param color The color of the layer to be drawn.
/// @attention This must be called in a drawing context (after BeginDrawing()
/// and before EndDrawing()).
void draw_geo_layer(Matrix<GeoCell> const& matrix, uint8_t layer, Color color, float scale = 20.0f);

/// @brief Draws an entire layer of a geometry matrix. 
/// @param matrix A constant reference to the matrix.
/// @param layer The layer (0 - 2).
/// @param color The color of the layer to be drawn.
/// @attention This must be called in a drawing context (after BeginDrawing()
/// and before EndDrawing()).
void draw_geo_layer(Matrix<GeoCell> const* matrix, uint8_t layer, Color color, float scale = 20.0f);

/// @brief Draws an entire layer of a geometry matrix with poles. 
/// @param matrix A constant reference to the matrix.
/// @param layer The layer (0 - 2).
/// @param scale The size of each cell in pixels.
/// @param color The color of the layer to be drawn.
/// @attention This must be called in a drawing context (after BeginDrawing()
/// and before EndDrawing()).
void draw_geo_and_poles_layer(Matrix<GeoCell> const& matrix, uint8_t layer, Color color, float scale = 20.0f);

/// @brief Draws an entire layer of a geometry matrix features.
/// @param matrix A constant reference to the matrix.
/// @param atlas A constant reference to the textures atlas.
/// @param color The brush color.
/// @param scale The size of each cell in pixels.
void draw_geo_features_layer(Matrix<GeoCell> const &matrix, const GE_Textures &atlas, uint8_t layer, Color color, float scale = 20.0f);

/// @brief Draws cracked terrain
void draw_geo_cracked(
  Matrix<GeoCell> const &matrix, 
  GE_Textures &atlas, 
  uint8_t layer,
  Color color, 
  float scale = 20.0f
);

void draw_geo_entrances(Matrix<GeoCell> const &matrix, GE_Textures &atlas, Color color, float scale = 20.0f);

}; // namespace draw

}; // namespace mr
