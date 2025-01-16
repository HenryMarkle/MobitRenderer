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

Tile_Page::Tile_Page(std::shared_ptr<context> ctx) : 
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