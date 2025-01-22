#include <memory>

#include <raylib.h>
#include <rlgl.h>

#include <MobitRenderer/draw.h>
#include <MobitRenderer/matrix.h>
#include <MobitRenderer/definitions.h>

namespace mr {

namespace draw {

void draw_texture(const Texture2D *texture, const iquad *quad) {
  rlSetTexture(texture->id);

  rlBegin(RL_QUADS);

  rlColor4ub(255, 255, 255, 255);

  bool flipx = quad->topleft.x > quad->topright.x && quad->bottomleft.x > quad->bottomright.x;
  bool flipy = quad->topleft.y > quad->bottomleft.y && quad->topright.y > quad->bottomright.y;
  

  int vtrx = flipx ? quad->topleft.x : quad->topright.x;
  int vtry = flipy ? quad->bottomright.y : quad->topright.y;

  int vtlx = flipx ? quad->topright.x : quad->topleft.x;
  int vtly = flipy ? quad->bottomleft.y : quad->topleft.y;

  int vblx = flipx ? quad->bottomright.x : quad->bottomleft.x;
  int vbly = flipy ? quad->topleft.y : quad->bottomleft.y;

  int vbrx = flipx ? quad->bottomleft.x : quad->bottomright.x;
  int vbry = flipy ? quad->topright.y : quad->bottomright.y;


  int ttrx = flipx ? 0 : 1.0f;
  int ttry = flipy ? 1.0f : 0;

  int ttlx = flipx ? 1.0f : 0;
  int ttly = flipy ? 1.0f : 0;

  int tblx = flipx ? 1.0f : 0;
  int tbly = flipy ? 0 : 1.0f;

  int tbrx = flipx ? 0 : 1.0f;
  int tbry = flipy ? 0 : 1.0f;


  // top right
  rlTexCoord2f(ttrx, ttry);
  rlVertex2i(vtrx, vtry);

  // top left
  rlTexCoord2f(ttlx, ttly);
  rlVertex2i(vtlx, vtly);
  
  // bottom left
  rlTexCoord2f(tblx, tbly);
  rlVertex2i(vblx, vbly);

  // bottom right
  rlTexCoord2f(tbrx, tbry);
  rlVertex2i(vbrx, vbry);


  // top right
  rlTexCoord2f(ttrx, ttry);
  rlVertex2i(vtrx, vtry);


  rlSetTexture(0);
}

void draw_texture(const Texture2D &texture, const iquad &quad) {
  draw_texture(&texture, &quad);
}

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
) noexcept {
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

  std::vector<int8_t> specs;
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

void draw_tile_prevs_layer(
  Matrix<GeoCell> const &geomtx,
  Matrix<TileCell> const &tilemtx, 
  const TileDex *tiles, 
  const MaterialDex *materials,
  uint8_t layer,
  float scale
) {
  if (layer > 2 || tiles == nullptr || materials == nullptr) 
    return;

  // terrible names. I know.

  const float sxy = scale * 0.1f;
  const float ss = scale * 0.1f;

  for (uint16_t x = 0; x < tilemtx.get_width(); x++) {
    for (uint16_t y = 0; y < tilemtx.get_height(); y++) {
      const auto *cell = tilemtx.get_const_ptr(x, y, layer);
      if (cell == nullptr) continue;

      switch (cell->type) {
        case TileType::head:
        {
          auto *def = cell->tile_def;
          if (def == nullptr) break;

          if (def->is_multilayer()) {
            draw_tile_prev_from_origin(
              def, 
              x*scale, 
              y*scale, 
              scale,
              def->get_color(),
              layer
            );
          }
          else {
            draw_tile_prev_from_origin(
              def, 
              x*scale, 
              y*scale, 
              scale,
              def->get_color()
            );
          }

        }
        break;

        case TileType::material:
        {
          auto *def = cell->material_def;
          if (def == nullptr) break;

          auto &geocell = geomtx.get_const(x, y, layer);

          draw_geo_shape(
            geocell.type, 
            x + sxy,
            y + sxy,
            ss,
            def->get_color()
          );
        }
        break;
      }
    }
  }
}

};

};
