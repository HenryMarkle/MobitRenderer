#if defined(_WIN32) || defined(_WIN64)
  #define WIN32_LEAN_AND_MEAN
#endif

#include <memory>
#include <cstdint>

#include <raylib.h>
#include <raymath.h>
#include <imgui.h>

#include <MobitRenderer/draw.h>
#include <MobitRenderer/matrix.h>
#include <MobitRenderer/pages.h>

namespace mr::pages {

void Geo_Page::f3() const noexcept {
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
  // f3->print(ctx->get_config_const().grid.visible, true);
  f3->print(ctx->get_config()->geometry.grid.visible, true);

  f3->print("Layer Pointer: Global");
  f3->print("L ");
  f3->print((int)ctx->level_layer_, true);
}

void Geo_Page::process() {
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
}
void Geo_Page::draw() noexcept {
  if (should_redraw1) {
    BeginTextureMode(ctx->_textures->geo_layer1.get());

    ClearBackground(WHITE);

    mr::draw::draw_geo_and_poles_layer(
      ctx->get_selected_level()->get_const_geo_matrix(),
      0,
      BLACK
    );

    EndTextureMode();

    should_redraw1 = false;
    should_redraw = true;
  }
  
  if (should_redraw2) {
    BeginTextureMode(ctx->_textures->geo_layer2.get());

    ClearBackground(WHITE);

    mr::draw::draw_geo_and_poles_layer(
      ctx->get_selected_level()->get_const_geo_matrix(),
      1,
      BLACK
    );

    EndTextureMode();

    should_redraw2 = false;
    should_redraw = true;
  }

  if (should_redraw3) {
    BeginTextureMode(ctx->_textures->geo_layer3.get());

    ClearBackground(WHITE);

    mr::draw::draw_geo_and_poles_layer(
      ctx->get_selected_level()->get_const_geo_matrix(),
      2,
      BLACK
    );

    EndTextureMode();

    should_redraw3 = false;
    should_redraw = true;
  }

  if (should_redraw_feature1) {

    BeginTextureMode(ctx->_textures->feature_layer1.get());

    ClearBackground(WHITE);

    mr::draw::draw_geo_features_layer(
      ctx->get_selected_level()->get_const_geo_matrix(),
      ctx->_textures->geometry_editor,
      0, 
      BLACK
    );

    mr::draw::draw_geo_entrances(
      ctx->get_selected_level()->get_const_geo_matrix(),
      ctx->_textures->geometry_editor,
      BLACK
    );

    EndTextureMode();
  
    should_redraw_feature1 = false;
    should_redraw = true;
  }

  if (should_redraw_feature2) {
    BeginTextureMode(ctx->_textures->feature_layer2.get());

    ClearBackground(WHITE);

    mr::draw::draw_geo_features_layer(
      ctx->get_selected_level()->get_const_geo_matrix(),
      ctx->_textures->geometry_editor,
      1, 
      BLACK
    );

    EndTextureMode();
  
    should_redraw_feature2 = false;
    should_redraw = true;
  }

  if (should_redraw_feature3) {
    BeginTextureMode(ctx->_textures->feature_layer3.get());

    ClearBackground(WHITE);

    mr::draw::draw_geo_features_layer(
      ctx->get_selected_level()->get_const_geo_matrix(),
      ctx->_textures->geometry_editor,
      2, 
      BLACK
    );

    EndTextureMode();
  
    should_redraw_feature3 = false;
    should_redraw = true;
  }

  if (should_redraw) {
    BeginTextureMode(ctx->_textures->get_main_level_viewport());
    {
      ClearBackground(Color{190, 190, 190, 255});

      const auto& shader = ctx->_shaders->white_remover_apply_color(); 
      BeginShaderMode(shader);
      {
        const auto geo3 = ctx->_textures->geo_layer3.get().texture;
        const auto geo2 = ctx->_textures->geo_layer2.get().texture;
        const auto geo1 = ctx->_textures->geo_layer1.get().texture;
        
        const auto feature3 = ctx->_textures->feature_layer3.get().texture;
        const auto feature2 = ctx->_textures->feature_layer2.get().texture;
        const auto feature1 = ctx->_textures->feature_layer1.get().texture;

        SetShaderValueTexture(shader, GetShaderLocation(shader, "texture0"), geo1);
        DrawTexture(geo1, 0, 0, BLACK);
        SetShaderValueTexture(shader, GetShaderLocation(shader, "texture0"), feature3);
        DrawTexture(feature3, 0, 0, WHITE);

        SetShaderValueTexture(shader, GetShaderLocation(shader, "texture0"), geo2);
        DrawTexture(geo2, 0, 0, Color{0, 255, 0, 80});
        SetShaderValueTexture(shader, GetShaderLocation(shader, "texture0"), feature2);
        DrawTexture(feature2, 0, 0, WHITE);

        SetShaderValueTexture(shader, GetShaderLocation(shader, "texture0"), geo3);
        DrawTexture(geo3, 0, 0, Color{255, 0, 0, 80});
        SetShaderValueTexture(shader, GetShaderLocation(shader, "texture0"), feature1);
        DrawTexture(feature1, 0, 0, WHITE);
      }
      EndShaderMode();
    }
    EndTextureMode();

    should_redraw = false;
  }
  //

  ClearBackground(DARKGRAY);

  BeginMode2D(ctx->get_camera());

  auto level = ctx->get_selected_level();
  auto &mtx = level->get_const_geo_matrix();
  auto width = mtx.get_width(), height = mtx.get_height();

  DrawRectangle(0, 0, width * 20, height * 20, GRAY);
  DrawTexture(ctx->_textures->get_main_level_viewport().texture, 0, 0, WHITE);

  if (ctx->get_config()->geometry.grid.visible) {
    mr::draw::draw_nested_grid(
      width, 
      height, 
      Color{255, 255, 255, 90}
    );
  }

  mr::draw::draw_double_frame(level->get_pixel_width(), level->get_pixel_height());

  const auto &features_border = level->buffer_geos;

  DrawRectangleLinesEx(
    Rectangle {
      features_border.left * 20.0f,
      features_border.top * 20.0f,
      (level->get_width() - features_border.right - features_border.left) * 20.0f,
      (level->get_height() - features_border.bottom - features_border.top) * 20.0f,
    },
    4,
    WHITE
  );

  EndMode2D();
}
void Geo_Page::windows() noexcept {}

void Geo_Page::order_level_redraw() noexcept {
  should_redraw = true;
}

void Geo_Page::on_level_loaded() noexcept {
  should_redraw = true;
}

void Geo_Page::on_level_unloaded() noexcept {
  should_redraw = true;
}

void Geo_Page::on_level_selected() noexcept {
  should_redraw = true;
}

void Geo_Page::on_page_selected() noexcept {
  should_redraw = true;
}

Geo_Page::Geo_Page(context *ctx) : LevelPage(ctx), 
  should_redraw1(true), 
  should_redraw2(true), 
  should_redraw3(true),
  should_redraw_feature1(true),
  should_redraw_feature2(true),
  should_redraw_feature3(true),
  should_redraw(true) {}

Geo_Page::~Geo_Page() {}

};