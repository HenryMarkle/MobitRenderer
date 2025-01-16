#if defined(_WIN32) || defined(_WIN64)
  #define WIN32_LEAN_AND_MEAN
#endif

#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>

#include <raylib.h>
#include <imgui.h>
#include <rlImGui.h>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

#include <MobitRenderer/definitions.h>
#include <MobitRenderer/imwin.h>
#include <MobitRenderer/managed.h>
#include <MobitRenderer/matrix.h>
#include <MobitRenderer/pages.h>
#include <MobitRenderer/state.h>
#include <MobitRenderer/events.h>
#include <MobitRenderer/dex.h>

#define STRINGIFY_DEFINED(x) #x
#define TO_STRING_DEFINED(x) STRINGIFY_DEFINED(x)

constexpr const char *PROJECT_VERSION =
    "Mobit Renderer v" TO_STRING_DEFINED(APP_VERSION_MAJOR) "." TO_STRING_DEFINED(
        APP_VERSION_MINOR) "." TO_STRING_DEFINED(APP_VERSION_PATCH);

using spdlog::logger;
using std::shared_ptr;
using std::string;

typedef std::unordered_map<mr::context_event_type,
                           void (*)(mr::context *, mr::pages::pager *,
                                    const std::any &)>
    event_handlers;

