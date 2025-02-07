#include <raylib.h>
#include <raymath.h>

#include <MobitRenderer/pages.h>
#include <MobitRenderer/state.h>

namespace mr::pages {

Page::Page(context *ctx) : ctx(ctx) {}

void Page::process() {}
void Page::draw() noexcept {}
void Page::windows() noexcept {}
void Page::f3() const noexcept {}


LevelPage::LevelPage(context *ctx) : 
    Page(ctx), 
    _is_mouse_in_mtx_bounds(false), 
    _mtx_mouse_pos({0, 0}) 
{}

void LevelPage::update_mtx_mouse_pos() noexcept {
    if (ctx == nullptr) {
        _is_mouse_in_mtx_bounds = false;
        return;
    }
    
    auto *level = ctx->get_selected_level();
    if (level == nullptr) {
        _is_mouse_in_mtx_bounds = false;
        return;
    }

    auto w = level->get_width();
    auto h = level->get_height();

    auto pos = GetScreenToWorld2D(GetMousePosition(), ctx->get_camera());
    
    auto mtx_pos = Vector2Divide(pos, {20, 20});
    _mtx_mouse_pos = mr::ivec2{(int)mtx_pos.x, (int)mtx_pos.y};
    
    _is_mouse_in_mtx_bounds = !(
        _mtx_mouse_pos.x < 0 || 
        _mtx_mouse_pos.y < 0 ||

        _mtx_mouse_pos.x >= w ||
        _mtx_mouse_pos.y >= h
    );
}

void LevelPage::order_level_redraw() noexcept {}

void LevelPage::on_level_selected() noexcept {}
void LevelPage::on_level_loaded() noexcept {}
void LevelPage::on_level_created() noexcept {}
void LevelPage::on_level_unloaded() noexcept {}

};