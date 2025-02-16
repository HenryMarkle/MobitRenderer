#include <memory>
#include <cstdint>
#include <vector>

#ifdef IS_DEBUG_BUILD
#include <iostream>
#endif

#include <raylib.h>
#include <raymath.h>
#include <imgui.h>
#include <rlimgui.h>

#include <MobitRenderer/draw.h>
#include <MobitRenderer/matrix.h>
#include <MobitRenderer/pages.h>

#define EDIT_MODE_PLACE 0
#define EDIT_MODE_ERASE 1

namespace mr::pages {

bool Geo_Page::_allowed() const noexcept {
  if (ctx == nullptr) return false;

  const auto layer = ctx->level_layer_;

  if (layer > 0 && _geo_category_index == 2 && (_geo_index == 2 || _geo_index == 3)) return false;
  if (layer > 0 && _geo_category_index == 3) return false;
  return true;
}

void Geo_Page::_place(
  uint16_t x, 
  uint16_t y, 
  uint16_t z, 
  uint16_t width, 
  uint16_t height, 
  uint16_t depth
) noexcept {
  if (width == 0 || height == 0 || depth == 0) return;

  if (ctx == nullptr) return;

  if (!_allowed()) return;

  auto *level = ctx->get_selected_level();
  if (level == nullptr) return;

  auto &mtx = level->get_geo_matrix();

  if (!mtx.is_in_bounds(x, y, z)) return;

  if ((uint16_t)(x + width) < x || (uint16_t)(y + height) < y || (uint16_t)(z + depth) < z) return;
  if (!mtx.is_in_bounds(x + width, y + height, z + depth)) return;

  switch (_geo_category_index) {
    // Type
    case 0:
    {
      GeoType t;
      switch (_geo_index) {
        case 0:  t = GeoType::solid;    break;
        case 1:  t = GeoType::slope_ne; break;
        case 2:  t = GeoType::glass;    break;
        default: t = GeoType::air;      break;
      }

      // This is needed to place all slopes "at once"
      std::vector<GeoCell*> cells_to_slopes{};
      std::vector<GeoType> cells_to_slopes_types{};

      // This is needed to prevent double slopes
      if (is_geo_type_slope(t)) {
        for (uint16_t cx = x; cx < x + width; cx++) {
          for (uint16_t cy = y; cy < y + height; cy++) {
            for (uint16_t cz = z; cz < z + depth; cz++) {
              auto *cell = mtx.get_ptr(cx, cy, cz);
    
              const auto *left = mtx.get_const_ptr(cx -1, cy, cz);
              const auto *top = mtx.get_const_ptr(cx, cy - 1, cz);
              const auto *right = mtx.get_const_ptr(cx + 1, cy, cz);
              const auto *bottom = mtx.get_const_ptr(cx , cy + 1, cz);
            
              if (
                left && 
                top && 
                left->is_solid() && 
                top->is_solid() && 
                (bottom == nullptr || !bottom->is_solid()) && 
                (right == nullptr || !right->is_solid())
              ) {
                if (cell->is_air())  cell->type = GeoType::slope_es;
              } else if (
                top && 
                right && 
                top->is_solid() && 
                right->is_solid() &&
                (left == nullptr || !left->is_solid()) &&
                (bottom == nullptr || !bottom->is_solid())
              ) {
                if (cell->is_air())  cell->type = GeoType::slope_sw;
              } else if (
                right && 
                bottom && 
                right->is_solid() && 
                bottom->is_solid() &&
                (top == nullptr || !top->is_solid()) &&
                (left == nullptr || !left->is_solid())
              ) {
                if (cell->is_air())  cell->type = GeoType::slope_nw;
              } else if (
                bottom && 
                left && 
                bottom->is_slope() && 
                left->is_solid() &&
                (right == nullptr || !right->is_solid()) &&
                (top == nullptr || !top->is_solid())
              ) {
                if (cell->is_air()) cell->type = GeoType::slope_ne;
              }
            }
          }
        }
      }

      for (uint16_t cx = x; cx < x + width; cx++) {
        for (uint16_t cy = y; cy < y + height; cy++) {
          for (uint16_t cz = z; cz < z + depth; cz++) {
            auto *cell = mtx.get_ptr(cx, cy, cz);
  
            if (is_geo_type_slope(t)) {
              const auto *left = mtx.get_const_ptr(cx -1, cy, cz);
              const auto *top = mtx.get_const_ptr(cx, cy - 1, cz);
              const auto *right = mtx.get_const_ptr(cx + 1, cy, cz);
              const auto *bottom = mtx.get_const_ptr(cx , cy + 1, cz);
            
              if (
                left && 
                top && 
                left->is_solid() && 
                top->is_solid() && 
                (bottom == nullptr || !(bottom->is_solid() || bottom->is_slope())) && 
                (right == nullptr || !(right->is_solid() || right->is_slope()))
              ) {
                cells_to_slopes.push_back(cell);
                cells_to_slopes_types.push_back(GeoType::slope_es);
              } else if (
                top && 
                right && 
                top->is_solid() && 
                right->is_solid() &&
                (left == nullptr || !(left->is_solid() || left->is_slope())) &&
                (bottom == nullptr || !(bottom->is_solid() || bottom->is_slope()))
              ) {
                cells_to_slopes.push_back(cell);
                cells_to_slopes_types.push_back(GeoType::slope_sw);
              } else if (
                right && 
                bottom && 
                right->is_solid() && 
                bottom->is_solid() &&
                (top == nullptr || !(top->is_solid() || top->is_slope())) &&
                (left == nullptr || !(left->is_solid() || left->is_slope()))
              ) {
                cells_to_slopes.push_back(cell);
                cells_to_slopes_types.push_back(GeoType::slope_nw);
              } else if (
                bottom && 
                left && 
                bottom->is_slope() && 
                left->is_solid() &&
                (right == nullptr || !(right->is_solid() || right->is_slope())) &&
                (top == nullptr || !(top->is_solid() || top->is_slope()))
              ) {
                cells_to_slopes.push_back(cell);
                cells_to_slopes_types.push_back(GeoType::slope_ne);
              }
            } else {
              cell->type = t;
            }
          }
        }
      }
    
      for (size_t c = 0; c < cells_to_slopes.size(); c++) {
        cells_to_slopes[c]->type = cells_to_slopes_types[c];
      }
    }
    break;

    // Features
    default:
    {
      GeoFeature f;

      switch (_geo_category_index) {
        case 1:
        switch (_geo_index) {
          case 0: f = GeoFeature::vertical_pole; break;
          case 1: f = GeoFeature::horizontal_pole; break;
          case 2: f = GeoFeature::cracked_terrain; break;
        }        
        break;

        case 2:
        switch (_geo_index) {
          case 0: f = GeoFeature::bathive; break;
          case 1: f = GeoFeature::forbid_fly_chains; break;
          case 2: f = GeoFeature::worm_grass; break;
          case 3: f = GeoFeature::waterfall; break;
        } 
        break;

        case 3:
        switch (_geo_index) {
          case 0: f = GeoFeature::shortcut_entrance; break;
          case 1: f = GeoFeature::shortcut_path; break;
          case 2: f = GeoFeature::room_entrance; break;
          case 3: f = GeoFeature::dragon_den; break;
          case 4: f = GeoFeature::wack_a_mole_hole; break;
          case 5: f = GeoFeature::scavenger_hole; break;
          case 6: f = GeoFeature::garbage_worm_hole; break;
        } 
        break;
      }
    
      for (uint16_t cx = x; cx < x + width; cx++) {
        for (uint16_t cy = y; cy < y + height; cy++) {
          for (uint16_t cz = z; cz < z + depth; cz++) {
            mtx.get(cx, cy, cz).enable(f);
          }
        }
      }
    }
    break;
  }
}

void Geo_Page::_erase(
  uint16_t x, 
  uint16_t y, 
  uint16_t z, 
  uint16_t width, 
  uint16_t height, 
  uint16_t depth
) noexcept {
  if (width == 0 || height == 0 || depth == 0) return;

  if (ctx == nullptr) return;

  if (!_allowed()) return;

  auto *level = ctx->get_selected_level();
  if (level == nullptr) return;

  auto &mtx = level->get_geo_matrix();

  if (!mtx.is_in_bounds(x, y, z)) return;

  if ((uint16_t)(x + width) < x || (uint16_t)(y + height) < y || (uint16_t)(z + depth) < z) return;
  if (!mtx.is_in_bounds(x + width, y + height, z + depth)) return;

  switch (_geo_category_index) {
    // Type
    case 0:
    {
      for (uint16_t cx = x; cx < x + width; cx++) {
        for (uint16_t cy = y; cy < y + height; cy++) {
          for (uint16_t cz = z; cz < z + depth; cz++) {
            auto &cell = mtx.get(cx, cy, cz);
  
            if (_erase_all) cell = GeoCell();
            else cell.type = GeoType::air;
          }
        }
      }
    }
    break;

    // Features
    default:
    {
      GeoFeature f;

      switch (_geo_category_index) {
        case 1:
        switch (_geo_index) {
          case 0: f = GeoFeature::vertical_pole; break;
          case 1: f = GeoFeature::horizontal_pole; break;
          case 2: f = GeoFeature::cracked_terrain; break;
        }        
        break;

        case 2:
        switch (_geo_index) {
          case 0: f = GeoFeature::bathive; break;
          case 1: f = GeoFeature::forbid_fly_chains; break;
          case 2: f = GeoFeature::worm_grass; break;
          case 3: f = GeoFeature::waterfall; break;
        } 
        break;

        case 3:
        switch (_geo_index) {
          case 0: f = GeoFeature::shortcut_entrance; break;
          case 1: f = GeoFeature::shortcut_path; break;
          case 2: f = GeoFeature::room_entrance; break;
          case 3: f = GeoFeature::dragon_den; break;
          case 4: f = GeoFeature::wack_a_mole_hole; break;
          case 5: f = GeoFeature::scavenger_hole; break;
          case 6: f = GeoFeature::garbage_worm_hole; break;
        } 
        break;
      }
    
      for (uint16_t cx = x; cx < x + width; cx++) {
        for (uint16_t cy = y; cy < y + height; cy++) {
          for (uint16_t cz = z; cz < z + depth; cz++) {
            auto &cell = mtx.get(cx, cy, cz);
            
            if (_erase_all) cell.type = GeoType::air;
            else if (_erase_all_features) cell.clear_features(); 
            else cell.disable(f);
          }
        }
      }
    }
    break;
  }
}

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

