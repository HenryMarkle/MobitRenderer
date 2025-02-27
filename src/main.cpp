#include <filesystem>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>

#include <imgui.h>
#include <raylib.h>
#include <rlImGui.h>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

#include <MobitRenderer/castlibs.h>
#include <MobitRenderer/definitions.h>
#include <MobitRenderer/dex.h>
#include <MobitRenderer/dirs.h>
#include <MobitRenderer/events.h>
#include <MobitRenderer/imwin.h>
#include <MobitRenderer/level.h>
#include <MobitRenderer/managed.h>
#include <MobitRenderer/matrix.h>
#include <MobitRenderer/pages.h>
#include <MobitRenderer/renderer.h>
#include <MobitRenderer/serialization.h>
#include <MobitRenderer/state.h>
#include <MobitRenderer/winmodes.h>

#define STRINGIFY_DEFINED(x) #x
#define TO_STRING_DEFINED(x) STRINGIFY_DEFINED(x)

using spdlog::logger;
using std::shared_ptr;
using std::string;

typedef std::unordered_map<mr::context_event_type,
                           void (*)(mr::context *, mr::pages::pager *,
                                    const std::any &)>
    event_handlers;

// Implementations appended to the end of the file.
//
int _missing_dirs_window(const mr::Dirs *d);
//

