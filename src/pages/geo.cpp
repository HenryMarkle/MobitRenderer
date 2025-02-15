#include <memory>
#include <cstdint>

#include <raylib.h>
#include <raymath.h>
#include <imgui.h>

#include <MobitRenderer/draw.h>
#include <MobitRenderer/matrix.h>
#include <MobitRenderer/pages.h>

#define EDIT_MODE_PLACE 0
#define EDIT_MODE_ERASE 1

namespace mr::pages {

void Geo_Page::f3() const noexcept {
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
  // f3->print(ctx->get_config_const().grid.visible, true);
  f3->print(ctx->get_config()->geometry.grid.visible, true);

  f3->print("Layer Pointer: Global");
  f3->print("L ");
  f3->print((int)ctx->level_layer_, true);

  f3->newline();

  f3->print("Hovered ");
  auto *level = ctx->get_selected_level();
  if (level != nullptr) {
    const auto &mtx = level->get_const_geo_matrix();
    const auto *cell = mtx.get_const_ptr(_mtx_mouse_pos.x, _mtx_mouse_pos.y, ctx->level_layer_);
    if (cell != nullptr) {
      f3->print(geo_type_cstr(cell->type), SKYBLUE, true);
    } else f3->print("-", true);
  } else f3->print("-", true);

  f3->print("Is Selecting ");
  f3->print(_is_selecting, true);

  f3->print("Edit Mode ");
  if (_edit_mode == EDIT_MODE_PLACE) f3->print("Place", GREEN, true);
  else if (_edit_mode == EDIT_MODE_ERASE) f3->print("Erase", RED, true);
  else f3->print("Unknown", MAGENTA, true);
}

void Geo_Page::on_mtx_pos_changed() noexcept {
  float originx = fmin(_selection_origin.x, _mtx_mouse_pos.x * 20.0f);
  float originy = fmin(_selection_origin.y, _mtx_mouse_pos.y * 20.0f);

  _selection_rect.x = originx;
  _selection_rect.y = originy;
  _selection_rect.width = abs(_selection_origin.x - _mtx_mouse_pos.x * 20.0f);
  _selection_rect.height = abs(_selection_origin.y - _mtx_mouse_pos.y * 20.0f);

  if (_selection_rect.width == 0) _selection_rect.width = 20;
  else _selection_rect.width += 20;

  if (_selection_rect.height == 0) _selection_rect.height = 20;
  else _selection_rect.height += 20;
}

void Geo_Page::process() {
  _update_mtx_mouse_pos();
  _update_arrows_mtx_camera_pos();

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

    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) _edit_mode = EDIT_MODE_PLACE;
    else if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) _edit_mode = EDIT_MODE_ERASE;

    if (_is_selecting) {

      if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) || IsMouseButtonReleased(MOUSE_BUTTON_RIGHT)) {
        _is_selecting = false;
        _selection_rect = Rectangle{0, 0, 0, 0};
        _selection_origin = Vector2{0, 0};
      }
    } else {
      if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) || IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
        _is_selecting = true;
        _selection_rect = Rectangle {
          _mtx_mouse_pos.x * 20.0f, 
          _mtx_mouse_pos.y * 20.0f, 
          20.0f, 
          20.0f
        };
        _selection_origin = Vector2{ 
          _mtx_mouse_pos.x * 20.0f, 
          _mtx_mouse_pos.y * 20.0f 
        };
      }
    }
  }


  _hovering_on_window = false;
}

