#include "MobitRenderer/managed.h"
#include <iostream>
#include <memory>
#include <string>

#include <imgui.h>
#include <raylib.h>
#include <rlImGui.h>

#include <MobitRenderer/definitions.h>
#include <MobitRenderer/imwin.h>
#include <MobitRenderer/matrix.h>
#include <MobitRenderer/state.h>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

using spdlog::logger;
using std::shared_ptr;
using std::string;

int main() {
  // std::cout << "size is " << sizeof(mr::TileCell) << " bytes" << std::endl;

  shared_ptr<mr::dirs> directories = std::make_shared<mr::dirs>();
  shared_ptr<logger> logger = nullptr;

  // Initializing logging
  //
  try {
    logger = spdlog::basic_logger_mt("main logger",
                                     directories->get_logs() / "logs.txt");
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

  ctx->textures_->main_level_viewport = mr::rendertexture(72 * 20, 53 * 20);
  ctx->textures_->reload_all_textures();

  BeginTextureMode(ctx->textures_->get_main_level_viewport());
  ClearBackground(BLACK);
  EndTextureMode();

  auto pe = std::make_unique<mr::ProjectExplorer>(directories, ctx->textures_);

  logger->info("entering main loop");

  while (!WindowShouldClose()) {

    // simple panning
    if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)) {
      auto delta = GetMouseDelta();
      auto &camera = ctx->get_camera();

      camera.target.x -= delta.x / camera.zoom;
      camera.target.y -= delta.y / camera.zoom;
    }

    BeginDrawing();
    ClearBackground(DARKGRAY);

    BeginMode2D(ctx->get_camera());

    DrawTexture(ctx->textures_->main_level_viewport.get().texture, 0, 0, WHITE);

    DrawText("Hello", 0, 0, 30, WHITE);

    EndMode2D();

    rlImGuiBegin();

    ImGui::DockSpaceOverViewport(ImGui::GetWindowDockID(),
                                 ImGui::GetMainViewport(),
                                 ImGuiDockNodeFlags_PassthruCentralNode);

    pe->draw();

    rlImGuiEnd();

    EndDrawing();
  }

  logger->info("exiting loop");

  rlImGuiShutdown();

  CloseWindow();

  logger->info("------ program has terminated");

  return 0;
}
