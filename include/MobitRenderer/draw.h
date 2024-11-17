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

  f3(Font font, float font_size, Color text_color, Color background_color);
  ~f3();
};

}; // namespace debug

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
