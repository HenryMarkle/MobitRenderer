#include <cstdint>

#include <raylib.h>
#include <imgui.h>

#include <MobitRenderer/draw.h>
#include <MobitRenderer/matrix.h>
#include <MobitRenderer/pages.h>

namespace mr::pages {

void Geo_Page::order_redraw() noexcept { should_redraw = true; }
void Geo_Page::process() {
  auto f3 = ctx_->f3_;

  f3->enqueue("W ");
  f3->enqueue(ctx_->get_selected_level()->get_width(), true);
  f3->enqueue(" H ", true);
  f3->enqueue(ctx_->get_selected_level()->get_height(), true);
}
void Geo_Page::draw() noexcept {
    ClearBackground(DARKGRAY);

    BeginMode2D(ctx_->get_camera());

    auto level = ctx_->get_selected_level();
    auto &mtx = level->get_const_geo_matrix();
    auto width = mtx.get_width(), height = mtx.get_height();

    DrawRectangle(0, 0, width * 20, height * 20, GRAY);

    EndMode2D();
}
void Geo_Page::windows() noexcept {}

Geo_Page::Geo_Page(std::shared_ptr<context> ctx,
                     std::shared_ptr<spdlog::logger> logger
                ) : Page(ctx, logger), should_redraw(true) {}

Geo_Page::~Geo_Page() {}
};