#include <memory>

#include "MobitRenderer/definitions.h"
#include <raylib.h>

#include <MobitRenderer/draw.h>
#include <MobitRenderer/matrix.h>

void mr::draw_mtx_tile(std::shared_ptr<mr::TileDef> def, int x, int y,
                       float scale, Color color) {
  if (def == nullptr || color.a == 0)
    return;

  auto &texture = def->get_texture();
  if (texture.id == 0)
    return;

  float width = def->calculate_width(scale);
  float height = def->calculate_height(scale);

  DrawTexturePro(
      def->get_texture(),
      Rectangle{.x = 0,
                .y = 0,
                .width = (float)texture.width,
                .height = (float)texture.height},
      Rectangle{
          .x = x * scale, .y = y * scale, .width = width, .height = height},
      Vector2{.x = 0, .y = 0}, 0, color);
}

void mr::draw_mtx_tile_from_origin(std::shared_ptr<mr::TileDef> def, int x,
                                   int y, float scale, Color color) {
  if (def == nullptr || color.a == 0)
    return;

  auto &texture = def->get_texture();
  if (texture.id == 0)
    return;

  float width = def->calculate_width(scale);
  float height = def->calculate_height(scale);
  ivec2 offset = def->get_head_offset();

  DrawTexturePro(def->get_texture(),
                 Rectangle{.x = 0,
                           .y = 0,
                           .width = (float)texture.width,
                           .height = (float)texture.height},
                 Rectangle{.x = (x - offset.x) * scale,
                           .y = (y - offset.y) * scale,
                           .width = width,
                           .height = height},
                 Vector2{.x = 0, .y = 0}, 0, color);
}

void draw_tile(std::shared_ptr<mr::TileDef> def, int x, int y, float scale,
               Color color) {
  if (def == nullptr || color.a == 0)
    return;

  auto &texture = def->get_texture();
  if (texture.id == 0)
    return;

  float width = def->calculate_width(scale);
  float height = def->calculate_height(scale);

  DrawTexturePro(
      def->get_texture(),
      Rectangle{.x = 0,
                .y = 0,
                .width = (float)texture.width,
                .height = (float)texture.height},
      Rectangle{.x = (float)x, .y = (float)y, .width = width, .height = height},
      Vector2{.x = 0, .y = 0}, 0, color);
}

void draw_tile_from_origin(std::shared_ptr<mr::TileDef> def, int x, int y,
                           float scale, Color color) {
  if (def == nullptr || color.a == 0)
    return;

  auto &texture = def->get_texture();
  if (texture.id == 0)
    return;

  float width = def->calculate_width(scale);
  float height = def->calculate_height(scale);
  mr::ivec2 offset = def->get_head_offset();

  DrawTexturePro(def->get_texture(),
                 Rectangle{.x = 0,
                           .y = 0,
                           .width = (float)texture.width,
                           .height = (float)texture.height},
                 Rectangle{.x = (float)(x - offset.x),
                           .y = (float)(y - offset.y),
                           .width = width,
                           .height = height},
                 Vector2{.x = 0, .y = 0}, 0, color);
}
