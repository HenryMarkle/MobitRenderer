#include <iostream>
#include <string>
#include <vector>
#include <memory>

#ifdef __linux__
#include <unistd.h>
#include <limits.h>
#endif

#ifdef __APPLE__
#include <mach-o/dyld.h>
#include <limits.h>
#endif

#ifdef _WIN32
#include <windows.h>
#endif

#include "raylib.h"
#include "imgui.h"
#include "rlImGui.h"

#include "matrix.h"
#include "definitions.h"
#include "state.h"

#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"

using std::string;
using std::shared_ptr;
using spdlog::logger;

int main() {
    shared_ptr<mr::context> ctx = std::make_shared<mr::context>();
    shared_ptr<logger> logger = nullptr;

    try {
        logger = spdlog::basic_logger_mt("main logger", ctx->get_dirs().get_logs() / "logs.txt");
    } catch (const spdlog::spdlog_ex &ex) {
        std::cout << "Initializing logger has failed" << std::endl;
        throw ex;
    }

    ctx->set_logger(logger);

    logger->info("------ starting program");
    logger->info("Mobit Renderer v{}.{}.{}", APP_VERSION_MAJOR, APP_VERSION_MINOR, APP_VERSION_PATCH);

    SetTargetFPS(40);

    InitWindow(1200, 800, "Mobit Renderer");
    
    SetWindowMinSize(1200, 800);
    SetWindowState(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);

    rlImGuiSetup(true);

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; 

    io.ConfigDockingWithShift = true;

    logger->info("entering main loop");

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(DARKGRAY);

        DrawText("Hello", 0, 0, 30, WHITE);

        rlImGuiBegin();

        ImGui::DockSpaceOverViewport(ImGui::GetWindowDockID(), ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

        ImGui::Begin("Test");
        ImGui::Text("Hello");
        ImGui::End();

        rlImGuiEnd();

        EndDrawing();
    }

    logger->info("exiting loop");

    rlImGuiShutdown();

    CloseWindow();

    logger->info("------ program has terminated");

    return 0;
}