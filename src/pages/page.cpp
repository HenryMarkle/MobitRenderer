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
    _mtx_mouse_pos({0, 0}),
    _mtx_mouse_prev_pos({0, 0})
{}

void LevelPage::_update_mtx_mouse_pos() noexcept {
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

    if (mtx_pos.x != _mtx_mouse_pos.x || mtx_pos.y != _mtx_mouse_pos.y) {
        _mtx_mouse_prev_pos = _mtx_mouse_pos;
        _mtx_mouse_pos = mr::ivec2{(int)mtx_pos.x, (int)mtx_pos.y};

        on_mtx_pos_changed();
    }
    
    _is_mouse_in_mtx_bounds = !(
        _mtx_mouse_pos.x < 0 || 
        _mtx_mouse_pos.y < 0 ||

        _mtx_mouse_pos.x >= w ||
        _mtx_mouse_pos.y >= h
    );
}

void LevelPage::_update_arrows_mtx_camera_pos() noexcept {
    if (ctx == nullptr) return;

    auto *level = ctx->get_selected_level();
    if (level == nullptr) return;

    auto &camera = ctx->get_camera();
    
    int speed = 1 + 
        (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) * 5 + 
        (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) * 5;

    bool left = IsKeyPressed(KEY_LEFT);
    bool up = IsKeyPressed(KEY_UP);
    bool right = IsKeyPressed(KEY_RIGHT);
    bool down = IsKeyPressed(KEY_DOWN);

    if (left) camera.target.x -= 20 * speed;
    if (up) camera.target.y -= 20 * speed;
    if (right) camera.target.x += 20 * speed;
    if (down) camera.target.y += 20 * speed;

    if (left || up || right || down) {
        if (camera.target.x < - 60) camera.target.x = -60;
        if (camera.target.y < - 60) camera.target.y = -60;
        if (camera.target.x > level->get_pixel_width()) camera.target.x = level->get_pixel_width(); 
        if (camera.target.y > level->get_pixel_height()) camera.target.y = level->get_pixel_height(); 
    }
}

void LevelPage::order_level_redraw() noexcept {}

void LevelPage::on_level_selected() noexcept {}
void LevelPage::on_level_loaded() noexcept {}
void LevelPage::on_level_created() noexcept {}
void LevelPage::on_level_unloaded() noexcept {}
void LevelPage::on_page_selected() noexcept {}
void LevelPage::on_mtx_pos_changed() {}

};