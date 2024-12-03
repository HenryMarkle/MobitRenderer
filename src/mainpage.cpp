#include <cstdint>
#include <raylib.h>

#include <MobitRenderer/draw.h>
#include <MobitRenderer/matrix.h>
#include <MobitRenderer/pages.h>

namespace mr::pages {

void Main_Page::process() {}
void Main_Page::draw() noexcept {
  ClearBackground(DARKGRAY);

  auto *level = ctx_->get_selected_level();
  auto &gmatrix = level->get_geo_matrix();

  for (uint16_t x = 0; x < gmatrix.get_width(); x++) {
    for (uint16_t y = 0; y < gmatrix.get_height(); y++) {
      auto cell = gmatrix.get_copy(x, y, 0);
      mr::draw_mtx_geo_type(cell.type, x, y, 20, BLACK);
    }
  }
}
void Main_Page::windows() noexcept {}

Main_Page::Main_Page(std::shared_ptr<context> ctx,
                     std::shared_ptr<spdlog::logger> logger)
    : Page(ctx, logger) {}

Main_Page::~Main_Page() {}

}; // namespace mr::pages
