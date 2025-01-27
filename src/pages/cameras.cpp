#include <math.h>

#include <raylib.h>
#include <raymath.h>

#include <MobitRenderer/level.h>
#include <MobitRenderer/state.h>
#include <MobitRenderer/pages.h>

namespace mr::pages {

void Camera_Page::order_level_redraw() noexcept { _should_redraw = true; }

void Camera_Page::process() {
    auto wheel = GetMouseWheelMove();

  if (wheel != 0) {

    auto &camera = ctx->get_camera();
    auto mouseWorldPosition = GetScreenToWorld2D(GetMousePosition(), camera);
    
    camera.offset = GetMousePosition();
    camera.target = mouseWorldPosition;
    camera.zoom += wheel * 0.125f;
    if (camera.zoom < 0.125f) camera.zoom = 0.125f;
  }

  if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)) {
    auto delta = GetMouseDelta();
    auto &camera = ctx->get_camera();

    delta = Vector2Scale(delta, -1.0f / camera.zoom);
    
    camera.target.x += delta.x;
    camera.target.y += delta.y;
  }
}

void Camera_Page::draw() noexcept {
    const auto *level = ctx->get_selected_level();
    if (level == nullptr) return;

    const auto &viewport = ctx->_textures->get_main_level_viewport();
    
    if (_should_redraw1) {
        BeginTextureMode(ctx->_textures->geo_layer1.get());

        ClearBackground(WHITE);

        mr::draw::draw_geo_and_poles_layer(
            ctx->get_selected_level()->get_const_geo_matrix(),
            0,
            BLACK
        );

        EndTextureMode();

        _should_redraw1 = false;
        _should_redraw = true;
    }
    
    if (_should_redraw2) {
        BeginTextureMode(ctx->_textures->geo_layer2.get());

        ClearBackground(WHITE);

        mr::draw::draw_geo_and_poles_layer(
            ctx->get_selected_level()->get_const_geo_matrix(),
            1,
            BLACK
        );

        EndTextureMode();

        _should_redraw2 = false;
        _should_redraw = true;
    }

    if (_should_redraw3) {
        BeginTextureMode(ctx->_textures->geo_layer3.get());

        ClearBackground(WHITE);

        mr::draw::draw_geo_and_poles_layer(
            ctx->get_selected_level()->get_const_geo_matrix(),
            2,
            BLACK
        );

        EndTextureMode();

        _should_redraw3 = false;
        _should_redraw = true;
    }

    if (_should_redraw) {
        BeginTextureMode(viewport);
        {
            ClearBackground(Color{200, 200, 200, 255});

            const auto &shader = ctx->_shaders->white_remover_apply_color();
            BeginShaderMode(shader);

            const auto &layer3 = ctx->_textures->geo_layer3.get().texture;
            SetShaderValueTexture(shader, GetShaderLocation(shader, "texture0"), layer3);
            DrawTexture(layer3, 0, 0, Color{50, 50, 50, 255});
            
            const auto &layer2 = ctx->_textures->geo_layer2.get().texture;
            SetShaderValueTexture(shader, GetShaderLocation(shader, "texture0"), layer2);
            DrawTexture(layer2, 0, 0, Color{20, 20, 20, 255});

            const auto &layer1 = ctx->_textures->geo_layer1.get().texture;
            SetShaderValueTexture(shader, GetShaderLocation(shader, "texture0"), layer1);
            DrawTexture(layer1, 0, 0, BLACK);

            EndShaderMode();
        }
        EndTextureMode();

        _should_redraw = false;
    }
    
    ClearBackground(DARKGRAY);

    BeginMode2D(ctx->get_camera());

    DrawTexture(viewport.texture, 0, 0, WHITE);

    const auto &features_border = level->buffer_geos;

    DrawRectangleLinesEx(
        Rectangle {
            features_border.left * 20.0f,
            features_border.top * 20.0f,
            (level->get_width() - features_border.right - features_border.left) * 20.0f,
            (level->get_height() - features_border.bottom - features_border.top) * 20.0f,
        },
        4,
        WHITE
    );

    auto &cameras = level->cameras;

    for (size_t c = 0; c < cameras.size(); c++) {
        auto &camera = cameras.at(c);
        auto &pos = camera.get_position();

        DrawRectangleLinesEx(camera.get_outer_rect(), 2, GREEN);
        
        // Quad points

        Vector2 tl_origin = camera.get_top_left_origin();
        Vector2 tr_origin = camera.get_top_right_origin();
        Vector2 br_origin = camera.get_bottom_right_origin();
        Vector2 bl_origin = camera.get_bottom_left_origin();
        
        Vector2 tl_point = camera.get_top_left_point();
        Vector2 tr_point = camera.get_top_right_point();
        Vector2 br_point = camera.get_bottom_right_point();
        Vector2 bl_point = camera.get_bottom_left_point();

        DrawLineEx(tl_point, tr_point, 2, LIME);
        DrawLineEx(tr_point, br_point, 2, LIME);
        DrawLineEx(br_point, bl_point, 2, LIME);
        DrawLineEx(bl_point, tl_point, 2, LIME);
        
        DrawLineEx(tl_origin, tl_point, 2, LIME);
        DrawLineEx(tr_origin, tr_point, 2, LIME);
        DrawLineEx(br_origin, br_point, 2, LIME);
        DrawLineEx(bl_origin, bl_point, 2, LIME);

        DrawCircleV(tl_point, 18, WHITE);
        DrawCircleV(tr_point, 18, WHITE);
        DrawCircleV(br_point, 18, WHITE);
        DrawCircleV(bl_point, 18, WHITE);
        
        DrawCircleV(tl_point, 14, BLACK);
        DrawCircleV(tr_point, 14, BLACK);
        DrawCircleV(br_point, 14, BLACK);
        DrawCircleV(bl_point, 14, BLACK);

        DrawCircleV(tl_point, 10, GREEN);
        DrawCircleV(tr_point, 10, GREEN);
        DrawCircleV(br_point, 10, GREEN);
        DrawCircleV(bl_point, 10, GREEN);

        DrawTextEx(
            ctx->_fonts->get_large_default_font(), 
            std::to_string(c + 1).c_str(), 
            Vector2{pos.x + 10, pos.y + 10},
            30,
            0,
            WHITE
        );
    }

    EndMode2D();
}

void Camera_Page::windows() noexcept {}
void Camera_Page::f3() const noexcept {
    auto f3 = ctx->f3_;
    
    const auto *level = ctx->get_selected_level();
    const auto &cameras = level->cameras;

    f3->print("Cameras ");
    f3->print(cameras.size(), true);
}

Camera_Page::Camera_Page(context *ctx) 
    : LevelPage(ctx), 

    _should_redraw1(true),
    _should_redraw2(true),
    _should_redraw3(true),
    _should_redraw(true)
{}

Camera_Page::~Camera_Page() {}

};