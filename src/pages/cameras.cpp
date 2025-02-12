#include <math.h>

#include <raylib.h>
#include <raymath.h>

#include <MobitRenderer/level.h>
#include <MobitRenderer/state.h>
#include <MobitRenderer/pages.h>

namespace mr::pages {

void Camera_Page::order_level_redraw() noexcept { _should_redraw = true; }

void Camera_Page::_draw_camera_sprite(
  const LevelCamera *level_camera,
  const Vector2 &mouse_pos,
  const Shader &shader,
  size_t label
) const noexcept {
  if (level_camera == nullptr) return;

  const auto &graf = ctx->_textures->cameras_editor.get_camera_graf();

  const auto &pos = level_camera->get_position();
  const auto &rect = level_camera->get_outer_rect();
  
  DrawRectangleLinesEx(
    rect,
    4,
    WHITE
  );
  DrawRectangleRec(
    rect, 
    Color{0, 255, 0, 50}
  );
  if (CheckCollisionPointRec(mouse_pos, rect)) {
    DrawRectangleRec(
      rect, 
      Color{255, 255, 255, 30}
    );

  }

  BeginShaderMode(shader);
  SetShaderValueTexture(shader, GetShaderLocation(shader, "texture0"), graf);
  DrawTexturePro(
    graf, 
    Rectangle{0, 0, static_cast<float>(graf.width), static_cast<float>(graf.height)}, 
    rect,
    Vector2{0,0},
    0,
    Color{0, 255, 0, 255}
  );
  EndShaderMode();
  
  // Quad points

  Vector2 tl_origin = level_camera->get_top_left_origin();
  Vector2 tr_origin = level_camera->get_top_right_origin();
  Vector2 br_origin = level_camera->get_bottom_right_origin();
  Vector2 bl_origin = level_camera->get_bottom_left_origin();
  
  Vector2 tl_point = level_camera->get_top_left_point();
  Vector2 tr_point = level_camera->get_top_right_point();
  Vector2 br_point = level_camera->get_bottom_right_point();
  Vector2 bl_point = level_camera->get_bottom_left_point();

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
    std::to_string(label).c_str(), 
    Vector2{pos.x + 10, pos.y + 10},
    30,
    0,
    WHITE
  );
}

void Camera_Page::process() {
  _update_mtx_mouse_pos();

  auto wheel = GetMouseWheelMove();
  auto &camera = ctx->get_camera();
  auto mouse_pos = GetScreenToWorld2D(GetMousePosition(), camera);

  if (!_hovering_on_window) {
    if (wheel != 0) {
      camera.offset = GetMousePosition();
      camera.target = mouse_pos;
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

    _is_hovering_camera = false;
    _hovered_camera_index = 0;
    _hovered_camera = nullptr;

    auto *level = ctx->get_selected_level();
    if (level != nullptr) {
      for (size_t c = 0; c < level->cameras.size(); c++) {
        auto &cam = level->cameras[c];

        if (CheckCollisionPointRec(mouse_pos, cam.get_outer_rect())) {
          _is_hovering_camera = true;
          _hovered_camera = &cam;
          _hovered_camera_index = c;
        }
      }

      if (_is_dragging_camera) {
        {
          auto delta = GetMouseDelta() / camera.zoom;
          auto new_pos = _dragged_camera->get_position();
  
          _dragged_camera->set_position(new_pos + delta);
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && _is_mouse_in_mtx_bounds) {
          _is_dragging_camera = false;
          _dragged_camera_index = 0;
          _dragged_camera = nullptr;
        } else if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
          level->cameras.erase(level->cameras.begin() + _dragged_camera_index);
          _is_dragging_camera = false;
          _dragged_camera_index = 0;
          _dragged_camera = nullptr;
        }
      } else {
        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
          level->cameras.push_back(LevelCamera(mouse_pos - Vector2{LevelCamera::pixel_width, LevelCamera::pixel_height}/2.0f));
          _is_dragging_camera = true;
          _dragged_camera_index = level->cameras.size() - 1;
          _dragged_camera = &level->cameras[_dragged_camera_index];
        } else if (_is_hovering_camera) {
          if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {            
            _is_dragging_camera = true;
            _dragged_camera_index = _hovered_camera_index;
            _dragged_camera = _hovered_camera;
          }
        }
      }
    }
  }

  _hovering_on_window = false;
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

    const auto &camera = ctx->get_camera();

    BeginMode2D(camera);

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

    auto mouse_pos = GetScreenToWorld2D(GetMousePosition(), camera);

    const auto &graf = ctx->_textures->cameras_editor.get_camera_graf();

    auto &cameras = level->cameras;

    const auto &shader = ctx->_shaders->white_remover();
    
    for (size_t c = 0; c < cameras.size(); c++) {
      if (_is_dragging_camera && c == _dragged_camera_index) continue;
      const auto &level_camera = cameras.at(c);
      _draw_camera_sprite(level_camera, mouse_pos, shader, c + 1);
    }

    if (_is_dragging_camera) {
      _draw_camera_sprite(_dragged_camera, mouse_pos, shader, _dragged_camera_index);
    }

    EndMode2D();
}

