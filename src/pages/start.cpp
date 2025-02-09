#include <exception>
#include <filesystem>
#include <memory>
#include <sstream>
#include <thread>

#include <raylib.h>
#include <imgui.h>
#include <rlImGui.h>

#include <spdlog/logger.h>

#include <MobitRenderer/imwin.h>
#include <MobitRenderer/pages.h>
#include <MobitRenderer/exceptions.h>
#include <MobitRenderer/serialization.h>
#include <MobitRenderer/state.h>

namespace mr::pages {

Start_Page::Start_Page(context *ctx)
    : LevelPage(ctx), explorer_(ctx->directories, ctx->_textures),
      loaded_level(nullptr), project_load_thread(nullptr),
      loaded_project_was_handled(true), explorer_file_clicked(false) {
  explorer_.set_filters({".txt"});
}

void Start_Page::process() {

  if (explorer_file_clicked && project_load_thread == nullptr) {
    const auto *file = explorer_.get_selected_entry_ptr();

    loaded_level = nullptr;
    project_thread_done = false;

    std::stringstream sb;
    sb << "begin loading level " << file->stem();
    ctx->logger->info(sb.str());

    // Really. Bad. Code.

    project_load_thread = std::make_unique<std::thread>([this, file]() {
      try {
      
        const std::filesystem::path path_copy = *file;
        this->loaded_level = mr::serde::deser_level(path_copy);
        mr::serde::define_tile_matrix(this->loaded_level->get_tile_matrix(), ctx->_tiledex, ctx->_materialdex);
        mr::serde::define_prop_list(this->loaded_level->props, ctx->_propdex);
        this->loaded_level->set_path(*file);
      
      } catch (const deserialization_failure &pf) {
        std::cout << "exception: " << pf.what() << std::endl;
      } catch (const std::exception &e) {
        std::cout << "exception: " << e.what() << std::endl;
      }

      loaded_project_was_handled = false;
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

    if (loaded_level != nullptr) {
      loaded_level->load_lightmap();

      ctx->_textures->resize_all_level_buffers(
        loaded_level->get_width()  * 20, 
        loaded_level->get_height() * 20
      );
      ctx->add_level(std::move(loaded_level));
      ctx->select_level(0);
      ctx->events.push(context_event{context_event_type::level_loaded, nullptr});
      ctx->events.push(context_event{context_event_type::level_selected, nullptr});
    }
  }
}

void Start_Page::draw() noexcept {
  if (project_load_thread == nullptr) {
    ClearBackground(DARKGRAY);
  } else {
    ClearBackground(BLACK);

    DrawTextEx(ctx->_fonts->get_large_default_font(), "Please wait..", Vector2{0, 30}, 30,0.2f, WHITE);
  }
}

void Start_Page::windows() noexcept {
  if (project_load_thread == nullptr) {
    explorer_file_clicked = explorer_.draw();
  }
}

void Start_Page::order_level_redraw() noexcept {}

Start_Page::~Start_Page() {
  if (project_load_thread != nullptr && !project_thread_done) {
    project_load_thread->detach();
  }
}

}; // namespace mr::pages