int main(int argc, char *argv[]) {

  // Automated rendering mode
  if (argc > 1) {
    return mr::auto_render_window(argc, argv);
  }

  auto directories = std::make_shared<mr::Dirs>();

  if (!directories->is_ok()) {
    return _missing_dirs_window(directories.get());
  }

  shared_ptr<logger> logger = nullptr;

  // Initializing logging
  //
  try {
#ifdef _WIN32
    logger = spdlog::basic_logger_mt(
        "main logger", (directories->get_logs() / "logs.txt").string());
#else
    logger = spdlog::basic_logger_mt("main logger",
                                     (directories->get_logs() / "logs.txt"));
#endif

#ifdef IS_DEBUG_BUILD
    logger->set_level(spdlog::level::level_enum::debug);
#endif
  } catch (const spdlog::spdlog_ex &ex) {
    std::cout << "Initializing logger has failed" << std::endl;
    throw ex;
  }
  //

  logger->info("------ starting program");

  logger->info("Mobit Renderer v{}", APP_VERSION);
  logger->info("build configuration: {}", BUILD_TYPE);

  logger->info("initializing context");

  auto *ctx = new mr::context(logger, directories);

  logger->info("loading cast libraries");

  auto *castlibs = new mr::CastLibs(directories->get_cast());
  castlibs->register_all();
  castlibs->load_all_members();
  ctx->_castlibs = castlibs;

  logger->info("loading tiles");

  auto *tiledex = new mr::TileDex();
  tiledex->register_from(directories->get_tiles() / "Init.txt", castlibs);
  tiledex->register_from(directories->get_cast() /
                             "Drought_393439_Drought Needed Init.txt",
                         castlibs);
  ctx->_tiledex = tiledex;

  logger->info("loading props");
  auto *propdex = new mr::PropDex();
  propdex->register_from(directories->get_props() / "Init.txt", castlibs);
  // propdex->register_from(directories->get_props() / "trees.txt", castlibs);
  propdex->register_tiles(tiledex);
  ctx->_propdex = propdex;

  logger->info("loading materials");

  auto *materialdex = new mr::MaterialDex();
  materialdex->load_internals();
  ctx->_materialdex = materialdex;

#ifdef FEATURE_DATAPACKS
  if (directories->is_datapacks_found()) { // Data packs
    logger->info("loading data packs");

    for (auto &entry :
         std::filesystem::directory_iterator(directories->get_tilepacks())) {
      if (!entry.is_directory())
        continue;

      auto init_path = entry.path() / "Init.txt";

      if (!std::filesystem::exists(init_path))
        continue;

      logger->info("loading tile pack init {}", init_path.string().c_str());
      tiledex->register_from(init_path, castlibs);
    }

    for (auto &entry :
         std::filesystem::directory_iterator(directories->get_proppacks())) {
      if (!entry.is_directory())
        continue;

      auto init_path = entry.path() / "Init.txt";

      if (!std::filesystem::exists(init_path))
        continue;

      logger->info("loading prop pack init {}", init_path.string().c_str());
      tiledex->register_from(init_path, castlibs);
    }
  }
#endif

  logger->info("initializing window");

  SetTargetFPS(40);

  InitWindow(1200, 800, "Mobit Renderer");

  SetWindowState(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_HIGHDPI);

  SetWindowMinSize(1200, 800);

  rlImGuiSetup(true);

  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

  io.ConfigDockingWithShift = true;

  logger->info("loading fonts");

  auto *fonts = new mr::fonts(directories->get_fonts());
  fonts->reload_all();
  ctx->_fonts = fonts;
  ctx->f3_->set_font(fonts->get_small_default_font());

  logger->info("loading shaders");

  auto *shaders = new mr::shaders(directories->get_shaders());
  shaders->reload_all();
  ctx->_shaders = shaders;

  logger->info("initializing main viewport");

  auto *textures = new mr::textures(directories);
  textures->reload_all_textures();

  ctx->_textures = textures;

  textures->main_level_viewport = mr::rendertexture(72 * 20, 53 * 20);

  BeginTextureMode(textures->get_main_level_viewport());
  ClearBackground(BLACK);
  EndTextureMode();

  logger->info("loading textures");

  auto pe = std::make_unique<mr::ProjectExplorer>(directories, textures);

  logger->info("initializing pages");

  auto *pager = new mr::pages::pager(ctx);

  logger->info("registering event handlers");

  event_handlers handlers = event_handlers();

  handlers[mr::context_event_type::level_loaded] = mr::handle_level_loaded;
  handlers[mr::context_event_type::level_selected] = mr::handle_level_selected;
  handlers[mr::context_event_type::goto_page] = mr::handle_goto_page;

  logger->info("entering main loop");

  while (!WindowShouldClose()) {
    if (IsFileDropped()) {
      auto list = LoadDroppedFiles();
    }

    { // Handle global shortcuts

      if (IsKeyPressed(KEY_F3)) {
        ctx->get_config()->f3 = !ctx->get_config()->f3;
      }

      if (!ctx->get_levels().empty() &&
          (IsKeyDown(KEY_LEFT_ALT) || IsKeyDown(KEY_RIGHT_ALT))) {
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

      if (IsKeyPressed(KEY_S) && IsKeyDown(KEY_R)) {
        shaders->reload_all();
      }
    }

    pager->get_selected()->process();

    BeginDrawing();
    {
      pager->get_selected()->draw();

      rlImGuiBegin();
      {
        ImGui::DockSpaceOverViewport(ImGui::GetWindowDockID(),
                                     ImGui::GetMainViewport(),
                                     ImGuiDockNodeFlags_PassthruCentralNode);

        pager->get_selected()->windows();
        auto current_page = pager->get_selected_index();

        auto menubarOpened = ImGui::BeginMainMenuBar();

        if (current_page != 0 && menubarOpened) {
          auto goto_main =
              ImGui::MenuItem("Main", nullptr, current_page == 1, true);
          auto goto_geo =
              ImGui::MenuItem("Geometry", nullptr, current_page == 2, true);
          auto goto_tiles =
              ImGui::MenuItem("Tiles", nullptr, current_page == 3, true);
          auto goto_cameras =
              ImGui::MenuItem("Cameras", nullptr, current_page == 4, true);
          auto goto_light =
              ImGui::MenuItem("Light", nullptr, current_page == 5, true);
          ImGui::MenuItem("Dimensions", nullptr, current_page == 6, false);
          ImGui::MenuItem("Effects", nullptr, current_page == 7, false);
          auto goto_props =
              ImGui::MenuItem("Props", nullptr, current_page == 8, true);
          auto goto_settings =
              ImGui::MenuItem("Settings", nullptr, current_page == 9, true);

          if (goto_main) {
            pager->select(1);
          } else if (goto_geo) {
            pager->select(2);
          } else if (goto_tiles) {
            pager->select(3);
          } else if (goto_cameras) {
            pager->select(4);
          } else if (goto_light) {
            pager->select(5);
          } else if (goto_props) {
            pager->select(8);
          } else if (goto_settings) {
            pager->select(9);
          }
        }

        ImGui::EndMainMenuBar();
      }
      rlImGuiEnd();

      if (ctx->get_config()->f3) {
        auto *f3 = ctx->f3_.get();

        f3->print(APP_NAME);
        f3->print(" v", true);
        f3->print(APP_VERSION, true);

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

        pager->get_selected()->f3();
        f3->reset();
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

  logger->info("cleaning up");

  delete pager;
  delete ctx;
  delete materialdex;
  delete propdex;
  delete tiledex;
  delete castlibs;
  delete shaders;
  delete textures;
  delete fonts;

  rlImGuiShutdown();

  CloseWindow();

  logger->info("------ program has terminated");

  return 0;
}

int _missing_dirs_window(const mr::Dirs *d) {
  SetTargetFPS(40);
  InitWindow(1200, 800, "Mobit Renderer");
  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(BLACK);
    DrawText("Missing Required Folders",
             (GetScreenWidth() - MeasureText("Missing Required Folders", 50)) /
                 2,
             50, 50, WHITE);

    DrawText("Assets/", 100, 230, 30, WHITE);
    DrawText("Assets/Shaders/", 100, 270, 30, WHITE);

    DrawText("Data/", 100, 320, 30, WHITE);
    DrawText("Data/Graphics/", 100, 360, 30, WHITE);
    DrawText("Data/Materials/", 100, 400, 30, WHITE);
    DrawText("Data/Props/", 100, 440, 30, WHITE);
    DrawText("Data/Cast/", 100, 480, 30, WHITE);

    DrawText(d->is_assets_found() ? "OK" : "MISSING", 500, 230, 30,
             d->is_assets_found() ? GREEN : ORANGE);
    DrawText(d->is_shaders_found() ? "OK" : "MISSING", 500, 270, 30,
             d->is_shaders_found() ? GREEN : ORANGE);

    DrawText(d->is_data_found() ? "OK" : "MISSING", 500, 320, 30,
             d->is_data_found() ? GREEN : ORANGE);
    DrawText(d->is_tiles_found() ? "OK" : "MISSING", 500, 360, 30,
             d->is_tiles_found() ? GREEN : ORANGE);
    DrawText(d->is_materials_found() ? "OK" : "MISSING", 500, 400, 30,
             d->is_materials_found() ? GREEN : ORANGE);
    DrawText(d->is_props_found() ? "OK" : "MISSING", 500, 440, 30,
             d->is_props_found() ? GREEN : ORANGE);
    DrawText(d->is_cast_found() ? "OK" : "MISSING", 500, 480, 30,
             d->is_cast_found() ? GREEN : ORANGE);

    const auto *version_text = APP_NAME " v" APP_VERSION;

#ifdef IS_DEBUG_BUILD
    version_text = TextFormat("%s Debug", version_text);
#endif

    DrawText(version_text, 10, GetScreenHeight() - 40, 30, WHITE);

    EndDrawing();
  }
  CloseWindow();

  return 0;
}
