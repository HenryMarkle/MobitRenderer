#include <memory>
#include <thread>

#include <imgui.h>
#include <raylib.h>
#include <rlImGui.h>
#include <spdlog/logger.h>

#include <MobitRenderer/imwin.h>
#include <MobitRenderer/pages.h>
#include <MobitRenderer/state.h>

namespace mr::pages {

Page::Page(std::shared_ptr<context> ctx, std::shared_ptr<spdlog::logger> logger)
    : ctx_(ctx), logger_(logger) {}

Start_Page::Start_Page(std::shared_ptr<context> ctx,
                       std::shared_ptr<spdlog::logger> logger)
    : Page(ctx, logger), explorer_(ctx->directories, ctx->textures_),
      project_load_thread(nullptr) {}

void Start_Page::process() {
  const auto *file = explorer_.get_selected_entry();

  // TODO: load a function in another thread then navigate to the main screen

  if (explorer_file_clicked && project_load_thread == nullptr) {
    project_load_thread = std::make_unique<std::thread>([]() {});
  }
}
void Start_Page::draw() noexcept { ClearBackground(DARKGRAY); }

void Start_Page::windows() noexcept {
  explorer_file_clicked = explorer_.draw();
}

}; // namespace mr::pages