int main() {
  shared_ptr<mr::dirs> directories = std::make_shared<mr::dirs>();
  shared_ptr<logger> logger = nullptr;

  // Initializing logging
  //
  try {
    #ifdef _WIN32
    logger = spdlog::basic_logger_mt("main logger",
                                     (directories->get_logs() / "logs.txt").string());
    #else
    logger = spdlog::basic_logger_mt("main logger",
                                     (directories->get_logs() / "logs.txt"));
    #endif
  } catch (const spdlog::spdlog_ex &ex) {
    std::cout << "Initializing logger has failed" << std::endl;
    throw ex;
  }
  //


  logger->info("------ starting program");
  logger->info("Mobit Renderer v{}.{}.{}{}", APP_VERSION_MAJOR, APP_VERSION_MINOR,
               APP_VERSION_PATCH, IS_DEBUG_BUILD ? "Debug" : "");

  logger->info("initializing context");

  auto *textures = new mr::textures(directories);
  auto *ctx = new mr::context(logger, directories, textures);

  logger->info("importing tiles");
  
  auto *tiledex = new mr::TileDex();
  tiledex->register_from(directories->get_tiles() / "Init.txt");

  logger->info("initializing window");

  SetTargetFPS(40);

  InitWindow(1200, 800, "Mobit Renderer");

  SetWindowState(FLAG_WINDOW_RESIZABLE);

  SetWindowMinSize(1200, 800);

  rlImGuiSetup(true);

  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

  io.ConfigDockingWithShift = true;

  logger->info("loading font");

  ctx->get_shaders().reload_all();

  auto font_path = ctx->directories->get_fonts() / "Oswald-Regular.ttf";
  auto font = LoadFont(font_path.string().c_str());
  ctx->add_font(font);
  ctx->select_font(0);

  logger->info("initializing main viewport");

  textures->main_level_viewport = mr::rendertexture(72 * 20, 53 * 20);

  BeginTextureMode(textures->get_main_level_viewport());
  ClearBackground(BLACK);
  EndTextureMode();

  logger->info("loading textures");

  textures->reload_all_textures();

  auto pe = std::make_unique<mr::ProjectExplorer>(directories, textures);

  logger->info("initializing pages");

  auto *pager = new mr::pages::pager(ctx);

  logger->info("registering event handlers");

  event_handlers handlers = event_handlers();

  handlers[mr::context_event_type::level_loaded] = mr::handle_level_loaded;
  handlers[mr::context_event_type::goto_page] = mr::handle_goto_page;

  logger->info("entering main loop");

  while (!WindowShouldClose()) {
    { // Handle global shortcuts

      if (IsKeyPressed(KEY_F3)) {
        ctx->f3_enabled = !ctx->f3_enabled;
      }

      if (!ctx->get_levels().empty()) {
        if (IsKeyPressed(KEY_ZERO)) {
        }

        if (IsKeyPressed(KEY_ONE)) {
          pager->select(1);
        } else if (IsKeyPressed(KEY_TWO)) {
          pager->select(2);
        } else if (IsKeyPressed(KEY_THREE)) {
          pager->select(3);
        } else if (IsKeyPressed(KEY_FOUR)) {
          pager->select(4);
        } else if (IsKeyPressed(KEY_FIVE)) {
          pager->select(5);
        } else if (IsKeyPressed(KEY_SIX)) {
          pager->select(6);
        } else if (IsKeyPressed(KEY_SEVEN)) {
          pager->select(7);
        } else if (IsKeyPressed(KEY_EIGHT)) {
          pager->select(8);
        } else if (IsKeyPressed(KEY_NINE)) {
          pager->select(9);
        }
      }
    }

    pager->current_process();

    BeginDrawing();
    {
      pager->current_draw();

      rlImGuiBegin();
      {
        ImGui::DockSpaceOverViewport(ImGui::GetWindowDockID(),
                                     ImGui::GetMainViewport(),
                                     ImGuiDockNodeFlags_PassthruCentralNode);

        pager->current_windows();
        auto current_page = pager->get_selected_index();

        auto menubarOpened = ImGui::BeginMainMenuBar();

        if (current_page != 0 && menubarOpened) {
          auto goto_main = ImGui::MenuItem("Main", nullptr, current_page == 1, true);
          auto goto_geo = ImGui::MenuItem("Geometry", nullptr, current_page == 2, true);
          auto goto_tiles = ImGui::MenuItem("Tiles", nullptr, current_page == 3, true);
          ImGui::MenuItem("Cameras", nullptr, current_page == 4, false);
          ImGui::MenuItem("Light", nullptr, current_page == 5, false);
          ImGui::MenuItem("Dimensions", nullptr, current_page == 6, false);
          ImGui::MenuItem("Effects", nullptr, current_page == 7, false);
          ImGui::MenuItem("Props", nullptr, current_page == 8, false);
          ImGui::MenuItem("Settings", nullptr, current_page == 9, false);

          if (goto_main) {
            pager->select(1);
          } else if (goto_geo) {
            pager->select(2);
          } else if (goto_tiles) {
            pager->select(3);
          }
        }

        ImGui::EndMainMenuBar();
      }
      rlImGuiEnd();

      if (ctx->f3_enabled) {
        auto *f3 = ctx->f3_.get();

        f3->print(PROJECT_VERSION);
        
        #ifdef IS_DEBUG_BUILD
          f3->print(" Debug", true);
        #else
          f3->print(" Release", true);
        #endif

        f3->print("FPS ");
        f3->print(GetFPS(), true);

        f3->print("PID ");
        f3->print(pager->get_selected_index(), true);

        f3->print(" PPID ", true);
        f3->print(pager->get_previous_index(), true);

        auto *current_level = ctx->get_selected_level();
        
        if (current_level != nullptr) {
          f3->print("Project ");
          f3->print(current_level->get_name(), true);

          f3->print("W ");
          f3->print(current_level->get_width(), true);
          
          f3->print(" H ", true);
          f3->print(current_level->get_height(), true);
        }

        pager->current_f3();
      }
    }
    EndDrawing();

    {
      // TODO: Paremeterize this.
      constexpr int EVENT_THRESHOLD = 30;
      auto handled_events = 0;

      while (!ctx->events.empty() && handled_events < EVENT_THRESHOLD) {
        auto &event = ctx->events.front();

        handlers[event.type](ctx, pager, event.payload);
        ctx->events.pop();

        handled_events++;
      }
    }
  }

  logger->info("exiting loop");

  delete pager;
  delete ctx;
  delete tiledex;
  delete textures;

  rlImGuiShutdown();

  CloseWindow();

  logger->info("------ program has terminated");

  return 0;
}
