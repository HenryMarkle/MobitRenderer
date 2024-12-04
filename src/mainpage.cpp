#include "imgui.h"
#include <cstdint>
#include <raylib.h>

#include <MobitRenderer/draw.h>
#include <MobitRenderer/matrix.h>
#include <MobitRenderer/pages.h>

namespace mr::pages {

void Main_Page::order_redraw() noexcept { should_redraw = true; }

void Main_Page::process() {
  if (should_redraw) {
    BeginTextureMode(ctx_->textures_->get_main_level_viewport());

    ClearBackground(Color{.r = 240, .g = 240, .b = 240, .a = 255});

    auto *level = ctx_->get_selected_level();
    auto &gmatrix = level->get_geo_matrix();

    for (uint16_t x = 0; x < gmatrix.get_width(); x++) {
      for (uint16_t y = 0; y < gmatrix.get_height(); y++) {
        mr::GeoCell cell1 = gmatrix.get_copy(x, y, 0);
        mr::GeoCell cell2 = gmatrix.get_copy(x, y, 1);
        mr::GeoCell cell3 = gmatrix.get_copy(x, y, 2);

        mr::draw_mtx_geo_type(cell1.type, x, y, 20, BLACK);
        mr::draw_mtx_geo_type(cell2.type, x, y, 20,
                              Color{.r = 0, .g = 255, .b = 0, .a = 80});
        mr::draw_mtx_geo_type(cell3.type, x, y, 20,
                              Color{.r = 255, .g = 0, .b = 0, .a = 80});
      }
    }

    EndTextureMode();

    should_redraw = false;
  }

  auto f3 = ctx_->f3_;

  f3->enqueue("W ");
  f3->enqueue(ctx_->get_selected_level()->get_width(), true);
  f3->enqueue(" H ", true);
  f3->enqueue(ctx_->get_selected_level()->get_height(), true);
}
void Main_Page::draw() noexcept {
  ClearBackground(DARKGRAY);

  BeginMode2D(ctx_->get_camera());
  BeginShaderMode(ctx_->get_shaders_const().vflip());
  const Shader &shader = ctx_->get_shaders_const().vflip();
  const auto texture = ctx_->textures_->get_main_level_viewport().texture;
  SetShaderValueTexture(shader, GetShaderLocation(shader, "texture0"), texture);
  DrawTexture(texture, 0, 0, WHITE);
  EndShaderMode();
  EndMode2D();
}
void Main_Page::windows() noexcept {
  auto opened = ImGui::BeginMainMenuBar();

  if (opened) {
    ImGui::MenuItem("Main", nullptr, true, true);
    ImGui::MenuItem("Geometry", nullptr, false, true);
    ImGui::MenuItem("Tiles", nullptr, false, false);
    ImGui::MenuItem("Cameras", nullptr, false, false);
    ImGui::MenuItem("Light", nullptr, false, false);
    ImGui::MenuItem("Dimensions", nullptr, false, false);
    ImGui::MenuItem("Effects", nullptr, false, false);
    ImGui::MenuItem("Props", nullptr, false, false);
    ImGui::MenuItem("Settings", nullptr, false, false);

    ImGui::EndMainMenuBar();
  }
}

Main_Page::Main_Page(std::shared_ptr<context> ctx,
                     std::shared_ptr<spdlog::logger> logger)
    : Page(ctx, logger), should_redraw(true) {}

Main_Page::~Main_Page() {}

}; // namespace mr::pages