void Geo_Page::draw() noexcept {
  if (should_redraw1) {
    BeginTextureMode(ctx->_textures->geo_layer1.get());

    ClearBackground(WHITE);

    mr::draw::draw_geo_and_poles_layer(
      ctx->get_selected_level()->get_const_geo_matrix(),
      0,
      BLACK
    );

    EndTextureMode();

    should_redraw1 = false;
    should_redraw = true;
  }
  
  if (should_redraw2) {
    BeginTextureMode(ctx->_textures->geo_layer2.get());

    ClearBackground(WHITE);

    mr::draw::draw_geo_and_poles_layer(
      ctx->get_selected_level()->get_const_geo_matrix(),
      1,
      BLACK
    );

    EndTextureMode();

    should_redraw2 = false;
    should_redraw = true;
  }

  if (should_redraw3) {
    BeginTextureMode(ctx->_textures->geo_layer3.get());

    ClearBackground(WHITE);

    mr::draw::draw_geo_and_poles_layer(
      ctx->get_selected_level()->get_const_geo_matrix(),
      2,
      BLACK
    );

    EndTextureMode();

    should_redraw3 = false;
    should_redraw = true;
  }

  if (should_redraw_feature1) {

    BeginTextureMode(ctx->_textures->feature_layer1.get());

    ClearBackground(WHITE);

    mr::draw::draw_geo_features_layer(
      ctx->get_selected_level()->get_const_geo_matrix(),
      ctx->_textures->geometry_editor,
      0, 
      BLACK
    );

    mr::draw::draw_geo_entrances(
      ctx->get_selected_level()->get_const_geo_matrix(),
      ctx->_textures->geometry_editor,
      BLACK
    );

    EndTextureMode();
  
    should_redraw_feature1 = false;
    should_redraw = true;
  }

  if (should_redraw_feature2) {
    BeginTextureMode(ctx->_textures->feature_layer2.get());

    ClearBackground(WHITE);

    mr::draw::draw_geo_features_layer(
      ctx->get_selected_level()->get_const_geo_matrix(),
      ctx->_textures->geometry_editor,
      1, 
      BLACK
    );

    EndTextureMode();
  
    should_redraw_feature2 = false;
    should_redraw = true;
  }

  if (should_redraw_feature3) {
    BeginTextureMode(ctx->_textures->feature_layer3.get());

    ClearBackground(WHITE);

    mr::draw::draw_geo_features_layer(
      ctx->get_selected_level()->get_const_geo_matrix(),
      ctx->_textures->geometry_editor,
      2, 
      BLACK
    );

    EndTextureMode();
  
    should_redraw_feature3 = false;
    should_redraw = true;
  }

  if (should_redraw) {
    BeginTextureMode(ctx->_textures->get_main_level_viewport());
    {
      ClearBackground(Color{190, 190, 190, 255});

      const auto& shader = ctx->_shaders->white_remover_apply_color(); 
      BeginShaderMode(shader);
      {
        const auto geo3 = ctx->_textures->geo_layer3.get().texture;
        const auto geo2 = ctx->_textures->geo_layer2.get().texture;
        const auto geo1 = ctx->_textures->geo_layer1.get().texture;
        
        const auto feature3 = ctx->_textures->feature_layer3.get().texture;
        const auto feature2 = ctx->_textures->feature_layer2.get().texture;
        const auto feature1 = ctx->_textures->feature_layer1.get().texture;

        SetShaderValueTexture(shader, GetShaderLocation(shader, "texture0"), geo1);
        DrawTexture(geo1, 0, 0, BLACK);
        SetShaderValueTexture(shader, GetShaderLocation(shader, "texture0"), feature3);
        DrawTexture(feature3, 0, 0, WHITE);

        SetShaderValueTexture(shader, GetShaderLocation(shader, "texture0"), geo2);
        DrawTexture(geo2, 0, 0, Color{0, 255, 0, 80});
        SetShaderValueTexture(shader, GetShaderLocation(shader, "texture0"), feature2);
        DrawTexture(feature2, 0, 0, WHITE);

        SetShaderValueTexture(shader, GetShaderLocation(shader, "texture0"), geo3);
        DrawTexture(geo3, 0, 0, Color{255, 0, 0, 80});
        SetShaderValueTexture(shader, GetShaderLocation(shader, "texture0"), feature1);
        DrawTexture(feature1, 0, 0, WHITE);
      }
      EndShaderMode();
    }
    EndTextureMode();

    should_redraw = false;
  }
  //

  ClearBackground(DARKGRAY);

  BeginMode2D(ctx->get_camera());

  auto level = ctx->get_selected_level();
  auto &mtx = level->get_const_geo_matrix();
  auto width = mtx.get_width(), height = mtx.get_height();

  DrawRectangle(0, 0, width * 20, height * 20, GRAY);
  DrawTexture(ctx->_textures->get_main_level_viewport().texture, 0, 0, WHITE);

  if (ctx->get_config()->geometry.grid.visible) {
    mr::draw::draw_nested_grid(
      width, 
      height, 
      Color{255, 255, 255, 90}
    );
  }

  mr::draw::draw_double_frame(level->get_pixel_width(), level->get_pixel_height());

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

  if (_is_selecting) {
    DrawRectangleLinesEx(_selection_rect, 2, _edit_mode == EDIT_MODE_PLACE ? WHITE : EDIT_MODE_ERASE ? RED : WHITE);
  } else {
    DrawRectangleLinesEx(
      Rectangle{ _mtx_mouse_pos.x * 20.0f - 2, _mtx_mouse_pos.y * 20.0f - 2, 20.0f + 4, 20.0f + 4 },
      2,
      _edit_mode == EDIT_MODE_PLACE ? WHITE : EDIT_MODE_ERASE ? RED : WHITE
    );
  }

  EndMode2D();
}
void Geo_Page::windows() noexcept {}

void Geo_Page::order_level_redraw() noexcept {
  should_redraw = true;
}

void Geo_Page::on_level_loaded() noexcept {
  should_redraw = true;
}

void Geo_Page::on_level_unloaded() noexcept {
  should_redraw = true;
}

void Geo_Page::on_level_selected() noexcept {
  should_redraw = true;
}

void Geo_Page::on_page_selected() noexcept {
  should_redraw = true;
}

Geo_Page::Geo_Page(context *ctx) : LevelPage(ctx), 
  _hovering_on_window(false),
  should_redraw1(true), 
  should_redraw2(true), 
  should_redraw3(true),
  should_redraw_feature1(true),
  should_redraw_feature2(true),
  should_redraw_feature3(true),
  should_redraw(true),
  _edit_mode(EDIT_MODE_PLACE),
  _is_selecting(false),
  _selection_rect(Rectangle{0, 0, 0, 0}),
  _selection_origin(Vector2{0, 0})
{}

Geo_Page::~Geo_Page() {}

};