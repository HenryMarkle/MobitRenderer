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

  auto f3 = ctx->f3_;
  auto camera = ctx->get_camera();

  f3->enqueue("W ");
  f3->enqueue(ctx->get_selected_level()->get_width(), true);
  f3->enqueue(" H ", true);
  f3->enqueue(ctx->get_selected_level()->get_height(), true);

  f3->enqueue("Zoom ");
  f3->enqueue(ctx->get_camera().zoom, true);

  f3->enqueue("Target ");
  f3->enqueue(ctx->get_camera().target, true);

  f3->enqueue("Offset ");
  f3->enqueue(ctx->get_camera().offset, true);

  {
    auto mouse = GetScreenToWorld2D(GetMousePosition(), ctx->get_camera());
    f3->enqueue("Position ");
    f3->enqueue(mouse, true);

    auto mtxMouse = Vector2Divide(mouse, {20, 20});
    f3->enqueue("MTX ");
    f3->enqueue((int)mtxMouse.x, true);
    f3->enqueue(" / ", true);
    f3->enqueue((int)mtxMouse.y, true);
  }

  f3->enqueue("Grid ");
  f3->enqueue(ctx->get_config_const().grid.visible, true);

  f3->enqueue("Layer Pointer: Global");
  f3->enqueue("L ");
  f3->enqueue((int)ctx->level_layer_, true);
}
void Geo_Page::draw() noexcept {
  if (should_redraw1) {
    BeginTextureMode(ctx->textures_->geo_layer1.get());

    ClearBackground(WHITE);

    auto *level = ctx->get_selected_level();
    auto &gmatrix = level->get_geo_matrix();

    for (uint16_t x = 0; x < gmatrix.get_width(); x++) {
      for (uint16_t y = 0; y < gmatrix.get_height(); y++) {
        auto cell1 = gmatrix.get_copy(x, y, 0);

        mr::draw_mtx_geo_type(cell1, x, y, 20, BLACK);
        mr::draw_mtx_geo_poles(cell1, x, y, 20, BLACK);
      }
    }

    EndTextureMode();

    should_redraw1 = false;
    should_redraw = true;
  }
  
  if (should_redraw2) {
    BeginTextureMode(ctx->textures_->geo_layer2.get());

    ClearBackground(WHITE);

    auto *level = ctx->get_selected_level();
    auto &gmatrix = level->get_geo_matrix();

    for (uint16_t x = 0; x < gmatrix.get_width(); x++) {
      for (uint16_t y = 0; y < gmatrix.get_height(); y++) {
        auto cell1 = gmatrix.get_copy(x, y, 1);

        mr::draw_mtx_geo_type(cell1, x, y, 20, BLACK);
        mr::draw_mtx_geo_poles(cell1, x, y, 20, BLACK);
      }
    }

    EndTextureMode();

    should_redraw2 = false;
    should_redraw = true;
  }

  if (should_redraw3) {
    BeginTextureMode(ctx->textures_->geo_layer3.get());

    ClearBackground(WHITE);

    auto *level = ctx->get_selected_level();
    auto &gmatrix = level->get_geo_matrix();

    for (uint16_t x = 0; x < gmatrix.get_width(); x++) {
      for (uint16_t y = 0; y < gmatrix.get_height(); y++) {
        auto cell1 = gmatrix.get_copy(x, y, 2);

        mr::draw_mtx_geo_type(cell1, x, y, 20, BLACK);
        mr::draw_mtx_geo_poles(cell1, x, y, 20, BLACK);
      }
    }

    EndTextureMode();

    should_redraw3 = false;
    should_redraw = true;
  }

  if (should_redraw_feature1) {
    BeginTextureMode(ctx->textures_->feature_layer1.get());

    ClearBackground(WHITE);

    auto *level = ctx->get_selected_level();
    auto &gmatrix = level->get_geo_matrix();

    for (uint16_t x = 0; x < gmatrix.get_width(); x++) {
      for (uint16_t y = 0; y < gmatrix.get_height(); y++) {
        auto cell1 = gmatrix.get_copy(x, y, 0);

        mr::draw_mtx_geo_features(cell1, x, y, 20, BLACK, ctx->textures_->geometry_editor);
      }
    }

    EndTextureMode();
  
    should_redraw_feature1 = false;
    should_redraw = true;
  }

  if (should_redraw_feature2) {
    BeginTextureMode(ctx->textures_->feature_layer2.get());

    ClearBackground(WHITE);

    auto *level = ctx->get_selected_level();
    auto &gmatrix = level->get_geo_matrix();

    for (uint16_t x = 0; x < gmatrix.get_width(); x++) {
      for (uint16_t y = 0; y < gmatrix.get_height(); y++) {
        auto cell1 = gmatrix.get_copy(x, y, 1);

        mr::draw_mtx_geo_features(cell1, x, y, 20, BLACK, ctx->textures_->geometry_editor);
      }
    }

    EndTextureMode();
  
    should_redraw_feature2 = false;
    should_redraw = true;
  }

  if (should_redraw_feature3) {
    BeginTextureMode(ctx->textures_->feature_layer3.get());

    ClearBackground(WHITE);

    auto *level = ctx->get_selected_level();
    auto &gmatrix = level->get_geo_matrix();

    for (uint16_t x = 0; x < gmatrix.get_width(); x++) {
      for (uint16_t y = 0; y < gmatrix.get_height(); y++) {
        auto cell1 = gmatrix.get_copy(x, y, 2);

        mr::draw_mtx_geo_features(cell1, x, y, 20, BLACK, ctx->textures_->geometry_editor);
      }
    }

    EndTextureMode();
  
    should_redraw_feature3 = false;
    should_redraw = true;
  }

  if (should_redraw) {
    BeginTextureMode(ctx->textures_->get_main_level_viewport());
    {
      ClearBackground(Color{190, 190, 190, 255});

      const auto& shader = ctx->get_shaders_const().white_remover_apply_color(); 
      BeginShaderMode(shader);
      {
        const auto geo3 = ctx->textures_->geo_layer3.get().texture;
        const auto geo2 = ctx->textures_->geo_layer2.get().texture;
        const auto geo1 = ctx->textures_->geo_layer1.get().texture;
        
        const auto feature3 = ctx->textures_->feature_layer3.get().texture;
        const auto feature2 = ctx->textures_->feature_layer2.get().texture;
        const auto feature1 = ctx->textures_->feature_layer1.get().texture;

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
  DrawTexture(ctx->textures_->get_main_level_viewport().texture, 0, 0, WHITE);

  if (ctx->get_config_const().grid.visible) {
    mr::draw_nested_grid(72, 43, Color{130, 130, 130, 200});
  }

  mr::draw_double_frame(level->get_pixel_width(), level->get_pixel_height());

  EndMode2D();
}
void Geo_Page::windows() noexcept {}

void Geo_Page::order_level_redraw() noexcept {
  should_redraw = true;
}

Geo_Page::Geo_Page(std::shared_ptr<context> ctx) : Page(ctx), 
  should_redraw1(true), 
  should_redraw2(true), 
  should_redraw3(true),
  should_redraw_feature1(true),
  should_redraw_feature2(true),
  should_redraw_feature3(true),
  should_redraw(true) {}

Geo_Page::~Geo_Page() {}

};