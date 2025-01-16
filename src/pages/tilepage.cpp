#include <cstdint>
#include <memory>

#include <raylib.h>
#include <raymath.h>
#include <imgui.h>

#include <MobitRenderer/draw.h>
#include <MobitRenderer/matrix.h>
#include <MobitRenderer/pages.h>

namespace mr::pages {

void Tile_Page::process() {}
void Tile_Page::draw() noexcept {
    ClearBackground(DARKGRAY);
}
void Tile_Page::windows() noexcept {}
void Tile_Page::order_level_redraw() noexcept { should_redraw = true; }
void Tile_Page::f3() const noexcept {
    auto f3 = ctx->f3_;
  auto camera = ctx->get_camera();

  f3->print("Zoom ");
  f3->print(ctx->get_camera().zoom, true);

  f3->print("Target ");
  f3->print(ctx->get_camera().target, true);

  f3->print("Offset ");
  f3->print(ctx->get_camera().offset, true);

  {
    auto mouse = GetScreenToWorld2D(GetMousePosition(), ctx->get_camera());
    f3->print("Position ");
    f3->print(mouse, true);

    auto mtxMouse = Vector2Divide(mouse, {20, 20});
    f3->print("MTX ");
    f3->print((int)mtxMouse.x, true);
    f3->print(" / ", true);
    f3->print((int)mtxMouse.y, true);
  }

  f3->print("Grid ");
  f3->print(ctx->get_config_const().grid.visible, true);

  f3->print("Layer Pointer: Global");
  f3->print("L ");
  f3->print((int)ctx->level_layer_, true);
}

Tile_Page::Tile_Page(context *ctx) : 
    Page(ctx),
    should_redraw(true), 
    should_redraw1(true),
    should_redraw2(true),
    should_redraw3(true),
    should_redraw_tile1(true),
    should_redraw_tile2(true),
    should_redraw_tile3(true) {}

Tile_Page::~Tile_Page() {}

};