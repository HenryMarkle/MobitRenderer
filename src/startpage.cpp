#include "MobitRenderer/exceptions.h"
#include <exception>
#include <filesystem>
#include <memory>
#include <sstream>
#include <thread>

#include <imgui.h>
#include <raylib.h>
#include <rlImGui.h>
#include <spdlog/logger.h>

#include <MobitRenderer/imwin.h>
#include <MobitRenderer/pages.h>
#include <MobitRenderer/serialization.h>
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
    const auto *file = explorer_.get_selected_entry_ptr();

    loaded_level = nullptr;
    project_thread_done = loaded_project_was_handled = false;

    std::stringstream sb;
    sb << "begin loading level " << '"' << file << '"' << ".\n";
    ctx_->logger->info(sb.str());

    project_load_thread = std::make_unique<std::thread>([this, file]() {
      // TODO: Load, read, and parse the file into a
      // level object here.

      try {
        const std::filesystem::path path_copy = *file;
        auto parsed_level = deser_level(path_copy);
        this->loaded_level = nullptr;
      } catch (const deserialization_failure &pf) {
        std::cout << "exception: " << pf.what() << std::endl;
      } catch (const std::exception &e) {
        std::cout << "exception: " << e.what() << std::endl;
      }

      project_thread_done = true;
    });
  }

  if (!loaded_project_was_handled) {
    if (!project_thread_done && project_load_thread != nullptr) {
      return;
    }

    loaded_project_was_handled = project_thread_done = true;
    if (project_load_thread != nullptr) {
      project_load_thread->detach();
      project_load_thread = nullptr;
    }
  }
}
void Start_Page::draw() noexcept { ClearBackground(DARKGRAY); }

void Start_Page::windows() noexcept {
  explorer_file_clicked = explorer_.draw();
}

Start_Page::~Start_Page() {
  if (project_load_thread != nullptr && !project_thread_done) {
    project_load_thread->detach();
  }
}

}; // namespace mr::pages