  f3->print("Menu Category Index");
  f3->print(_geo_category_index, true);

  f3->print("Menu Index");
  f3->print(_geo_index, true);

  f3->print("Allowed ");
  f3->print(_allowed(), true);
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
        if (_edit_mode == EDIT_MODE_PLACE) {
          _place(
            _selection_rect.x / 20, 
            _selection_rect.y / 20, 
            ctx->level_layer_, 
            _selection_rect.width / 20, 
            _selection_rect.height / 20
          );

          bool geos_or_features = _geo_category_index == 0 || (_geo_category_index == 1 && _geo_index != 2);

          switch (ctx->level_layer_) {
            case 0: if (!geos_or_features) should_redraw_feature1 = true; else should_redraw1 = true; break;
            case 1: if (!geos_or_features) should_redraw_feature2 = true; else should_redraw2 = true; break;
            case 2: if (!geos_or_features) should_redraw_feature3 = true; else should_redraw3 = true; break;
          }
        } else if (_edit_mode == EDIT_MODE_ERASE) {
          _erase(
            _selection_rect.x / 20, 
            _selection_rect.y / 20, 
            ctx->level_layer_, 
            _selection_rect.width / 20, 
            _selection_rect.height / 20
          );

          bool geos_or_features = _geo_category_index == 0 || (_geo_category_index == 1 && _geo_index != 2);

          switch (ctx->level_layer_) {
            case 0: 
              should_redraw1 = geos_or_features || _erase_all; 
              should_redraw_feature1 = !geos_or_features;
            break;
            
            case 1: 
              should_redraw2 = geos_or_features || _erase_all; 
              should_redraw_feature2 = !geos_or_features; 
            break;
            
            case 2: 
              should_redraw3 = geos_or_features || _erase_all; 
              should_redraw_feature3 = !geos_or_features;

            break;
          }
        }

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

