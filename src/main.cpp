#include <iostream>
#include <memory>
#include <string>
#include <sstream>
#include <exception>
#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <iomanip>
#include <filesystem>

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
#include <MobitRenderer/castlibs.h>
#include <MobitRenderer/dirs.h>

#define STRINGIFY_DEFINED(x) #x
#define TO_STRING_DEFINED(x) STRINGIFY_DEFINED(x)

using spdlog::logger;
using std::shared_ptr;
using std::string;

typedef std::unordered_map<mr::context_event_type, void (*)(mr::context *, mr::pages::pager *, const std::any &)> 
        event_handlers;

// Implementations appended to the end of the file.
//
int _missing_dirs_window(const mr::Dirs *d);
int _auto_render_window(int argc, char* argv[]);
//

int main(int argc, char* argv[]) {

  // Automated rendering mode
  if (argc > 1) {
    return _auto_render_window(argc, argv);
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
    logger = spdlog::basic_logger_mt("main logger",
                                     (directories->get_logs() / "logs.txt").string());
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
  tiledex->register_from(directories->get_cast() / "Drought_393439_Drought Needed Init.txt", castlibs);
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

    for (auto &entry : std::filesystem::directory_iterator(directories->get_tilepacks())) {
      if (!entry.is_directory()) continue;
      
      auto init_path = entry.path() / "Init.txt";

      if (!std::filesystem::exists(init_path)) continue;

      logger->info("loading tile pack init {}", init_path.string().c_str());
      tiledex->register_from(init_path, castlibs);
    }

    for (auto &entry : std::filesystem::directory_iterator(directories->get_proppacks())) {
      if (!entry.is_directory()) continue;
      
      auto init_path = entry.path() / "Init.txt";

      if (!std::filesystem::exists(init_path)) continue;

      logger->info("loading prop pack init {}", init_path.string().c_str());
      tiledex->register_from(init_path, castlibs);
    }
  }
  #endif

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
  handlers[mr::context_event_type::goto_page] = mr::handle_goto_page;

  logger->info("entering main loop");

  while (!WindowShouldClose()) {
    if (IsFileDropped()) {
      auto list = LoadDroppedFiles();
    }

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
    
      if (IsKeyPressed(KEY_S) && IsKeyDown(KEY_R)) {
        shaders->reload_all();
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
          auto goto_cameras = ImGui::MenuItem("Cameras", nullptr, current_page == 4, true);
          ImGui::MenuItem("Light", nullptr, current_page == 5, false);
          ImGui::MenuItem("Dimensions", nullptr, current_page == 6, false);
          ImGui::MenuItem("Effects", nullptr, current_page == 7, false);
          auto goto_props = ImGui::MenuItem("Props", nullptr, current_page == 8, true);
          auto goto_settings = ImGui::MenuItem("Settings", nullptr, current_page == 9, true);

          if (goto_main) {
            pager->select(1);
          } else if (goto_geo) {
            pager->select(2);
          } else if (goto_tiles) {
            pager->select(3);
          } else if (goto_cameras) {
            pager->select(4);
          }
          else if (goto_props) {
            pager->select(8);
          }
          else if (goto_settings) {
            pager->select(9);
          }
        }

        ImGui::EndMainMenuBar();
      }
      rlImGuiEnd();

      if (ctx->f3_enabled) {
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

        pager->current_f3();
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
    DrawText(
      "Missing Required Folders", 
      (GetScreenWidth() - MeasureText("Missing Required Folders", 50))/2,
      50,
      50,
      WHITE
    );

    DrawText("Assets/", 100, 230, 30, WHITE);
    DrawText("Assets/Shaders/", 100, 270, 30, WHITE);

    DrawText("Data/", 100, 320, 30, WHITE);
    DrawText("Data/Graphics/", 100, 360, 30, WHITE);
    DrawText("Data/Materials/", 100, 400, 30, WHITE);
    DrawText("Data/Props/", 100, 440, 30, WHITE);
    DrawText("Data/Cast/", 100, 480, 30, WHITE);

    DrawText(
      d->is_assets_found() ? "OK" : "MISSING", 
      500, 230, 30, 
      d->is_assets_found() ? GREEN : ORANGE
    );
    DrawText(
      d->is_shaders_found() ? "OK" : "MISSING", 
      500, 270, 30, 
      d->is_shaders_found() ? GREEN : ORANGE
    );

    DrawText(
      d->is_data_found() ? "OK" : "MISSING", 
      500, 320, 30, 
      d->is_data_found() ? GREEN : ORANGE
    );
    DrawText(
      d->is_tiles_found() ? "OK" : "MISSING", 
      500, 360, 30, 
      d->is_tiles_found() ? GREEN : ORANGE
    );
    DrawText(
      d->is_materials_found() ? "OK" : "MISSING", 
      500, 400, 30, 
      d->is_materials_found() ? GREEN : ORANGE
    );
    DrawText(
      d->is_props_found() ? "OK" : "MISSING", 
      500, 440, 30, 
      d->is_props_found() ? GREEN : ORANGE
    );
    DrawText(
      d->is_cast_found() ? "OK" : "MISSING", 
      500, 480, 30, 
      d->is_cast_found() ? GREEN : ORANGE
    );

    const auto *version_text = APP_NAME " v" APP_VERSION;

    #ifdef IS_DEBUG_BUILD
    version_text = TextFormat("%s Debug", version_text);
    #endif

    DrawText(
      version_text,
      10,
      GetScreenHeight() - 40,
      30,
      WHITE
    );

    EndDrawing();
  }
  CloseWindow();

  return 0;
}
int _auto_render_window(int argc, char* argv[]) {
  if (!std::strcmp(argv[1], "--help") || !std::strcmp(argv[1], "-h")) {
    const char *exec_name;

    #ifdef _WIN32
    exec_name = "MobitRenderer.exe";
    #else
    exec_name = "MobitRenderer";
    #endif
    
    std::cout 
      << "\nUsage\n\t" 
      << exec_name << " [flags] [options] <project file path>\n\n"
      << "Flags\n\t"
      << std::left << std::setw(30) << "--no-echo" << "No console feedback" << "\n\t"
      << std::left << std::setw(30) << "--no_light" << "Render without the light map" << "\n\t"
      << std::left << std::setw(30) << "--no-props" << "Render without props" << "\n\t"
      << std::left << std::setw(30) << "--no-effects" << "Render without effects" << "\n\t"
      << std::left << std::setw(30) << "--no-tiles" << "Render without tiles" << "\n\n"
      << "Options\n\t"
      << std::left << std::setw(30) << "--output=<PATH>" << "Sets the output directory" << "\n\t"
      << std::left << std::setw(30) << "--data=<PATH>" << "Sets the Data directory (including Cast)" << "\n\t"
      << std::left << std::setw(30) << "--cameras=<1,2,..>" << "Selects the cameras to render" << "\n\t"
      << std::left << std::setw(30) << "--tiles=<INIT FILE PATH>" << "Add tiles from an init file" << "\n\t"
      << std::left << std::setw(30) << "--props=<INIT FILE PATH>" << "Add props from an init file" << "\n\t"
      << std::left << std::setw(30) << "--materials=<INIT FILE PATH>" << "Add materials from an init file" << "\n\n";
    
    return 0;
  }

  shared_ptr<mr::Dirs> directories = std::make_shared<mr::Dirs>();
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

    #ifdef IS_DEBUG_BUILD
    logger->set_level(spdlog::level::level_enum::debug);
    #endif
  } 
  catch (const spdlog::spdlog_ex &ex) {
    std::cout << "initializing logger has failed" << std::endl;
    return -1;
  }
  //

  logger->info("------ starting program");
  
  logger->info("Mobit Renderer v{}", APP_VERSION);
  logger->info("build configuration: {}", BUILD_TYPE);
  logger->info("automated rendering mode is active");

  bool no_echo = false;

  bool no_light  = false, 
    no_props = false, 
    no_effects = false, 
    no_tiles = false;
  
  char 
    *input = nullptr, 
    *output = nullptr, 
    *data = nullptr,
    *data_no_cast = nullptr;

  std::vector<size_t> cameras;
  std::vector<std::filesystem::path> add_tiles, add_materials, add_props;

  input = argv[argc - 1];

  if (argc > 2) {
    // This is to ensure consistent --no-echo behavior
    for (int e = 1; e < argc - 1; e++) {
      if (!std::strcmp(argv[e], "--no-echo")) 
        no_echo = true;
    }

    for (int e = 1; e < argc - 1; e++) {
      char *arg = argv[e];

      if (!std::strcmp(arg, "--no-light"))   no_light   = true;
      if (!std::strcmp(arg, "--no-props"))   no_props   = true;
      if (!std::strcmp(arg, "--no-effects")) no_effects = true;
      if (!std::strcmp(arg, "--no-tiles"))   no_tiles   = true;
      
      if (!std::strncmp(arg, "--output=", 9)) output = arg + 9;
      if (!std::strncmp(arg, "--data=", 7)) data = arg + 7;
      if (!std::strncmp(arg, "--data-no-cast=", 15)) data_no_cast = arg + 15;
      if (!std::strncmp(arg, "--cameras=", 10)) {
        std::stringstream camstr(arg+10);
        std::string token;

        while (std::getline(camstr, token, ',')) {
          try {
            cameras.push_back(std::stoi(token) - 1);
          } catch (std::exception &e) {
            if (no_echo) std::cout 
              << "error while parsing --cameras: "
              << e.what();

            logger->error("failed to parse --cameras value: {}", e.what());
            
            return -2;
          }
        }
      }

      if (!std::strncmp(arg, "--tiles=", 8)) {
        add_tiles.push_back(std::filesystem::path(arg+8));
      }
      if (!std::strncmp(arg, "--props=", 8)) {
        add_props.push_back(std::filesystem::path(arg+8));
      }
      if (!std::strncmp(arg, "--materials=", 11)) {
        add_materials.push_back(std::filesystem::path(arg+11));
      }
    }
  }

  if (!std::strncmp(input, "--", 2)) {
    if (!no_echo) std::cout << "missing project file path argument" << std::endl;
    logger->error("missing project file path argument");
    return -3;
  }

  std::filesystem::path project_path(input);

  if (!std::filesystem::exists(project_path)) {
    if (!no_echo) std::cout << "project file does not exist" << std::endl;
    logger->error("project file does not exist: \"{}\"", project_path.string());
    return -4;
  }
  if (!std::filesystem::is_regular_file(project_path)) {
    if (!no_echo) std::cout << "project file path is not a file" << std::endl;
    logger->error("project file path is not a file: \"{}\"", project_path.string());

    return -5;
  }
  
  if (data != nullptr && data_no_cast != nullptr)
  {
    if (!no_echo) std::cout << "cannot provide both --data and --data-no-cast options" << std::endl;
    logger->error("cannot provide both --data and --data-no-cast options");
    return -6;
  }

  logger->debug("enabled flags:");
  logger->debug("no echo:    {}", no_echo);
  logger->debug("no light:   {}", no_light);
  logger->debug("no props:   {}", no_props);
  logger->debug("no effects: {}", no_effects);
  logger->debug("no tiles:   {}", no_tiles);

  logger->debug("project file: \"{}\"", input);
  
  if (!cameras.empty()) {
    std::stringstream ss;
    for (size_t c = 0; c < cameras.size(); c++) {
      ss << cameras[c];
      if (c != cameras.size() - 1) ss << ", ";
    }
    logger->debug("cameras: {}", ss.str());
  } else {
    logger->debug("cameras: ALL");
  }

  if (data != nullptr) logger->debug("Data directory: {}", data);
  if (data_no_cast != nullptr) logger->debug("Data directory (without Cast): {}", data_no_cast);

  if (output != nullptr) {
    std::filesystem::path output_dir(output);

    try {
      directories->set_levels(output_dir);
    } catch (std::invalid_argument &ia) {
      if (no_echo) std::cout << "output is not a directory" << std::endl;
      logger->error("output is not a directory");
      return -7;
    }
  }

  if (!directories->is_data_found()) {
    if (!no_echo) std::cout << "Data directory not found" << std::endl;
    logger->error("Data directory not found");
    return -8;
  }

  if (!directories->is_assets_found()) {
    if (!no_echo) std::cout << "Assets directory not found" << std::endl;
    logger->error("Assets directory not found");
    return -9;
  }

  logger->info("loading Cast libraries");

  auto *castlibs = new mr::CastLibs(directories->get_cast());
  castlibs->register_all();
  castlibs->load_all_members();

  logger->info("loading tiles");

  auto *tiledex = new mr::TileDex();
  tiledex->register_from(directories->get_tiles() / "Init.txt", castlibs);
  tiledex->register_from(directories->get_cast() / "Drought_393439_Drought Needed Init.txt", castlibs);
  for (auto &tpath : add_tiles) {
    if (!std::filesystem::exists(tpath) || std::filesystem::is_directory(tpath)) {
      if (!no_echo) std::cout << "tiles init path does not exist: " << tpath << std::endl;
      logger->error("tiles init path does not exist: \"{}\"", tpath.string());
      return -10;
    }

    tiledex->register_from(tpath, castlibs);
  }

  logger->info("loading materials");

  auto *materialdex = new mr::MaterialDex();
  materialdex->load_internals();

  logger->info("initializing window");

  SetTargetFPS(10);

  InitWindow(1400, 800, "Mobit Renderer");

  logger->info("loading fonts");

  auto *fonts = new mr::fonts(directories->get_fonts());
  fonts->reload_all();

  logger->info("loading shaders");

  auto *shaders = new mr::shaders(directories->get_shaders());
  shaders->reload_all();

  while (!WindowShouldClose()) {
  
    BeginDrawing();
    ClearBackground(WHITE);
    /// TODO: Continue here..
    EndDrawing();

  }

  logger->info("cleaning up");

  delete shaders;
  delete fonts;
  delete materialdex;
  delete tiledex;
  delete castlibs;

  CloseWindow();

  logger->info("------ program has terminated");
  return 0;
}

