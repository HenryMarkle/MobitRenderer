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

// TODO: Move this definition to its own, dedicated file.
Page::Page(std::shared_ptr<context> ctx, std::shared_ptr<spdlog::logger> logger)
    : ctx_(ctx), logger_(logger) {}

Start_Page::Start_Page(std::shared_ptr<context> ctx,
                       std::shared_ptr<spdlog::logger> logger)
    : Page(ctx, logger), explorer_(ctx->directories, ctx->textures_),
      loaded_level(nullptr), project_load_thread(nullptr) {}

void Start_Page::process() {

  if (explorer_file_clicked && project_load_thread == nullptr) {
    const auto *file = explorer_.get_selected_entry();

    loaded_level = nullptr;

    project_load_thread = std::make_unique<std::thread>([this]() {
      // TODO: Load, read, and parse the file into a
      // level object here.

      // The marks its termination by setting the loaded_level to a valid
      // pointer.
      this->loaded_level = nullptr;
    });
  }

  // Project load protocol wait.
  if (project_load_thread != nullptr) {
  }
}
void Start_Page::draw() noexcept { ClearBackground(DARKGRAY); }

void Start_Page::windows() noexcept {
  explorer_file_clicked = explorer_.draw();
}

}; // namespace mr::pages