  if (IsKeyPressed(KEY_A)) {
    if (_geo_category_index == 0) {
      if (ctx->get_config()->list_wrap) {
        _geo_category_index = 3;
        _geo_index = 0;
      }
    } else {
      _geo_category_index--;
      _geo_index = 0;
    }
  } else if (IsKeyPressed(KEY_D)) {
    if (_geo_category_index == 3) {
      if (ctx->get_config()->list_wrap) {
        _geo_category_index = 0;
        _geo_index = 0;
      }
    } else {
      _geo_category_index++;
      _geo_index = 0;
    }
  }

  if (IsKeyPressed(KEY_W)) {

    switch (_geo_category_index) {
      case 0:
      if (_geo_index == 0) {
        if (ctx->get_config()->list_wrap) _geo_index = 2;
      } else _geo_index--;
      break;

      case 1:
      if (_geo_index == 0) {
        if (ctx->get_config()->list_wrap) _geo_index = 2;
      } else _geo_index--;
      break;

      case 2:
      if (_geo_index == 0) {
        if (ctx->get_config()->list_wrap) _geo_index = 3;
      } else _geo_index--;
      break;

      case 3:
      if (_geo_index == 0) {
        if (ctx->get_config()->list_wrap) _geo_index = 6;
      } else _geo_index--;
      break;
    }

  } else if (IsKeyPressed(KEY_S)) {
    switch (_geo_category_index) {
      case 0:
      if (_geo_index == 2) {
        if (ctx->get_config()->list_wrap) _geo_index = 0;
      } else _geo_index++;
      break;

      case 1:
      if (_geo_index == 2) {
        if (ctx->get_config()->list_wrap) _geo_index = 0;
      } else _geo_index++;
      break;

      case 2:
      if (_geo_index == 3) {
        if (ctx->get_config()->list_wrap) _geo_index = 0;
      } else _geo_index++;
      break;

      case 3:
      if (_geo_index == 6) {
        if (ctx->get_config()->list_wrap) _geo_index = 0;
      } else _geo_index++;
      break;
    }
  }

