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
#include <MobitRenderer/serialization.h>
#include <MobitRenderer/imwin.h>
#include <MobitRenderer/managed.h>
#include <MobitRenderer/matrix.h>
#include <MobitRenderer/pages.h>
#include <MobitRenderer/state.h>
#include <MobitRenderer/events.h>
#include <MobitRenderer/dex.h>
#include <MobitRenderer/castlibs.h>
#include <MobitRenderer/dirs.h>
#include <MobitRenderer/renderer.h>
#include <MobitRenderer/level.h>
#include <MobitRenderer/winmodes.h>

using spdlog::logger;
using std::shared_ptr;
using std::string;

namespace mr {

int auto_render_window(int argc, char* argv[]) {
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
        << std::left << std::setw(30) << "--data-no-cast=<PATH>" << "Sets the Data directory (without Cast)" << "\n\t"
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
    
    const char *selected_cameras_cstr = nullptr;
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

            selected_cameras_cstr = arg + 10;
    
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
    
    logger->info("loading props");
    
    auto *propdex = new mr::PropDex();
    propdex->register_from(directories->get_props() / "Init.txt", castlibs);
        for (auto &ppath : add_props) {
        if (!std::filesystem::exists(ppath) || std::filesystem::is_directory(ppath)) {
        if (!no_echo) std::cout << "props init path does not exist: " << ppath << std::endl;
            logger->error("props init path does not exist: \"{}\"", ppath.string());
            return -11;
        }
    
        tiledex->register_from(ppath, castlibs);
    }
    propdex->register_tiles(tiledex);
    
    logger->info("loading materials");
    
    auto *materialdex = new mr::MaterialDex();
    materialdex->load_internals();
    
    
    #ifdef FEATURE_PALETTES
    logger->info("registering palettes");

    std::vector<mr::Palette> palettes{};
    
    if (directories->is_palettes_found()) {
        for (const auto &entry : std::filesystem::directory_iterator(directories->get_palettes())) {
            if (entry.path().extension() != ".png") continue;

            palettes.push_back(Palette(entry.path().stem().string(), entry.path()));
        }
    }
    #endif
    
    auto renderer = new mr::renderer::Renderer(
        directories, 
        logger, 
        tiledex, 
        propdex, 
        materialdex, 
        castlibs
    );
    
    logger->info("deserializing level");
    
    std::unique_ptr<mr::Level> level = nullptr;
    
    try {
        level = mr::serde::deser_level(project_path);
    
        mr::serde::define_tile_matrix(level->get_tile_matrix(), tiledex, materialdex);
        mr::serde::define_prop_list(level->props, propdex);
        level->set_path(project_path);
    
    } catch (std::exception &e) {
        logger->error("failed to deserialize level: {}", e.what());
        if (!no_echo) std::cout << "failed to deserialize level: " << e.what() << std::endl;
        return -13;
    }
    
    logger->info("initializing window");
    
    SetTargetFPS(15);
    
    #ifdef IS_DEBUG_BUILD
    InitWindow(1400 + 300, 800 + 35, "Mobit Renderer");
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetWindowMinSize(1400 + 300, 800 + 35);
    rlImGuiSetup(true);
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    io.ConfigDockingWithShift = true;
    #else
    InitWindow(1400, 800, "Mobit Renderer");
    #endif

    logger->info("loading lightmap");
    
    level->load_lightmap();

    logger->info("loading level");

    try {
        renderer->load(level.get());
    } catch (std::exception &e) {
        logger->error("failed to load renderer: {}", e.what());
        if (!no_echo) std::cout << "failed to load renderer: " << e.what() << std::endl;
        return -13;
    }

    renderer->reset_cleaned();
    
    logger->info("loading fonts");
    
    auto *fonts = new mr::fonts(directories->get_fonts());
    fonts->reload_all();
    
    logger->info("loading shaders");
    
    auto *shaders = new mr::shaders(directories->get_shaders());
    shaders->reload_all();

    #ifdef FEATURE_PALETTES
    logger->info("loading palettes");
    for (auto &p : palettes) p.reload();
    #endif
    
    logger->info("entering main loop");
    
    const char *initializing_cstr = "Initializing..";
    auto initializing_cstr_size = MeasureTextEx(fonts->get_large_default_font(), initializing_cstr, 30, 0.12f);
    
    const char *loading_cstr = "Loading Level..";
    auto loading_cstr_size = MeasureTextEx(fonts->get_large_default_font(), loading_cstr, 30, 0.12f);

    #ifdef FEATURE_PALETTES
    bool use_palette = false;
    size_t palette_index = 0;
    size_t selected_palette_index = 0;
    #endif

    int current_page = 1;
    size_t selected_layer_index = 0;

    while (!WindowShouldClose()) {

        if (!renderer->is_initialized()) {
            BeginDrawing();
            ClearBackground(DARKGRAY);
        
            DrawRectangleRec(
                Rectangle { 
                    (GetScreenWidth() - initializing_cstr_size.x)/2.0f - 5, 
                    (GetScreenHeight() - initializing_cstr_size.y)/2.0f - 2, 
                    initializing_cstr_size.x + 10, 
                    initializing_cstr_size.y + 4 
                },
                Color {0, 0, 0, 70}
            );

            DrawTextEx(
                fonts->get_large_default_font(), 
                initializing_cstr, 
                Vector2{
                    (GetScreenWidth() - initializing_cstr_size.x)/2.0f, 
                    (GetScreenHeight() - initializing_cstr_size.y)/2.0f
                },
                30,
                0.12f,
                WHITE
            );
            EndDrawing();

            if (renderer->frame_initialize(15)) {
                logger->info("renderer initialized");
                logger->info("preparing renderer");
                renderer->prepare();
            }

            continue;
        }

        if (!renderer->is_cleaned()) {
            BeginDrawing();
            ClearBackground(DARKGRAY);
        
            DrawRectangleRec(
                Rectangle { 
                    (GetScreenWidth() - initializing_cstr_size.x)/2.0f - 5, 
                    (GetScreenHeight() - initializing_cstr_size.y)/2.0f - 2, 
                    initializing_cstr_size.x + 10, 
                    initializing_cstr_size.y + 4 
                },
                Color {0, 0, 0, 70}
            );

            DrawTextEx(
                fonts->get_large_default_font(), 
                initializing_cstr, 
                Vector2{
                    (GetScreenWidth() - initializing_cstr_size.x)/2.0f, 
                    (GetScreenHeight() - initializing_cstr_size.y)/2.0f
                },
                30,
                0.12f,
                WHITE
            );
            EndDrawing();

            renderer->frame_cleanup(15);

            continue;
        }

        if (!renderer->is_preparation_done()) {
            BeginDrawing();
            ClearBackground(WHITE);
        
            DrawRectangleRec(
                Rectangle { 
                    (GetScreenWidth() - loading_cstr_size.x)/2.0f - 5, 
                    (GetScreenHeight() - loading_cstr_size.y)/2.0f - 2, 
                    loading_cstr_size.x + 10, 
                    loading_cstr_size.y + 4 
                },
                Color {0, 0, 0, 70}
            );

            DrawTextEx(
                fonts->get_large_default_font(), 
                loading_cstr, 
                Vector2{
                    (GetScreenWidth() - loading_cstr_size.x)/2.0f, 
                    (GetScreenHeight() - loading_cstr_size.y)/2.0f
                },
                30,
                0.12f,
                WHITE
            );
            EndDrawing();
        
            continue;
        }

        {
            // if (IsKeyPressed(KEY_ONE)) current_page = 1;
            // if (IsKeyPressed(KEY_TWO)) current_page = 2;
            // if (IsKeyPressed(KEY_THREE)) current_page = 3;
            // if (IsKeyPressed(KEY_FOUR)) current_page = 4;
            // if (IsKeyPressed(KEY_FIVE)) current_page = 5;
            // if (IsKeyPressed(KEY_SIX)) current_page = 6;
        }

        {
            BeginDrawing();
            #ifdef IS_DEBUG_BUILD
            ClearBackground(DARKGRAY);
            #else
            ClearBackground(WHITE);
            #endif
            
            renderer->frame_compose();

            #ifdef IS_DEBUG_BUILD
            rlImGuiBegin();
            {
                ImGui::DockSpaceOverViewport(
                    ImGui::GetWindowDockID(),
                    ImGui::GetMainViewport(), 
                    ImGuiDockNodeFlags_PassthruCentralNode
                );

                // if (ImGui::BeginMainMenuBar()) {
                //     if (ImGui::MenuItem("Main", "1", current_page == 1)) { current_page = 1; }
                //     if (ImGui::MenuItem("Lightmap", "2", current_page == 2)) { current_page = 2; }
                //     if (ImGui::MenuItem("Layers", "3", current_page == 3)) { current_page = 3; }
                //     if (ImGui::MenuItem("DC Layers", "4", current_page == 4)) { current_page = 4; }
                //     if (ImGui::MenuItem("Gradient A Layers", "5", current_page == 5)) { current_page = 5; }
                //     if (ImGui::MenuItem("Gradient B Layers", "6", current_page == 6)) { current_page = 6; }

                //     ImGui::EndMainMenuBar();
                // }

                if (ImGui::Begin("Info##AutoRendererInfo")) {
                    if (ImGui::BeginTable("##LevelInfo", 2, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders)) {
                        // ImGui::TableSetupColumn("Field");
                        // ImGui::TableSetupColumn("Value");

                        ImGui::TableNextRow();

                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("Name");

                        ImGui::TableSetColumnIndex(1);
                        ImGui::Text(level->get_name().c_str());

                        ImGui::TableNextRow();

                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("Size");

                        ImGui::TableSetColumnIndex(1);
                        ImGui::Text((std::to_string(level->get_width()) + " * " + std::to_string(level->get_height())).c_str());

                        ImGui::TableNextRow();

                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("Cameras");

                        ImGui::TableSetColumnIndex(1);
                        ImGui::Text(std::to_string(level->cameras.size()).c_str());

                        ImGui::TableNextRow();

                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("Seed");

                        ImGui::TableSetColumnIndex(1);
                        ImGui::Text(std::to_string(level->seed).c_str());

                        ImGui::TableNextRow();

                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("Water");

                        ImGui::TableSetColumnIndex(1);
                        ImGui::Text(std::to_string(level->water).c_str());

                        ImGui::TableNextRow();

                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("Water in front");

                        ImGui::TableSetColumnIndex(1);
                        ImGui::Text(level->front_water ? "Yes" : "No");

                        ImGui::TableNextRow();

                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("Light");

                        ImGui::TableSetColumnIndex(1);
                        ImGui::Text(level->light ? "Yes" : "No");

                        ImGui::TableNextRow();

                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("Light Angle");

                        ImGui::TableSetColumnIndex(1);
                        ImGui::Text(std::to_string(level->light_angle).c_str());

                        ImGui::TableNextRow();

                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("Light Flatness");

                        ImGui::TableSetColumnIndex(1);
                        ImGui::Text(std::to_string(level->light_flatness).c_str());

                        ImGui::TableNextRow();

                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("Terrain");

                        ImGui::TableSetColumnIndex(1);
                        ImGui::Text(level->terrain ? "Yes" : "No");
                        
                        ImGui::EndTable();
                    }
                }
                ImGui::End();

                if (ImGui::Begin("Render State##AutoRendererRenderState")) {
                    if (ImGui::BeginTable("##RenderState", 2, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders)) {
                        
                        ImGui::TableNextRow();

                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("FPS");

                        ImGui::TableSetColumnIndex(1);
                        ImGui::Text("15");

                        ImGui::TableNextRow();

                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("Selected Cameras");

                        ImGui::TableSetColumnIndex(1);
                        if (cameras.empty()) ImGui::Text("All");
                        else if (selected_cameras_cstr != nullptr) ImGui::Text(selected_cameras_cstr);
                        else ImGui::Text("Error");

                        ImGui::EndTable();
                    }

                    ImGui::Spacing();


                }
                ImGui::End();

                if (ImGui::Begin("Preview##AutoRendererPreview")) {
                    rlImGuiImageRenderTexture(&renderer->_composed_layers);
                }
                ImGui::End();

                if (ImGui::Begin("Final View##AutoRendererFinalView")) {
                    rlImGuiImageRenderTexture(&renderer->_final);
                }
                ImGui::End();

                if (ImGui::Begin("Layers##AutoRendererLayers")) {
                    if (ImGui::BeginTable("layout", 2, ImGuiTableFlags_Resizable)) {
                        ImGui::TableSetupColumn("layers_col", ImGuiTableColumnFlags_WidthFixed, 100);
                  
                        ImGui::TableNextRow();
                  
                        ImGui::TableSetColumnIndex(0);

                        if (ImGui::BeginListBox("##Layers", ImGui::GetContentRegionAvail())) {
                            for (size_t l = 0; l < 30; l++) {
                                if (ImGui::Selectable(std::to_string(l).c_str(), selected_layer_index == l)) { 
                                    selected_layer_index = l; 
                                }

                                if (ImGui::IsItemHovered()) {
                                    ImGui::BeginTooltip();
                                    rlImGuiImageRect(
                                        &renderer->_layers[l].texture, 
                                        1400/3, 800/3, 
                                        Rectangle{0, 0, 1400, 800}
                                    );
                                    ImGui::EndTooltip();
                                }
                            }
                            ImGui::EndListBox();
                        }

                        ImGui::TableSetColumnIndex(1);

                        rlImGuiImageRenderTextureFit(&renderer->_layers[selected_layer_index], false);

                        ImGui::EndTable();
                    }
                }
                ImGui::End();

                if (ImGui::Begin("DC Layers##AutoRendererDCLayers")) {
                    if (ImGui::BeginTable("layout", 2, ImGuiTableFlags_Resizable)) {
                        ImGui::TableSetupColumn("layers_col", ImGuiTableColumnFlags_WidthFixed, 100);
                  
                        ImGui::TableNextRow();
                  
                        ImGui::TableSetColumnIndex(0);

                        if (ImGui::BeginListBox("##Layers", ImGui::GetContentRegionAvail())) {
                            for (size_t l = 0; l < 30; l++) {
                                if (ImGui::Selectable(std::to_string(l).c_str(), selected_layer_index == l)) { 
                                    selected_layer_index = l; 
                                }

                                if (ImGui::IsItemHovered()) {
                                    ImGui::BeginTooltip();
                                    rlImGuiImageRect(
                                        &renderer->_dc_layers[l].texture, 
                                        1400/3, 800/3, 
                                        Rectangle{0, 0, 1400, 800}
                                    );
                                    ImGui::EndTooltip();
                                }
                            }
                            ImGui::EndListBox();
                        }

                        ImGui::TableSetColumnIndex(1);

                        rlImGuiImageRenderTextureFit(&renderer->_dc_layers[selected_layer_index], false);

                        ImGui::EndTable();
                    }
                }
                ImGui::End();

                if (ImGui::Begin("Gradient A Layers##AutoRendererGALayers")) {
                    if (ImGui::BeginTable("layout", 2, ImGuiTableFlags_Resizable)) {
                        ImGui::TableSetupColumn("layers_col", ImGuiTableColumnFlags_WidthFixed, 100);
                  
                        ImGui::TableNextRow();
                  
                        ImGui::TableSetColumnIndex(0);

                        if (ImGui::BeginListBox("##Layers", ImGui::GetContentRegionAvail())) {
                            for (size_t l = 0; l < 30; l++) {
                                if (ImGui::Selectable(std::to_string(l).c_str(), selected_layer_index == l)) { 
                                    selected_layer_index = l; 
                                }

                                if (ImGui::IsItemHovered()) {
                                    ImGui::BeginTooltip();
                                    rlImGuiImageRect(
                                        &renderer->_ga_layers[l].texture, 
                                        1400/3, 800/3, 
                                        Rectangle{0, 0, 1400, 800}
                                    );
                                    ImGui::EndTooltip();
                                }
                            }
                            ImGui::EndListBox();
                        }

                        ImGui::TableSetColumnIndex(1);

                        rlImGuiImageRenderTextureFit(&renderer->_ga_layers[selected_layer_index], false);

                        ImGui::EndTable();
                    }
                }
                ImGui::End();

                if (ImGui::Begin("Gradient B Layers##AutoRendererGBLayers")) {
                    if (ImGui::BeginTable("layout", 2, ImGuiTableFlags_Resizable)) {
                        ImGui::TableSetupColumn("layers_col", ImGuiTableColumnFlags_WidthFixed, 100);
                  
                        ImGui::TableNextRow();
                  
                        ImGui::TableSetColumnIndex(0);

                        if (ImGui::BeginListBox("##Layers", ImGui::GetContentRegionAvail())) {
                            for (size_t l = 0; l < 30; l++) {
                                if (ImGui::Selectable(std::to_string(l).c_str(), selected_layer_index == l)) { 
                                    selected_layer_index = l; 
                                }

                                if (ImGui::IsItemHovered()) {
                                    ImGui::BeginTooltip();
                                    rlImGuiImageRect(
                                        &renderer->_gb_layers[l].texture, 
                                        1400/3, 800/3, 
                                        Rectangle{0, 0, 1400, 800}
                                    );
                                    ImGui::EndTooltip();
                                }
                            }
                            ImGui::EndListBox();
                        }

                        ImGui::TableSetColumnIndex(1);

                        rlImGuiImageRenderTextureFit(&renderer->_gb_layers[selected_layer_index], false);

                        ImGui::EndTable();
                    }
                }
                ImGui::End();

                #ifdef FEATURE_PALETTES
                if (ImGui::Begin("Palettes")) {
                    ImGui::Checkbox("Use Palettes", &use_palette);
                    ImGui::Spacing();
                    
                    if (!use_palette) ImGui::BeginDisabled();
                    
                    if (ImGui::BeginListBox("##AutoRendererPlattes", ImGui::GetContentRegionAvail())) {
    
                        for (palette_index = 0; palette_index < palettes.size(); palette_index++) {
                            const auto &p = palettes[palette_index];
                            
                            if (
                                ImGui::Selectable((p.get_name()+"##"+std::to_string(palette_index)).c_str(), 
                                palette_index == selected_palette_index)
                            ) {
                                selected_palette_index = palette_index;
                            }
    
                            if (ImGui::IsItemHovered()) {
                                ImGui::BeginTooltip();
                                rlImGuiImageSize(&p.get_texture(), 32 * 10, 16 * 10);
                                ImGui::EndTooltip();
                            }
                        }
    
                        ImGui::EndListBox();
                    }
    
                    if (!use_palette) ImGui::EndDisabled();
                }
                ImGui::End();
                #endif
            }
            rlImGuiEnd();
            #endif
            
            EndDrawing();
        }
    }
    
    logger->info("exiting main loop");
    
    logger->info("cleaning up");

    #ifdef IS_DEBUG_BUILD
    rlImGuiShutdown();
    #endif

    delete renderer;
    delete shaders;
    delete fonts;
    delete materialdex;
    delete tiledex;
    delete propdex;
    delete castlibs;
    
    CloseWindow();
    
    logger->info("------ program has terminated");
    return 0;
    }

};
