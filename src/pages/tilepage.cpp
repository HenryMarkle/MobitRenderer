#include <cstdint>
#include <memory>

#include <raylib.h>
#include <raymath.h>
#include <imgui.h>

#include <MobitRenderer/draw.h>
#include <MobitRenderer/matrix.h>
#include <MobitRenderer/pages.h>

namespace mr::pages {

void Tile_Page::process() {
  auto wheel = GetMouseWheelMove();

  if (wheel != 0) {

    auto &camera = ctx->get_camera();
    auto mouseWorldPosition = GetScreenToWorld2D(GetMousePosition(), camera);
    
    camera.offset = GetMousePosition();
    camera.target = mouseWorldPosition;
    camera.zoom += wheel * 0.125f;
    if (camera.zoom < 0.125f) camera.zoom = 0.125f;
  }

  if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)) {
    auto delta = GetMouseDelta();
    auto &camera = ctx->get_camera();

    delta = Vector2Scale(delta, -1.0f / camera.zoom);
    
    camera.target.x += delta.x;
    camera.target.y += delta.y;
  }

  if (IsKeyPressed(KEY_L)) {
    ctx->level_layer_ = (ctx->level_layer_ + 1) % 3;
    should_redraw = true;
  }
}

void Tile_Page::draw() noexcept {
  ClearBackground(DARKGRAY);

  auto &camera = ctx->get_camera();
  const auto *level = ctx->get_selected_level();
  const auto &viewport = ctx->textures_->get_main_level_viewport();

  if (should_redraw1) {
    BeginTextureMode(ctx->textures_->geo_layer1.get());

    ClearBackground(WHITE);

    draw_geo_and_poles_layer(
      ctx->get_selected_level()->get_const_geo_matrix(),
      0,
      BLACK
    );

    EndTextureMode();

    should_redraw1 = false;
    should_redraw = true;
  }
  
  if (should_redraw2) {
    BeginTextureMode(ctx->textures_->geo_layer2.get());

    ClearBackground(WHITE);

    draw_geo_and_poles_layer(
      ctx->get_selected_level()->get_const_geo_matrix(),
      1,
      BLACK
    );

    EndTextureMode();

    should_redraw2 = false;
    should_redraw = true;
  }

  if (should_redraw3) {
    BeginTextureMode(ctx->textures_->geo_layer3.get());

    ClearBackground(WHITE);

    draw_geo_and_poles_layer(
      ctx->get_selected_level()->get_const_geo_matrix(),
      2,
      BLACK
    );

    EndTextureMode();

    should_redraw3 = false;
    should_redraw = true;
  }

  if (should_redraw) {
    BeginTextureMode(ctx->textures_->get_main_level_viewport());
    ClearBackground(Color{200, 200, 200, 255});

    const auto &shader = ctx->get_shaders_const().white_remover_apply_color();
    BeginShaderMode(shader);
    {
      const auto layer = ctx->level_layer_;

      if (layer != 2) {
        SetShaderValueTexture(
          shader, 
          GetShaderLocation(shader, "texture0"), 
          ctx->textures_->geo_layer3.get().texture
        );
        DrawTexture(
          ctx->textures_->geo_layer3.get().texture, 
          0, 
          0, 
          Color{40, 40, 40, 200}
        );
      }

      if (layer != 1) {
        SetShaderValueTexture(
          shader, 
          GetShaderLocation(shader, "texture0"), 
          ctx->textures_->geo_layer2.get().texture
        );
        DrawTexture(
          ctx->textures_->geo_layer2.get().texture, 
          0, 
          0, 
          Color{20, 20, 20, 200}
        );
      }

      if (layer != 0) {
        SetShaderValueTexture(
          shader, 
          GetShaderLocation(shader, "texture0"), 
          ctx->textures_->geo_layer1.get().texture
        );
        DrawTexture(
          ctx->textures_->geo_layer1.get().texture, 
          0, 
          0, 
          Color{0, 0, 0, 120}
        );
      }

      // Foreground

      if (layer == 2) {
        SetShaderValueTexture(
          shader, 
          GetShaderLocation(shader, "texture0"), 
          ctx->textures_->geo_layer3.get().texture
        );
        DrawTexture(
          ctx->textures_->geo_layer3.get().texture, 
          0, 
          0, 
          Color{0, 0, 0, 220}
        );
      }

      if (layer == 1) {
        SetShaderValueTexture(
          shader, 
          GetShaderLocation(shader, "texture0"), 
          ctx->textures_->geo_layer2.get().texture
        );
        DrawTexture(
          ctx->textures_->geo_layer2.get().texture, 
          0, 
          0, 
          Color{0, 0, 0, 220}
        );
      }

      if (layer == 0) {
        SetShaderValueTexture(
          shader, 
          GetShaderLocation(shader, "texture0"), 
          ctx->textures_->geo_layer1.get().texture
        );
        DrawTexture(
          ctx->textures_->geo_layer1.get().texture, 
          0, 
          0, 
          Color{0, 0, 0, 220}
        );
      }
    }
    EndShaderMode();
    
    EndTextureMode();

    should_redraw = false;
  }

  BeginMode2D(camera);
  {
    DrawTexture(viewport.texture, 0, 0, WHITE);
    
    // if (ctx->get_config_const().grid.visible) {
    //   mr::draw_nested_grid(72, 43, Color{130, 130, 130, 200});
    // }

    mr::draw_double_frame(
      level->get_pixel_width(), 
      level->get_pixel_height()
    );
  }
  EndMode2D();
}
void Tile_Page::windows() noexcept {}
void Tile_Page::order_level_redraw() noexcept { should_redraw = true; }
void Tile_Page::f3() const noexcept {
    auto f3 = ctx->f3_;
  auto camera = ctx->get_camera();

  f3->print("Zoom ");
  f3->print(ctx->get_camera().zoom, true);

  f3->print("Target ");
  f3->print(ctx->get_camera().target, true);

  f3->print("Offset ");
  f3->print(ctx->get_camera().offset, true);

  {
    auto mouse = GetScreenToWorld2D(GetMousePosition(), ctx->get_camera());
    f3->print("Position ");
    f3->print(mouse, true);

    auto mtxMouse = Vector2Divide(mouse, {20, 20});
    f3->print("MTX ");
    f3->print((int)mtxMouse.x, true);
    f3->print(" / ", true);
    f3->print((int)mtxMouse.y, true);
  }

  f3->print("Grid ");
  f3->print(ctx->get_config_const().grid.visible, true);

  f3->print("Layer Pointer: Global");
  f3->print("L ");
  f3->print((int)ctx->level_layer_, true);
}

Tile_Page::Tile_Page(context *ctx) : 
    Page(ctx),
    should_redraw(true), 
    should_redraw1(true),
    should_redraw2(true),
    should_redraw3(true),
    should_redraw_tile1(true),
    should_redraw_tile2(true),
    should_redraw_tile3(true) {}

Tile_Page::~Tile_Page() {}

};