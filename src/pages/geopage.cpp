#include <cstdint>

#include <raylib.h>
#include <imgui.h>

#include <MobitRenderer/draw.h>
#include <MobitRenderer/matrix.h>
#include <MobitRenderer/pages.h>

namespace mr::pages {

void Geo_Page::process() {
  auto f3 = ctx_->f3_;

  f3->enqueue("W ");
  f3->enqueue(ctx_->get_selected_level()->get_width(), true);
  f3->enqueue(" H ", true);
  f3->enqueue(ctx_->get_selected_level()->get_height(), true);

  f3->enqueue("Layer Pointer: Global");
  f3->enqueue("L ");
  f3->enqueue((int)ctx_->level_layer_, true);
}
void Geo_Page::draw() noexcept {
  if (should_redraw1) {
    BeginTextureMode(ctx_->textures_->geo_layer1.get());

    ClearBackground(WHITE);

    auto *level = ctx_->get_selected_level();
    auto &gmatrix = level->get_geo_matrix();

    for (uint16_t x = 0; x < gmatrix.get_width(); x++) {
      for (uint16_t y = 0; y < gmatrix.get_height(); y++) {
        auto cell1 = gmatrix.get_copy(x, y, 0);

        mr::draw_mtx_geo_type(cell1.type, x, y, 20, BLACK);
      }
    }

    EndTextureMode();

    should_redraw1 = false;
    should_redraw = true;
  }

  if (should_redraw2) {
    BeginTextureMode(ctx_->textures_->geo_layer2.get());

    ClearBackground(WHITE);

    auto *level = ctx_->get_selected_level();
    auto &gmatrix = level->get_geo_matrix();

    for (uint16_t x = 0; x < gmatrix.get_width(); x++) {
      for (uint16_t y = 0; y < gmatrix.get_height(); y++) {
        auto cell1 = gmatrix.get_copy(x, y, 1);

        mr::draw_mtx_geo_type(cell1.type, x, y, 20, BLACK);
      }
    }

    EndTextureMode();

    should_redraw2 = false;
    should_redraw = true;
  }

  if (should_redraw3) {
    BeginTextureMode(ctx_->textures_->geo_layer3.get());

    ClearBackground(WHITE);

    auto *level = ctx_->get_selected_level();
    auto &gmatrix = level->get_geo_matrix();

    for (uint16_t x = 0; x < gmatrix.get_width(); x++) {
      for (uint16_t y = 0; y < gmatrix.get_height(); y++) {
        auto cell1 = gmatrix.get_copy(x, y, 2);

        mr::draw_mtx_geo_type(cell1.type, x, y, 20, BLACK);
      }
    }

    EndTextureMode();

    should_redraw3 = false;
    should_redraw = true;
  }

  if (should_redraw) {
    BeginTextureMode(ctx_->textures_->get_main_level_viewport());
    {
      ClearBackground(Color{190, 190, 190, 255});

      const auto& shader = ctx_->get_shaders_const().white_remover_apply_color(); 
      BeginShaderMode(shader);
      {
        const auto texture3 = ctx_->textures_->geo_layer3.get().texture;
        const auto texture2 = ctx_->textures_->geo_layer2.get().texture;
        const auto texture1 = ctx_->textures_->geo_layer1.get().texture;

        SetShaderValueTexture(shader, GetShaderLocation(shader, "texture0"), texture1);
        DrawTexture(texture1, 0, 0, BLACK);

        SetShaderValueTexture(shader, GetShaderLocation(shader, "texture0"), texture2);
        DrawTexture(texture2, 0, 0, Color{0, 255, 0, 80});

        SetShaderValueTexture(shader, GetShaderLocation(shader, "texture0"), texture3);
        DrawTexture(texture3, 0, 0, Color{255, 0, 0, 80});
      }
      EndShaderMode();
    }
    EndTextureMode();

    should_redraw = false;
  }
  //

  ClearBackground(DARKGRAY);

  BeginMode2D(ctx_->get_camera());

  auto level = ctx_->get_selected_level();
  auto &mtx = level->get_const_geo_matrix();
  auto width = mtx.get_width(), height = mtx.get_height();

  DrawRectangle(0, 0, width * 20, height * 20, GRAY);
  DrawTexture(ctx_->textures_->get_main_level_viewport().texture, 0, 0, WHITE);

  EndMode2D();
}
void Geo_Page::windows() noexcept {}

void Geo_Page::order_level_redraw() noexcept {
  should_redraw = true;
}

Geo_Page::Geo_Page(std::shared_ptr<context> ctx,
                     std::shared_ptr<spdlog::logger> logger
                ) : Page(ctx, logger), should_redraw1(true), should_redraw2(true), should_redraw3(true) {}

Geo_Page::~Geo_Page() {}
};