  if (IsKeyPressed(KEY_L)) {
    ctx->level_layer_ = (ctx->level_layer_ + 1) % 3;
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

    const auto &mtx = ctx->get_selected_level()->get_const_geo_matrix();

    mr::draw::draw_geo_features_layer(
      mtx,
      ctx->_textures->geometry_editor,
      0, 
      BLACK
    );

    mr::draw::draw_geo_cracked(
      mtx,
      ctx->_textures->geometry_editor,
      0,
      BLACK
    );

    mr::draw::draw_geo_entrances(
      mtx,
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

    const auto &mtx = ctx->get_selected_level()->get_const_geo_matrix();

    mr::draw::draw_geo_features_layer(
      mtx,
      ctx->_textures->geometry_editor,
      1, 
      BLACK
    );

    mr::draw::draw_geo_cracked(
      mtx,
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

    const auto &mtx = ctx->get_selected_level()->get_const_geo_matrix();

    mr::draw::draw_geo_features_layer(
      mtx,
      ctx->_textures->geometry_editor,
      2, 
      BLACK
    );

    mr::draw::draw_geo_cracked(
      mtx,
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

  if (ctx->get_config()->geometry.ruler.visible) {
    const auto *level = ctx->get_selected_level();
    Color color = WHITE;
    color.a = ctx->get_config()->geometry.ruler.opacity;

    mr::draw::draw_ruler(
      _mtx_mouse_pos.x, 
      _mtx_mouse_pos.y, 
      level->get_width(), 
      level->get_height(), 
      color
    );
  }

  if (_is_selecting) {
    DrawRectangleLinesEx(_selection_rect, 2, _edit_mode == EDIT_MODE_PLACE ? WHITE : EDIT_MODE_ERASE ? RED : WHITE);
  } else if (_is_mouse_in_mtx_bounds) {
    DrawRectangleLinesEx(
      Rectangle{ _mtx_mouse_pos.x * 20.0f - 2, _mtx_mouse_pos.y * 20.0f - 2, 20.0f + 4, 20.0f + 4 },
      2,
      _edit_mode == EDIT_MODE_PLACE ? WHITE : EDIT_MODE_ERASE ? RED : WHITE
    );
  }

  if (_is_mouse_in_mtx_bounds) {
    const auto &textures = ctx->_textures->geometry_editor;
    auto color = Color { 255, 255, 255, 70 };

    auto scaled_pos = Vector2 { (_mtx_mouse_pos.x + 1) * 20.0f, (_mtx_mouse_pos.y + 1) * 20.0f };
    const auto size = 40.0f / ctx->get_camera().zoom;

    switch (_geo_category_index) {
      case 0:
      switch (_geo_index) {
        case 0:
        DrawRectangle(scaled_pos.x, scaled_pos.y, size, size, color);
        break;

        case 1:
        DrawTriangle(
          Vector2{scaled_pos.x,        scaled_pos.y       },
          Vector2{scaled_pos.x,        scaled_pos.y + size},
          Vector2{scaled_pos.x + size, scaled_pos.y + size},
          color
        );
        break;

        case 2:
        DrawRectangle(
          scaled_pos.x,
          scaled_pos.y,
          size,
          size/2,
          color
        );
        break;
      }
      break;
  
      case 1:
      switch (_geo_index) {
        case 0:
        DrawRectangle(
          scaled_pos.x + size/7 * 3,
          scaled_pos.y,
          size/7,
          size,
          color
        );
        break;

        case 1:
        DrawRectangle(
          scaled_pos.x,
          scaled_pos.y + size/7 * 3,
          size,
          size/7,
          color
        );
        break;

        case 2:
        DrawTexturePro(
          textures.ui_crack(),
          Rectangle {0, 0, static_cast<float>(textures.ui_crack().width), static_cast<float>(textures.ui_crack().height)},
          Rectangle{
            scaled_pos.x,
            scaled_pos.y,
            size,
            size
          },
          Vector2 {0, 0},
          0,
          color
        );
        break;
      }
      break;
  
      case 2:
      switch (_geo_index) {
        case 0:
        DrawTexturePro(
          textures.bathive(),
          Rectangle {0, 0, static_cast<float>(textures.bathive().width), static_cast<float>(textures.bathive().height)},
          Rectangle {
            scaled_pos.x,
            scaled_pos.y,
            size,
            size
          },
          Vector2 {0, 0},
          0,
          color
        );
        break;

        case 1:
        DrawTexturePro(
          textures.forbid(),
          Rectangle {0, 0, static_cast<float>(textures.forbid().width), static_cast<float>(textures.forbid().height)},
          Rectangle {
            scaled_pos.x,
            scaled_pos.y,
            size,
            size
          },
          Vector2 {0, 0},
          0,
          color
        );
        break;

        case 2:
        DrawTexturePro(
          textures.wormgrass(),
          Rectangle {0, 0, static_cast<float>(textures.wormgrass().width), static_cast<float>(textures.wormgrass().height)},
          Rectangle {
            scaled_pos.x,
            scaled_pos.y,
            size,
            size
          },
          Vector2 {0, 0},
          0,
          color
        );
        break;

        case 3:
        DrawTexturePro(
          textures.waterfall(),
          Rectangle {0, 0, static_cast<float>(textures.waterfall().width), static_cast<float>(textures.waterfall().height)},
          Rectangle {
            scaled_pos.x,
            scaled_pos.y,
            size,
            size
          },
          Vector2 {0, 0},
          0,
          color
        );
        break;
      }
      break;
  
      case 3:
      switch (_geo_index) {
        case 0:
        DrawTexturePro(
          textures.entry_loose(),
          Rectangle {0, 0, static_cast<float>(textures.entry_loose().width), static_cast<float>(textures.entry_loose().height)},
          Rectangle {
            scaled_pos.x,
            scaled_pos.y,
            size,
            size
          },
          Vector2 {0, 0},
          0,
          color
        );
        break;

        case 1:
        DrawTexturePro(
          textures.ui_path(),
          Rectangle {0, 0, static_cast<float>(textures.ui_path().width), static_cast<float>(textures.ui_path().height)},
          Rectangle {
            scaled_pos.x,
            scaled_pos.y,
            size,
            size
          },
          Vector2 {0, 0},
          0,
          color
        );
        break;

        case 2:
        DrawTexturePro(
          textures.passage(),
          Rectangle {0, 0, static_cast<float>(textures.passage().width), static_cast<float>(textures.passage().height)},
          Rectangle {
            scaled_pos.x,
            scaled_pos.y,
            size,
            size
          },
          Vector2 {0, 0},
          0,
          color
        );
        break;

        case 3:
        DrawTexturePro(
          textures.den(),
          Rectangle {0, 0, static_cast<float>(textures.den().width), static_cast<float>(textures.den().height)},
          Rectangle {
            scaled_pos.x,
            scaled_pos.y,
            size,
            size
          },
          Vector2 {0, 0},
          0,
          color
        );
        break;

        case 4:
        DrawTexturePro(
          textures.wack(),
          Rectangle {0, 0, static_cast<float>(textures.wack().width), static_cast<float>(textures.wack().height)},
          Rectangle {
            scaled_pos.x,
            scaled_pos.y,
            size,
            size
          },
          Vector2 {0, 0},
          0,
          color
        );
        break;

        case 5:
        DrawTexturePro(
          textures.scav(),
          Rectangle {0, 0, static_cast<float>(textures.scav().width), static_cast<float>(textures.scav().height)},
          Rectangle {
            scaled_pos.x,
            scaled_pos.y,
            size,
            size
          },
          Vector2 {0, 0},
          0,
          color
        );
        break;

        case 6:
        DrawTexturePro(
          textures.garbageworm(),
          Rectangle {0, 0, static_cast<float>(textures.garbageworm().width), static_cast<float>(textures.garbageworm().height)},
          Rectangle {
            scaled_pos.x,
            scaled_pos.y,
            size,
            size
          },
          Vector2 {0, 0},
          0,
          color
        );
        break;
      }
      break;
    }
  }
  
  EndMode2D();
}

void Geo_Page::windows() noexcept {
  auto menu_opened = ImGui::Begin("Menu##GeoPageMenu");
  _hovering_on_window |= ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows);

  auto available_width = ImGui::GetContentRegionAvail().x / 4 - 4;
  auto *draw_list = ImGui::GetWindowDrawList();

  if (menu_opened) {

    //

    auto cursor_pos = ImGui::GetCursorScreenPos();
    auto mouse_pos = ImGui::GetMousePos();
    auto hovered = mouse_pos.x > cursor_pos.x && 
        mouse_pos.x < cursor_pos.x + available_width &&
        mouse_pos.y > cursor_pos.y &&
        mouse_pos.y < cursor_pos.y + available_width;

    draw_list->AddRectFilled(
      cursor_pos,
      ImVec2{cursor_pos.x + available_width, cursor_pos.y +available_width},
      ImGui::ColorConvertFloat4ToU32(
          _geo_category_index == 0 
              ? ImVec4{0.35f, 0.5f, 0.9f, 0.77f} 
              : (hovered ? ImVec4{1, 1, 1, 0.57f} :  ImVec4{0, 0, 0, 0})
      )
    );
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.0f);
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10.0f);

    auto &textures = ctx->_textures->geometry_editor;

    rlImGuiImageSize(&textures.ui_category_one(), available_width - 20, available_width - 20);

    if (ImGui::GetMouseClickedCount(ImGuiMouseButton_Left) && hovered) { _geo_category_index = 0; }

    //

    ImGui::SetCursorPosY(ImGui::GetCursorPosY() - available_width + 6);
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + available_width + 5);

    cursor_pos = ImGui::GetCursorScreenPos();

    mouse_pos = ImGui::GetMousePos();
    hovered = mouse_pos.x > cursor_pos.x && 
        mouse_pos.x < cursor_pos.x + available_width &&
        mouse_pos.y > cursor_pos.y &&
        mouse_pos.y < cursor_pos.y + available_width;

    draw_list->AddRectFilled(
      cursor_pos,
      ImVec2{cursor_pos.x + available_width, cursor_pos.y +available_width},
      ImGui::ColorConvertFloat4ToU32(
          _geo_category_index == 1 
              ? ImVec4{0.35f, 0.5f, 0.9f, 0.77f} 
              : (hovered ? ImVec4{1, 1, 1, 0.57f} :  ImVec4{0, 0, 0, 0})
      )
    );
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.0f);
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10.0f);

    rlImGuiImageSize(&textures.ui_category_two(), available_width - 20, available_width - 20);

    if (ImGui::GetMouseClickedCount(ImGuiMouseButton_Left) && hovered) { _geo_category_index = 1; }

    //

    ImGui::SetCursorPosY(ImGui::GetCursorPosY() - available_width + 6);
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (available_width + 5)*2);

    cursor_pos = ImGui::GetCursorScreenPos();

    mouse_pos = ImGui::GetMousePos();
    hovered = mouse_pos.x > cursor_pos.x && 
        mouse_pos.x < cursor_pos.x + available_width &&
        mouse_pos.y > cursor_pos.y &&
        mouse_pos.y < cursor_pos.y + available_width;

    draw_list->AddRectFilled(
      cursor_pos,
      ImVec2{cursor_pos.x + available_width, cursor_pos.y +available_width},
      ImGui::ColorConvertFloat4ToU32(
          _geo_category_index == 2 
              ? ImVec4{0.35f, 0.5f, 0.9f, 0.77f} 
              : (hovered ? ImVec4{1, 1, 1, 0.57f} :  ImVec4{0, 0, 0, 0})
      )
    );
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.0f);
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10.0f);

    rlImGuiImageSize(&textures.ui_category_three(), available_width - 20, available_width - 20);

    if (ImGui::GetMouseClickedCount(ImGuiMouseButton_Left) && hovered) { _geo_category_index = 2; }

    //

    ImGui::SetCursorPosY(ImGui::GetCursorPosY() - available_width + 6);
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (available_width + 5)*3);

    cursor_pos = ImGui::GetCursorScreenPos();

    mouse_pos = ImGui::GetMousePos();
    hovered = mouse_pos.x > cursor_pos.x && 
        mouse_pos.x < cursor_pos.x + available_width &&
        mouse_pos.y > cursor_pos.y &&
        mouse_pos.y < cursor_pos.y + available_width;

    draw_list->AddRectFilled(
      cursor_pos,
      ImVec2{cursor_pos.x + available_width, cursor_pos.y +available_width},
      ImGui::ColorConvertFloat4ToU32(
          _geo_category_index == 3
              ? ImVec4{0.35f, 0.5f, 0.9f, 0.77f} 
              : (hovered ? ImVec4{1, 1, 1, 0.57f} :  ImVec4{0, 0, 0, 0})
      )
    );
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.0f);
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10.0f);

    rlImGuiImageSize(&textures.ui_category_four(), available_width - 20, available_width - 20);

    if (ImGui::GetMouseClickedCount(ImGuiMouseButton_Left) && hovered) { _geo_category_index = 3; }

    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20.0f);

    //

    if (ImGui::BeginListBox("##Geos", ImGui::GetContentRegionAvail())) {

      auto selectable_size = ImGui::GetContentRegionAvail();
      selectable_size.y = 40;

      switch (_geo_category_index) {
        case 0:
        {
          {
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 3);
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3);
            
            auto cursor_pos = ImGui::GetCursorScreenPos();
            
            draw_list->AddRectFilled(
              cursor_pos,
              ImVec2{cursor_pos.x + 34, cursor_pos.y + 34},
              ImGui::ColorConvertFloat4ToU32(ImVec4{1, 1, 1, 1})
            );
            
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 3);
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 45);

            if (ImGui::Selectable("Solid", _geo_index == 0, 0, selectable_size)) _geo_index = 0;
          }
          {
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 3);

            rlImGuiImageSize(&textures.ui_category_one(), 34, 34);
            ImGui::SameLine();
    
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 3);

            if (ImGui::Selectable("Slope", _geo_index == 1, 0, selectable_size)) _geo_index = 1;
          }
          {
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 3);

            rlImGuiImageSize(&textures.ui_platform(), 34, 34);
            ImGui::SameLine();
    
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 3);

            if (ImGui::Selectable("Platform", _geo_index == 2, 0, selectable_size)) _geo_index = 2;
          }
        }
        break;

        case 1:
        {
          {
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 3);
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3);
            
            auto cursor_pos = ImGui::GetCursorScreenPos();

            cursor_pos.x += 12;
            
            draw_list->AddRectFilled(
              cursor_pos,
              ImVec2{cursor_pos.x + 6, cursor_pos.y + 34},
              ImGui::ColorConvertFloat4ToU32(ImVec4{1, 1, 1, 1})
            );
            
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 3);
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 45);

            if (ImGui::Selectable("Vertical Pole", _geo_index == 0, 0, selectable_size)) _geo_index = 0;
          }
          {
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 3);
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3);
            
            auto cursor_pos = ImGui::GetCursorScreenPos();

            cursor_pos.y += 12;
            
            draw_list->AddRectFilled(
              cursor_pos,
              ImVec2{cursor_pos.x + 34, cursor_pos.y + 6},
              ImGui::ColorConvertFloat4ToU32(ImVec4{1, 1, 1, 1})
            );
            
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 3);
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 45);

            if (ImGui::Selectable("Horizontal Pole", _geo_index == 1, 0, selectable_size)) _geo_index = 1;
          }
          {
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 3);

            rlImGuiImageSize(&textures.ui_crack(), 34, 34);
            ImGui::SameLine();
    
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 3);

            if (ImGui::Selectable("Crack", _geo_index == 2, 0, selectable_size)) _geo_index = 2;
          }
        }
        break;

        case 2:
        {
          {
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 3);

            rlImGuiImageSize(&textures.bathive(), 34, 34);
            ImGui::SameLine();
    
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 3);

            if (ImGui::Selectable("Bathive", _geo_index == 0, 0, selectable_size)) _geo_index = 0;
          }
          {
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 3);

            rlImGuiImageSize(&textures.forbid(), 34, 34);
            ImGui::SameLine();
    
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 3);

            if (ImGui::Selectable("Forbid Fly Chains", _geo_index == 1, 0, selectable_size)) _geo_index = 1;
          }
          {
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 3);

            rlImGuiImageSize(&textures.wormgrass(), 34, 34);
            ImGui::SameLine();
    
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 3);

            if (ImGui::Selectable("Worm Grass", _geo_index == 2, 0, selectable_size)) _geo_index = 2;
          }
          {
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 3);

            rlImGuiImageSize(&textures.waterfall(), 34, 34);
            ImGui::SameLine();
    
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 3);

            if (ImGui::Selectable("Waterfall", _geo_index == 3, 0, selectable_size)) _geo_index = 3;
          }
        }
        break;

        case 3:
        {
          {
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 3);

            rlImGuiImageSize(&textures.entry_loose(), 34, 34);
            ImGui::SameLine();
    
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 3);

            if (ImGui::Selectable("Shortcut Entry", _geo_index == 0, 0, selectable_size)) _geo_index = 0;
          }
          {
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 3);

            rlImGuiImageSize(&textures.ui_path(), 34, 34);
            ImGui::SameLine();
    
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 3);

            if (ImGui::Selectable("Shortcut Path", _geo_index == 1, 0, selectable_size)) _geo_index = 1;
          }
          {
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 3);

            rlImGuiImageSize(&textures.passage(), 34, 34);
            ImGui::SameLine();
    
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 3);

            if (ImGui::Selectable("Passage", _geo_index == 2, 0, selectable_size)) _geo_index = 2;
          }
          {
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 3);

            rlImGuiImageSize(&textures.den(), 34, 34);
            ImGui::SameLine();
    
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 3);

            if (ImGui::Selectable("Dragon Den", _geo_index == 3, 0, selectable_size)) _geo_index = 3;
          }
          {
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 3);

            rlImGuiImageSize(&textures.wack(), 34, 34);
            ImGui::SameLine();
    
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 3);

            if (ImGui::Selectable("Wach-a-mole Hole", _geo_index == 4, 0, selectable_size)) _geo_index = 4;
          }
          {
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 3);

            rlImGuiImageSize(&textures.scav(), 34, 34);
            ImGui::SameLine();
    
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 3);

            if (ImGui::Selectable("Scavenger", _geo_index == 5, 0, selectable_size)) _geo_index = 5;
          }
          {
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 3);

            rlImGuiImageSize(&textures.garbageworm(), 34, 34);
            ImGui::SameLine();
    
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 3);

            if (ImGui::Selectable("Garbage Worm", _geo_index == 6, 0, selectable_size)) _geo_index = 6;
          }
        }
        break;
      }

      ImGui::EndListBox();
    }
  }

  ImGui::End();
}

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
  _geo_category_index(0),
  _geo_index(0),
  _edit_mode(EDIT_MODE_PLACE),
  _is_selecting(false),
  _erase_all(false),
  _erase_all_features(false),
  _selection_rect(Rectangle{0, 0, 0, 0}),
  _selection_origin(Vector2{0, 0})
{}

Geo_Page::~Geo_Page() {}

};