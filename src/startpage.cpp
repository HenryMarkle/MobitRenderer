#include <memory>

#include <imgui.h>
#include <raylib.h>
#include <rlImGui.h>
#include <spdlog/logger.h>

#include <MobitRenderer/imwin.h>
#include <MobitRenderer/pages.h>
#include <MobitRenderer/state.h>

namespace mr::pages {

Page::Page(std::shared_ptr<context> ctx, std::shared_ptr<spdlog::logger> logger)
    : ctx_(ctx), logger_(logger) {}

Start_Page::Start_Page(std::shared_ptr<context> ctx,
                       std::shared_ptr<spdlog::logger> logger)
    : Page(ctx, logger), explorer_(ctx->directories, ctx->textures_) {}

void Start_Page::process() {}
void Start_Page::draw() noexcept { ClearBackground(DARKGRAY); }

void Start_Page::windows() noexcept { bool clicked = explorer_.draw(); }

}; // namespace mr::pages