void Camera_Page::windows() noexcept {}
void Camera_Page::f3() const noexcept {
  auto f3 = ctx->f3_;
  
  const auto *level = ctx->get_selected_level();
  const auto &cameras = level->cameras;

  f3->newline();

  f3->print("Cameras ");
  f3->print(cameras.size(), true);

  f3->newline();

  f3->print("Is Hovering Camera ");
  f3->print(_is_hovering_camera, true);

  f3->print("Hovered ");
  f3->print(_hovered_camera, true);

  f3->print("Hovered Index ");
  f3->print(_hovered_camera_index, true);
  
  f3->print("Hovered Pos");
  if (_hovered_camera != nullptr) f3->print(_hovered_camera->get_position(), true);
  else f3->print("NULL", MAGENTA, true);

  f3->newline();

  f3->print("Is Dragging Camera ");
  f3->print(_is_dragging_camera, true);

  f3->print("Dragged ");
  f3->print(_dragged_camera, true);

  f3->print("Dragged Index ");
  f3->print(_dragged_camera_index, true);

  f3->print("Dragged Pos");
  if (_dragged_camera != nullptr) f3->print(_dragged_camera->get_position(), true);
  else f3->print("NULL", MAGENTA, true);
}

void Camera_Page::on_level_loaded() noexcept {
  _should_redraw1 = true;
  _should_redraw2 = true;
  _should_redraw3 = true;
//   _should_redraw_tile1 = true;
//   _should_redraw_tile2 = true;
//   _should_redraw_tile3 = true;
//   _should_redraw_props = true;
  _should_redraw = true;
}

void Camera_Page::on_level_unloaded() noexcept {
  _should_redraw1 = true;
  _should_redraw2 = true;
  _should_redraw3 = true;
//   _should_redraw_tile1 = true;
//   _should_redraw_tile2 = true;
//   _should_redraw_tile3 = true;
//   _should_redraw_props = true;
  _should_redraw = true;
}

void Camera_Page::on_level_selected() noexcept {
  _should_redraw1 = true;
  _should_redraw2 = true;
  _should_redraw3 = true;
//   _should_redraw_tile1 = true;
//   _should_redraw_tile2 = true;
//   _should_redraw_tile3 = true;
//   _should_redraw_props = true;
  _should_redraw = true;
}

void Camera_Page::on_page_selected() noexcept {
  _should_redraw1 = true;
  _should_redraw2 = true;
  _should_redraw3 = true;
//   _should_redraw_tile1 = true;
//   _should_redraw_tile2 = true;
//   _should_redraw_tile3 = true;
//   _should_redraw_props = true;
  _should_redraw = true;
}

Camera_Page::Camera_Page(context *ctx) 
    : LevelPage(ctx), 

    _should_redraw1(true),
    _should_redraw2(true),
    _should_redraw3(true),
    _should_redraw(true),

    _hovering_on_window(false),

    _is_dragging_camera(false),
    _is_hovering_camera(false),
    _dragged_camera_index(0),
    _hovered_camera_index(0),

    _hovered_camera(nullptr),
    _dragged_camera(nullptr)
{}

Camera_Page::~Camera_Page() {}

};