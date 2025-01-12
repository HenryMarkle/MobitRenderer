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

#define STRINGIFY_DEFINED(x) #x
#define TO_STRING_DEFINED(x) STRINGIFY_DEFINED(x)

constexpr const char *PROJECT_VERSION =
    "Mobit Renderer v" TO_STRING_DEFINED(APP_VERSION_MAJOR) "." TO_STRING_DEFINED(
        APP_VERSION_MINOR) "." TO_STRING_DEFINED(APP_VERSION_PATCH);

using spdlog::logger;
using std::shared_ptr;
using std::string;

typedef std::unordered_map<mr::context_event_type,
                           void (*)(mr::context *, mr::pages::Pager *,
                                    const std::any &)>
    event_handlers;

int main() {
  // std::cout << "size is " << sizeof(mr::TileCell) << " bytes" << std::endl;

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

  shared_ptr<mr::context> ctx =
      std::make_shared<mr::context>(logger, directories);

  logger->info("------ starting program");
  logger->info("Mobit Renderer v{}.{}.{}", APP_VERSION_MAJOR, APP_VERSION_MINOR,
               APP_VERSION_PATCH);

  SetTargetFPS(40);

  //  SetWindowState(FLAG_MSAA_4X_HINT);

  InitWindow(1200, 800, "Mobit Renderer");

  SetWindowState(FLAG_WINDOW_RESIZABLE);

  SetWindowMinSize(1200, 800);

  rlImGuiSetup(true);

  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

  io.ConfigDockingWithShift = true;

  logger->info("initializing");

  ctx->get_shaders().reload_all();

  auto font_path = ctx->directories->get_fonts() / "Oswald-Regular.ttf";
  auto font = LoadFont(font_path.string().c_str());
  ctx->add_font(font);
  ctx->select_font(0);

  ctx->textures_->main_level_viewport = mr::rendertexture(72 * 20, 53 * 20);
  ctx->textures_->reload_all_textures();

  BeginTextureMode(ctx->textures_->get_main_level_viewport());
  ClearBackground(BLACK);
  EndTextureMode();

  auto pe =
      std::make_unique<mr::ProjectExplorer>(directories, ctx->textures_.get());

  logger->info("registering pages");

  auto *start_page = new mr::pages::Start_Page(ctx, logger);
  auto *main_page = new mr::pages::Main_Page(ctx, logger);
  auto *geo_page = new mr::pages::Geo_Page(ctx, logger);

  auto pager = std::make_unique<mr::pages::Pager>();

  pager->push_page(start_page);
  pager->push_page(main_page);
  pager->push_page(geo_page);

  // A page must be selected or a segmentation fault will be thrown.
  pager->select_page(0);

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
          pager->select_page(1);
        } else if (IsKeyPressed(KEY_TWO)) {
          pager->select_page(2);
        } else if (IsKeyPressed(KEY_THREE)) {
          pager->select_page(3);
        } else if (IsKeyPressed(KEY_FOUR)) {
          pager->select_page(4);
        } else if (IsKeyPressed(KEY_FIVE)) {
          pager->select_page(5);
        } else if (IsKeyPressed(KEY_SIX)) {
          pager->select_page(6);
        } else if (IsKeyPressed(KEY_SEVEN)) {
          pager->select_page(7);
        } else if (IsKeyPressed(KEY_EIGHT)) {
          pager->select_page(8);
        } else if (IsKeyPressed(KEY_NINE)) {
          pager->select_page(9);
        }
      }
    }

    pager->get_current_page()->process();

    BeginDrawing();
    {
      pager->get_current_page()->draw();

      rlImGuiBegin();
      {
        ImGui::DockSpaceOverViewport(ImGui::GetWindowDockID(),
                                     ImGui::GetMainViewport(),
                                     ImGuiDockNodeFlags_PassthruCentralNode);

        pager->get_current_page()->windows();
        auto current_page = pager->get_current_page_index();

        auto menubarOpened = ImGui::BeginMainMenuBar();

        if (current_page != 0 && menubarOpened) {
          auto goto_main = ImGui::MenuItem("Main", nullptr, current_page == 1, true);
          auto goto_geo = ImGui::MenuItem("Geometry", nullptr, current_page == 2, true);
          ImGui::MenuItem("Tiles", nullptr, current_page == 3, false);
          ImGui::MenuItem("Cameras", nullptr, current_page == 4, false);
          ImGui::MenuItem("Light", nullptr, current_page == 5, false);
          ImGui::MenuItem("Dimensions", nullptr, current_page == 6, false);
          ImGui::MenuItem("Effects", nullptr, current_page == 7, false);
          ImGui::MenuItem("Props", nullptr, current_page == 8, false);
          ImGui::MenuItem("Settings", nullptr, current_page == 9, false);

          if (goto_main) {
            ctx->events.push(mr::context_event{mr::context_event_type::goto_page, 1});
            
          } else if (goto_geo) {
            ctx->events.push(mr::context_event{mr::context_event_type::goto_page, 2});
          }
        }

        ImGui::EndMainMenuBar();
      }
      rlImGuiEnd();

      if (ctx->f3_enabled) {
        auto *f3 = ctx->f3_.get();

        f3->print(PROJECT_VERSION);

        f3->print("FPS ");
        f3->print(GetFPS(), true);

        f3->print("PID ");
        f3->print(pager->get_current_page_index(), true);

        f3->print(" PPID ", true);
        f3->print(pager->get_previous_page_index(), true);

        f3->print_queue();
      }
      ctx->f3_->reset();
    }
    EndDrawing();

    {
      // TODO: Paremeterize this.
      constexpr int EVENT_THRESHOLD = 30;
      auto handled_events = 0;

      while (!ctx->events.empty() && handled_events < EVENT_THRESHOLD) {
        auto &event = ctx->events.front();

        handlers[event.type](ctx.get(), pager.get(), event.payload);
        ctx->events.pop();

        handled_events++;
      }
    }
  }

  logger->info("exiting loop");

  pager.reset();
  ctx.reset();

  rlImGuiShutdown();

  CloseWindow();

  logger->info("------ program has terminated");

  return 0;
}
