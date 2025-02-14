#include <memory>

#include <raylib.h>
#include <rlgl.h>

#include <MobitRenderer/draw.h>
#include <MobitRenderer/matrix.h>
#include <MobitRenderer/definitions.h>

namespace mr {

namespace draw {

void draw_mtx_tile_prev(const TileDef *def, int x, int y,
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
      def->get_preview_rectangle(),
      Rectangle{
          x * scale, 
          y * scale, 
          width, 
          height
        },
      Vector2{ 0, 0 }, 0, 
      color
  );
}

void draw_mtx_tile_prev_from_origin(const TileDef *def, int x,
                                   int y, float scale, Color color) {
  if (def == nullptr || color.a == 0)
    return;

  auto &texture = def->get_texture();
  if (texture.id == 0)
    return;

  float width = def->calculate_width(scale);
  float height = def->calculate_height(scale);
  ivec2 offset = def->get_head_offset();

  DrawTexturePro(
    def->get_texture(),
    def->get_preview_rectangle(),
    Rectangle{
      (x - offset.x) * scale,
      (y - offset.y) * scale,
      width,
      height
    },
    Vector2{0, 0}, 0, color
  );
}

void draw_tile_prev(const TileDef *def, int x, int y,
                       float scale, Color color) {
  if (def == nullptr || color.a == 0)
    return;

  auto &texture = def->get_texture();
  if (texture.id == 0)
    return;

  float width = def->get_width() * scale;
  float height = def->get_height() * scale;
  auto src_rect = def->get_preview_rectangle();

  DrawTexturePro(
      def->get_texture(),
      src_rect,
      Rectangle{
          (float)x, 
          (float)y, 
          width, 
          height
        },
      Vector2{ 0, 0 }, 0, 
      color
  );
}

void draw_tile_prev_from_origin(
  const TileDef *def, 
  int x, 
  int y,
  float scale,
  Color color
) {
  if (def == nullptr || color.a == 0)
    return;

  if (!def->is_texture_loaded()) return;
  
  auto &texture = def->get_texture();

  float width = def->get_width() * scale;
  float height = def->get_height() * scale;
  ivec2 offset = def->get_head_offset();

  DrawTexturePro(
    def->get_texture(),
    def->get_preview_rectangle(),
    Rectangle{
      (x - offset.x * scale),
      (y - offset.y * scale),
      width,
      height
    },
    Vector2{0, 0}, 0, color
  );
}

void draw_tile_prev_from_origin(
  const TileDef *def, 
  int x, 
  int y,
  float scale,
  Color color,
  uint8_t tile_spec_layer
) {
  if (def == nullptr || color.a == 0 || tile_spec_layer > 2)
    return;

  auto &texture = def->get_texture();
  if (texture.id == 0)
    return;

  std::vector<int> specs;
  switch (tile_spec_layer) {
    case 0: specs = def->get_specs(); break;
    case 1: specs = def->get_specs2(); break;
    case 2: specs = def->get_specs3(); break;
  }
  if (specs.empty()) return;

  float width = def->calculate_width(scale);
  float height = def->calculate_height(scale);
  ivec2 offset = def->get_head_offset();
  Rectangle src_rec = def->get_preview_rectangle();

  for (size_t sx = 0; sx < def->get_width(); sx++) {
    for (size_t sy = 0; sy < def->get_height(); sy++) {
      auto &spec = specs.at(sy + (sx * def->get_width()));

      if (spec == -1) continue;

      DrawTexturePro(
        def->get_texture(),
        Rectangle{
          src_rec.x + sx*16.0f,
          src_rec.y + sy*16.0f,
          16.0f,
          16.0f
        },
        Rectangle{
          (x - offset.x * scale + sx * scale),
          (y - offset.y * scale + sy * scale),
          scale,
          scale
        },
        Vector2{0, 0}, 0, color
      );

    }
  }

}

void draw_tile(const TileDef* def, int x, int y, float scale) {
  if (def == nullptr)
    return;

  auto &texture = def->get_texture();
  if (texture.id == 0) return;

  float width = def->calculate_width(scale);
  float height = def->calculate_height(scale);

  float src_width = def->calculate_width(20);
  float src_height = def->calculate_height(20);

  for (int h = def->get_repeat().size(); h >= 0; h--) {
    auto offset = 1 + 20 * src_height * h;

    DrawTexturePro(
      def->get_texture(),
      Rectangle{
        0,
        offset,
        src_width,
        src_height
      },
      Rectangle{(float)x, (float)y, width, height},
      Vector2{0, 0}, 0, WHITE
    );
  }
}

void draw_tile_from_origin(std::shared_ptr<TileDef> def, int x, int y,
                           float scale, Color color) {
  if (def == nullptr || color.a == 0)
    return;

  auto &texture = def->get_texture();
  if (texture.id == 0)
    return;

  float width = def->calculate_width(scale);
  float height = def->calculate_height(scale);
  ivec2 offset = def->get_head_offset();

  DrawTexturePro(def->get_texture(),
                 Rectangle{0,
                          0,
                          (float)texture.width,
                          (float)texture.height},
                 Rectangle{(float)(x - offset.x),
                           (float)(y - offset.y),
                           width,
                           height},
                 Vector2{0, 0}, 0, color);
}

void draw_tile_tinted(
  const TileDef *def, 
  int x, int y, float scale,
  Color color) {
  if (def == nullptr || color.a == 0)
    return;

  auto &texture = def->get_texture();
  if (texture.id == 0) return;

  float width = def->calculate_width(scale);
  float height = def->calculate_height(scale);

  float src_width = def->calculate_width(20);
  float src_height = def->calculate_height(20);

  for (int h = def->get_repeat().size(); h >= 0; h--) {
    auto offset = 1 + 20 * src_height * h;

    DrawTexturePro(
      def->get_texture(),
      Rectangle{
        0,
        offset,
        src_width,
        src_height
      },
      Rectangle{(float)x, (float)y, width, height},
      Vector2{0, 0}, 0, 
      color
    );
  }
}

void mtx_erase_tile_prev_from_origin(
  const TileDef *def,
  int x, int y,
  float scale = 20.0f
) noexcept {
  if (def == nullptr) return;

  auto offset = def->get_head_offset();

  DrawRectangleRec(
    Rectangle {
      (x - offset.x) * scale,
      (y - offset.y) * scale,
      scale,
      scale
    },
    WHITE
  );
}

};

};
