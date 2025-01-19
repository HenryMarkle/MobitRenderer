#if defined(_WIN32) || defined(_WIN64)
  #define WIN32_LEAN_AND_MEAN
#endif

#include <memory>

#include <raylib.h>
#include <rlgl.h>

#include <MobitRenderer/draw.h>
#include <MobitRenderer/matrix.h>
#include <MobitRenderer/definitions.h>

namespace mr {

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

  DrawTexturePro(def->get_texture(),
                 def->get_preview_rectangle(),
                 Rectangle{(x - offset.x) * scale,
                          (y - offset.y) * scale,
                          width,
                          height},
                 Vector2{0, 0}, 0, color);
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

void draw_tile_prev_from_origin(const TileDef *def, int x, int y,
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
                 def->get_preview_rectangle(),
                 Rectangle{(x - offset.x * scale),
                          (y - offset.y * scale),
                          width,
                          height},
                 Vector2{0, 0}, 0, color);
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
};
