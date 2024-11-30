#include <raylib.h>

#include <MobitRenderer/pages.h>

namespace mr::pages {

void Main_Page::process() {}
void Main_Page::draw() noexcept {
  ClearBackground(DARKGRAY);
  DrawTextEx(*ctx_->get_selected_font(), "Hello", {0, 0}, 30, 0.2f, WHITE);
}
void Main_Page::windows() noexcept {}

Main_Page::Main_Page(std::shared_ptr<context> ctx,
                     std::shared_ptr<spdlog::logger> logger)
    : Page(ctx, logger) {}

Main_Page::~Main_Page() {}

}; // namespace mr::pages
