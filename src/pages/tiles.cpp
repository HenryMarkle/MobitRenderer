#include <cstdint>
#include <memory>

#ifdef IS_DEBUG_BUILD
#include <iostream>
#endif

#include <imgui.h>
#include <raylib.h>
#include <raymath.h>
#include <rlImGui.h>

#include <MobitRenderer/draw.h>
#include <MobitRenderer/matrix.h>
#include <MobitRenderer/pages.h>
#include <MobitRenderer/sdraw.h>
#include <MobitRenderer/utils.h>

#define EDIT_MODE_MATERIAL 0
#define EDIT_MODE_TILE 1

#define FORCE_MODE_PERMISSIVE 0
#define FORCE_MODE_WITH_GEO 1
#define FORCE_MODE_WITHOUT_GEO 2

namespace mr::pages {

void Tile_Page::_redraw_tile_preview_rt() noexcept {
  if (_hovered_tile == _previously_drawn_preview)
    return;

  _previously_drawn_preview = _hovered_tile;

  mr::utils::unload_rendertexture(_tile_preview_rt);
  if (_hovered_tile == nullptr)
    return;

  _tile_preview_rt = LoadRenderTexture(_hovered_tile->get_width() * 16,
                                       _hovered_tile->get_height() * 16);

  BeginTextureMode(_tile_preview_rt);
  {
    ClearBackground(Color{0, 0, 0, 0});

    const auto &shader = ctx->_shaders->white_remover_apply_color();
    const auto &texture = _hovered_tile->get_loaded_texture();
    BeginShaderMode(shader);
    {
      SetShaderValueTexture(shader, GetShaderLocation(shader, "texture0"),
                            texture);
      mr::draw::draw_tile_prev(_hovered_tile, 0, 0, 16, WHITE);
    }
    EndShaderMode();
  }
  EndTextureMode();
}
void Tile_Page::_redraw_tile_texture_rt() noexcept {
  if (_selected_tile == _previously_drawn_texture)
    return;

  _previously_drawn_texture = _selected_tile;

  mr::utils::unload_rendertexture(_tile_texture_rt);

  if (_selected_tile == nullptr)
    return;

  _tile_texture_rt = LoadRenderTexture(_selected_tile->calculate_width(20),
                                       _selected_tile->calculate_height(20));

  BeginTextureMode(_tile_texture_rt);
  {
    ClearBackground(Color{0, 0, 0, 0});

    const auto &texture = _selected_tile->get_loaded_texture();

    if (_selected_tile->get_type() == TileDefType::box) {
      const auto &shader = ctx->_shaders->white_remover_rgb_recolor();

      BeginShaderMode(shader);
      {
        SetShaderValueTexture(shader, GetShaderLocation(shader, "texture0"),
                              texture);

        DrawTexturePro(texture,
                       Rectangle{0,
                                 1.0f * _selected_tile->get_width() *
                                     _selected_tile->get_height() * 20,
                                 1.0f * _selected_tile->calculate_width(20),
                                 1.0f * _selected_tile->calculate_height(20)},
                       Rectangle{0, 0,
                                 1.0f * _selected_tile->calculate_width(20),
                                 1.0f * _selected_tile->calculate_height(20)},
                       Vector2{0, 0}, 0, _selected_tile->get_color());
      }
      EndShaderMode();
    } else {
      const auto &shader = ctx->_shaders->voxel_struct_tinted();
      BeginShaderMode(shader);
      {
        SetShaderValueTexture(shader, GetShaderLocation(shader, "texture0"),
                              texture);

        auto layers = _selected_tile->get_repeat().size();
        SetShaderValue(shader, GetShaderLocation(shader, "layers"), &layers,
                       SHADER_UNIFORM_INT);

        float height = _selected_tile->calculate_height(20) * 1.0f /
                       _selected_tile->get_texture().height;
        SetShaderValue(shader, GetShaderLocation(shader, "height"), &height,
                       SHADER_UNIFORM_FLOAT);

        float width = _selected_tile->calculate_width(20) * 1.0f /
                      _selected_tile->get_texture().width;
        SetShaderValue(shader, GetShaderLocation(shader, "width"), &width,
                       SHADER_UNIFORM_FLOAT);

        auto depth = -(0.8f / layers);
        SetShaderValue(shader, GetShaderLocation(shader, "depth"), &depth,
                       SHADER_UNIFORM_FLOAT);

        int offset = 0;
        SetShaderValue(shader, GetShaderLocation(shader, "depthOffset"),
                       &offset, SHADER_UNIFORM_INT);

        DrawTexturePro(
            texture,
            Rectangle{0, 0, (float)_selected_tile->get_texture().width,
                      (float)_selected_tile->get_texture().height},
            Rectangle{0, 0, (float)_selected_tile->calculate_width(20),
                      (float)_selected_tile->calculate_height(20)},
            Vector2{0, 0}, 0, _selected_tile->get_color());

        // draw_tile_tinted(_selected_tile, 0, 0, 20,
        // ctx->_tiledex->colors().at(_selected_tile->get_category()));
      }
      EndShaderMode();
    }
  }
  EndTextureMode();
}
void Tile_Page::_redraw_tile_specs_rt() noexcept {
  if (_selected_tile == _previously_drawn_specs)
    return;

  _previously_drawn_specs = _selected_tile;

  mr::utils::unload_rendertexture(_tile_specs_rt);

  if (_selected_tile == nullptr)
    return;

  _tile_specs_rt = LoadRenderTexture(_selected_tile->get_width() * 10,
                                     _selected_tile->get_height() * 10);

  BeginTextureMode(_tile_specs_rt);
  {
    for (size_t x = 0; x < _selected_tile->get_width(); x++) {
      for (size_t y = 0; y < _selected_tile->get_height(); y++) {
        auto index = y + x * _selected_tile->get_height();
        auto spec = _selected_tile->get_specs()[index];

        if (spec == 1)
          DrawRectangleLinesEx(Rectangle{x * 10.0f, y * 10.0f, 10.0f, 10.0f}, 1,
                               WHITE);

        if (spec == 0) {
          DrawLineEx(Vector2{x * 10.0f, y * 10.0f},
                     Vector2{(x + 1) * 10.0f, (y + 1) * 10.0f}, 1, WHITE);
          DrawLineEx(Vector2{x * 10.0f, (y + 1) * 10.0f},
                     Vector2{(x + 1) * 10.0f, y * 10.0f}, 1, WHITE);
        }
      }
    }

    // specs 2

    if (_selected_tile->get_specs2().size() > 0) {
      for (int8_t x = 0; x < _selected_tile->get_width(); x++) {
        for (int8_t y = 0; y < _selected_tile->get_height(); y++) {
          auto spec = _selected_tile
                          ->get_specs2()[y + x * _selected_tile->get_height()];

          if (spec < 0)
            continue;

          if (spec == 1)
            DrawRectangleLinesEx(
                Rectangle{x * 10.0f + 2, y * 10.0f + 2, 6.0f, 6.0f}, 1, GREEN);

          if (spec == 0) {
            DrawLineEx(Vector2{x * 10.0f + 2, y * 10.0f + 2},
                       Vector2{(x + 1) * 10.0f - 2, (y + 1) * 10.0f - 2}, 1,
                       WHITE);
            DrawLineEx(Vector2{x * 10.0f + 2, (y + 1) * 10.0f + 2},
                       Vector2{(x + 1) * 10.0f - 2, y * 10.0f - 2}, 1, WHITE);
          }
        }
      }
    }
  }
  EndTextureMode();
}

void Tile_Page::process() {
  if (ctx == nullptr)
    return;

  auto wheel = GetMouseWheelMove();

  _update_mtx_mouse_pos();
  _update_arrows_mtx_camera_pos();

  if (!_hovering_on_window) {
    if (wheel != 0) {

      auto &camera = ctx->get_camera();
      auto mouseWorldPosition = GetScreenToWorld2D(GetMousePosition(), camera);

      camera.offset = GetMousePosition();
      camera.target = mouseWorldPosition;
      camera.zoom += wheel * 0.125f;
      if (camera.zoom < 0.125f)
        camera.zoom = 0.125f;
    }

    if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)) {
      auto delta = GetMouseDelta();
      auto &camera = ctx->get_camera();

      delta = Vector2Scale(delta, -1.0f / camera.zoom);

      camera.target.x += delta.x;
      camera.target.y += delta.y;
    }

    if (IsKeyPressed(KEY_L)) {
      ctx->level_layer_ = (ctx->level_layer_ + 1) % 3;
      _should_redraw = true;
    }

    if (IsKeyPressed(KEY_M)) {
      if (_edit_mode == EDIT_MODE_TILE)
        _edit_mode = EDIT_MODE_MATERIAL;
      else
        _edit_mode = EDIT_MODE_TILE;
    }

    if (_is_mouse_in_mtx_bounds) {

      if (IsKeyDown(KEY_G)) {
        _force_mode = FORCE_MODE_WITH_GEO;
      } else if (IsKeyDown(KEY_F)) {
        _force_mode = FORCE_MODE_WITHOUT_GEO;
      } else {
        _force_mode = FORCE_MODE_PERMISSIVE;
      }

      // Pickup item
      if (IsKeyPressed(KEY_Q)) {
        const auto *cell =
            ctx->get_selected_level()->get_const_tile_matrix().get_const_ptr(
                _mtx_mouse_pos.x, _mtx_mouse_pos.y, ctx->level_layer_);

        if (cell == nullptr)
          goto break_lookup;

        switch (cell->type) {
        case TileType::head: {
          _selected_tile = cell->tile_def;

          if (_selected_tile == nullptr)
            goto break_lookup;

          _redraw_tile_texture_rt();
          _redraw_tile_specs_rt();

          const auto &categories = ctx->_tiledex->sorted_tiles();
          for (size_t c = 0; c < categories.size(); c++) {
            const auto &tiles = categories[c];
            for (size_t t = 0; t < tiles.size(); t++) {
              if (tiles[t]->get_name() == _selected_tile->get_name()) {
                _selected_tile_category_index = c;
                _selected_tile_index = t;
                goto break_lookup;
              }
            }
          }
        } break;

        case TileType::body: {
          const auto *supposed_head =
              ctx->get_selected_level()->get_const_tile_matrix().get_const_ptr(
                  cell->head_pos_x, cell->head_pos_y, cell->head_pos_z);

          if (supposed_head == nullptr || supposed_head->type != TileType::head)
            goto break_lookup;

          _selected_tile = supposed_head->tile_def;

          _redraw_tile_texture_rt();
          _redraw_tile_specs_rt();

          const auto &categories = ctx->_tiledex->sorted_tiles();
          for (size_t c = 0; c < categories.size(); c++) {
            const auto &tiles = categories[c];
            for (size_t t = 0; t < tiles.size(); t++) {
              if (tiles[t]->get_name() == _selected_tile->get_name()) {
                _selected_tile_category_index = c;
                _selected_tile_index = t;
                goto break_lookup;
              }
            }
          }
        } break;

        case TileType::material: {
          _selected_material = cell->material_def;
          if (_selected_material == nullptr)
            goto break_lookup;

          const auto &categories = ctx->_materialdex->sorted_materials();
          for (size_t c = 0; c < categories.size(); c++) {
            const auto &materials = categories[c];
            for (size_t m = 0; m < materials.size(); m++) {
              if (materials[m]->get_name() == _selected_tile->get_name()) {
                _selected_material_category_index = c;
                _selected_material_index = m;
                goto break_lookup;
              }
            }
          }
        } break;

        case TileType::_default: {
          const auto &name = ctx->get_selected_level()->default_material;

          /// TODO: Inefficient lookup method.

          const auto &categories = ctx->_materialdex->sorted_materials();
          for (size_t c = 0; c < categories.size(); c++) {
            const auto &materials = categories[c];
            for (size_t m = 0; m < materials.size(); m++) {
              if (materials[m]->get_name() == name) {
                _selected_material_category_index = c;
                _selected_material_index = m;
                _selected_material = materials[m];
                goto break_lookup;
              }
            }
          }
        } break;
        }
      break_lookup: {}
      }
    }
  }

  if (IsKeyPressed(KEY_W)) {
    if (_edit_mode == EDIT_MODE_MATERIAL) {
      if (_selected_material_index == 0) {
        if (ctx->get_config()->list_wrap) {
          _selected_material_index =
              ctx->_materialdex
                  ->sorted_materials()[_selected_material_category_index]
                  .size() -
              1;
        }
      } else
        _selected_material_index--;
    } else if (_edit_mode == EDIT_MODE_TILE) {
      if (_selected_tile_index == 0) {
        if (ctx->get_config()->list_wrap) {
          _selected_tile_index =
              ctx->_tiledex->sorted_tiles()[_selected_tile_category_index]
                  .size() -
              1;
        }
      } else
        _selected_tile_index--;
    }
  } else if (IsKeyPressed(KEY_S)) {
    if (_edit_mode == EDIT_MODE_MATERIAL) {
      if (_selected_material_index ==
          ctx->_materialdex
                  ->sorted_materials()[_selected_material_category_index]
                  .size() -
              1) {
        if (ctx->get_config()->list_wrap) {
          _selected_material_index = 0;
        }
      } else
        _selected_material_index++;
    } else if (_edit_mode == EDIT_MODE_TILE) {
      if (_selected_tile_index ==
          ctx->_tiledex->sorted_tiles()[_selected_tile_category_index].size() -
              1) {
        if (ctx->get_config()->list_wrap) {
          _selected_tile_index = 0;
        }
      } else
        _selected_tile_index++;
    }
  }

  if (IsKeyPressed(KEY_A)) {
    if (_edit_mode == EDIT_MODE_TILE) {
      if (_selected_tile_category_index == 0) {
        if (ctx->get_config()->list_wrap) {
          _selected_tile_category_index =
              ctx->_tiledex->sorted_tiles().size() - 1;
          _selected_material_index = 0;
        }
      } else {
        _selected_tile_category_index--;
        _selected_material_index = 0;
      }
    } else if (_edit_mode == EDIT_MODE_MATERIAL) {
      if (_selected_material_category_index == 0) {
        if (ctx->get_config()->list_wrap) {
          _selected_material_category_index =
              ctx->_materialdex->sorted_materials().size() - 1;
          _selected_material_index = 0;
        }
      } else {
        _selected_material_category_index--;
        _selected_material_index = 0;
      }
    }
  } else if (IsKeyPressed(KEY_D)) {
    if (_edit_mode == EDIT_MODE_TILE) {
      if (_selected_tile_category_index ==
          ctx->_tiledex->sorted_tiles().size() - 1) {
        if (ctx->get_config()->list_wrap) {
          _selected_tile_category_index = 0;
          _selected_tile_index = 0;
        }
      } else {
        _selected_tile_category_index++;
        _selected_tile_index = 0;
      }
    } else if (_edit_mode == EDIT_MODE_MATERIAL) {
      if (_selected_material_category_index ==
          ctx->_materialdex->sorted_materials().size() - 1) {
        if (ctx->get_config()->list_wrap) {
          _selected_material_category_index = 0;
          _selected_material_index = 0;
        }
      } else {
        _selected_material_category_index++;
        _selected_material_index = 0;
      }
    }
  }

  auto *level = ctx->get_selected_level();

  if (level != nullptr) {
    if (_is_mouse_in_mtx_bounds) {
      auto *cell = level->get_tile_matrix().get_ptr(
          _mtx_mouse_pos.x, _mtx_mouse_pos.y, ctx->level_layer_);

      _hovered_cell = cell;
    }
  }

  _hovering_on_window = false;
}

void Tile_Page::draw() noexcept {
  if (ctx == nullptr)
    return;

  const auto *level = ctx->get_selected_level();

  if (level == nullptr)
    return;

  ClearBackground(DARKGRAY);

  auto &camera = ctx->get_camera();
  const auto &viewport = ctx->_textures->get_main_level_viewport();

  if (_should_redraw1) {
    BeginTextureMode(ctx->_textures->geo_layer1.get());
    ClearBackground(WHITE);

    mr::draw::draw_geo_and_poles_layer(
        ctx->get_selected_level()->get_const_geo_matrix(), 0, BLACK);
    EndTextureMode();

    BeginTextureMode(ctx->_textures->feature_layer1.get());
    ClearBackground(WHITE);

    mr::draw::draw_geo_features_layer(
        ctx->get_selected_level()->get_const_geo_matrix(),
        ctx->_textures->geometry_editor, 0, BLACK);

    mr::draw::draw_geo_entrances(
        ctx->get_selected_level()->get_const_geo_matrix(),
        ctx->_textures->geometry_editor, BLACK);
    EndTextureMode();

    _should_redraw1 = false;
    _should_redraw = true;
  }

  if (_should_redraw2) {
    BeginTextureMode(ctx->_textures->geo_layer2.get());

    ClearBackground(WHITE);

    mr::draw::draw_geo_and_poles_layer(
        ctx->get_selected_level()->get_const_geo_matrix(), 1, BLACK);

    EndTextureMode();

    _should_redraw2 = false;
    _should_redraw = true;
  }

  if (_should_redraw3) {
    BeginTextureMode(ctx->_textures->geo_layer3.get());

    ClearBackground(WHITE);

    mr::draw::draw_geo_and_poles_layer(
        ctx->get_selected_level()->get_const_geo_matrix(), 2, BLACK);

    EndTextureMode();

    _should_redraw3 = false;
    _should_redraw = true;
  }

  if (_should_redraw_tile1) {
    BeginTextureMode(ctx->_textures->tile_layer1.get());

    ClearBackground(WHITE);

    mr::sdraw::draw_tile_prevs_layer(ctx->_shaders,
                                     level->get_const_geo_matrix(),
                                     level->get_const_tile_matrix(), 0, 20);

    EndTextureMode();

    _should_redraw_tile1 = false;
    _should_redraw = true;
  }

  if (_should_redraw_tile2) {
    BeginTextureMode(ctx->_textures->tile_layer2.get());

    ClearBackground(WHITE);

    mr::sdraw::draw_tile_prevs_layer(ctx->_shaders,
                                     level->get_const_geo_matrix(),
                                     level->get_const_tile_matrix(), 1, 20);

    EndTextureMode();

    _should_redraw_tile2 = false;
    _should_redraw = true;
  }

  if (_should_redraw_tile3) {
    BeginTextureMode(ctx->_textures->tile_layer3.get());

    ClearBackground(WHITE);

    mr::sdraw::draw_tile_prevs_layer(ctx->_shaders,
                                     level->get_const_geo_matrix(),
                                     level->get_const_tile_matrix(), 2, 20);

    EndTextureMode();

    _should_redraw_tile3 = false;
    _should_redraw = true;
  }

  if (_should_redraw) {
    BeginTextureMode(ctx->_textures->get_main_level_viewport());
    ClearBackground(Color{200, 200, 200, 255});
    const auto layer = ctx->level_layer_;
    {
      // Background

      if (layer != 2) {
        const auto &color_shader = ctx->_shaders->white_remover_apply_color();
        BeginShaderMode(color_shader);
        {
          SetShaderValueTexture(color_shader,
                                GetShaderLocation(color_shader, "texture0"),
                                ctx->_textures->geo_layer3.get().texture);

          DrawTexture(ctx->_textures->geo_layer3.get().texture, 0, 0,
                      Color{50, 50, 50, 255});
        }
        EndShaderMode();

        const auto &bkg_shader = ctx->_shaders->white_remover_apply_alpha();
        BeginShaderMode(bkg_shader);
        {
          SetShaderValueTexture(bkg_shader,
                                GetShaderLocation(bkg_shader, "texture0"),
                                ctx->_textures->tile_layer3.get().texture);

          int alpha = 200;

          SetShaderValue(bkg_shader, GetShaderLocation(bkg_shader, "alpha"),
                         &alpha, SHADER_UNIFORM_INT);

          DrawTexture(ctx->_textures->tile_layer3.get().texture, 0, 0, WHITE);
        }
        EndShaderMode();
      }

      if (layer != 1) {
        const auto &color_shader = ctx->_shaders->white_remover_apply_color();
        BeginShaderMode(color_shader);
        {
          SetShaderValueTexture(color_shader,
                                GetShaderLocation(color_shader, "texture0"),
                                ctx->_textures->geo_layer2.get().texture);

          DrawTexture(ctx->_textures->geo_layer2.get().texture, 0, 0,
                      Color{20, 20, 20, 200});
        }
        EndShaderMode();

        const auto &bkg_shader = ctx->_shaders->white_remover_apply_alpha();
        BeginShaderMode(bkg_shader);
        {
          SetShaderValueTexture(bkg_shader,
                                GetShaderLocation(bkg_shader, "texture0"),
                                ctx->_textures->tile_layer2.get().texture);

          int alpha = 200;

          SetShaderValue(bkg_shader, GetShaderLocation(bkg_shader, "alpha"),
                         &alpha, SHADER_UNIFORM_INT);

          DrawTexture(ctx->_textures->tile_layer2.get().texture, 0, 0, WHITE);
        }
        EndShaderMode();
      }

      if (layer != 0) {
        const auto &color_shader = ctx->_shaders->white_remover_apply_color();
        BeginShaderMode(color_shader);
        {
          SetShaderValueTexture(color_shader,
                                GetShaderLocation(color_shader, "texture0"),
                                ctx->_textures->geo_layer1.get().texture);

          DrawTexture(ctx->_textures->geo_layer1.get().texture, 0, 0,
                      Color{0, 0, 0, 230});
        }
        EndShaderMode();

        const auto &bkg_shader = ctx->_shaders->white_remover_apply_alpha();
        BeginShaderMode(bkg_shader);
        {
          SetShaderValueTexture(bkg_shader,
                                GetShaderLocation(bkg_shader, "texture0"),
                                ctx->_textures->tile_layer1.get().texture);

          int alpha = 230;

          SetShaderValue(bkg_shader, GetShaderLocation(bkg_shader, "alpha"),
                         &alpha, SHADER_UNIFORM_INT);

          DrawTexture(ctx->_textures->tile_layer1.get().texture, 0, 0, WHITE);
        }
        EndShaderMode();
      }

      // Foreground

      if (layer == 2) {
        const auto &color_shader = ctx->_shaders->white_remover_apply_color();
        BeginShaderMode(color_shader);
        {
          SetShaderValueTexture(color_shader,
                                GetShaderLocation(color_shader, "texture0"),
                                ctx->_textures->geo_layer3.get().texture);

          DrawTexture(ctx->_textures->geo_layer3.get().texture, 0, 0,
                      Color{0, 0, 0, 220});
        }
        EndShaderMode();

        const auto &bkg_shader = ctx->_shaders->white_remover();
        BeginShaderMode(bkg_shader);
        {
          SetShaderValueTexture(color_shader,
                                GetShaderLocation(color_shader, "texture0"),
                                ctx->_textures->tile_layer3.get().texture);

          DrawTexture(ctx->_textures->tile_layer3.get().texture, 0, 0, WHITE);
        }
        EndShaderMode();
      }

      if (layer == 1) {
        const auto &color_shader = ctx->_shaders->white_remover_apply_color();
        BeginShaderMode(color_shader);
        {
          SetShaderValueTexture(color_shader,
                                GetShaderLocation(color_shader, "texture0"),
                                ctx->_textures->geo_layer2.get().texture);

          DrawTexture(ctx->_textures->geo_layer2.get().texture, 0, 0,
                      Color{0, 0, 0, 220});
        }
        EndShaderMode();

        const auto &bkg_shader = ctx->_shaders->white_remover();
        BeginShaderMode(bkg_shader);
        {
          SetShaderValueTexture(color_shader,
                                GetShaderLocation(color_shader, "texture0"),
                                ctx->_textures->tile_layer2.get().texture);

          DrawTexture(ctx->_textures->tile_layer2.get().texture, 0, 0, WHITE);
        }
        EndShaderMode();
      }

      if (layer == 0) {
        const auto &color_shader = ctx->_shaders->white_remover_apply_color();
        BeginShaderMode(color_shader);
        {
          SetShaderValueTexture(color_shader,
                                GetShaderLocation(color_shader, "texture0"),
                                ctx->_textures->geo_layer1.get().texture);

          DrawTexture(ctx->_textures->geo_layer1.get().texture, 0, 0,
                      Color{0, 0, 0, 220});
        }
        EndShaderMode();

        const auto &bkg_shader = ctx->_shaders->white_remover();
        BeginShaderMode(bkg_shader);
        {
          SetShaderValueTexture(color_shader,
                                GetShaderLocation(color_shader, "texture0"),
                                ctx->_textures->tile_layer1.get().texture);

          DrawTexture(ctx->_textures->tile_layer1.get().texture, 0, 0, WHITE);
        }
        EndShaderMode();

        BeginShaderMode(color_shader);
        {
          SetShaderValueTexture(color_shader,
                                GetShaderLocation(color_shader, "texture0"),
                                ctx->_textures->feature_layer1.get().texture);

          DrawTexture(ctx->_textures->feature_layer1.get().texture, 0, 0,
                      WHITE);
        }
        EndShaderMode();
      }
    }
    EndTextureMode();

    _should_redraw = false;
  }

  // Over viewport

  BeginMode2D(camera);
  {
    DrawTexture(viewport.texture, 0, 0, WHITE);

    mr::draw::draw_double_frame(level->get_pixel_width(),
                                level->get_pixel_height());

    const auto &features_border = level->buffer_geos;

    DrawRectangleLinesEx(
        Rectangle{
            features_border.left * 20.0f,
            features_border.top * 20.0f,
            (level->get_width() - features_border.right -
             features_border.left) *
                20.0f,
            (level->get_height() - features_border.bottom -
             features_border.top) *
                20.0f,
        },
        4, WHITE);

    switch (_edit_mode) {
    case EDIT_MODE_MATERIAL:
      if (_hovered_cell != nullptr) {
        switch (_hovered_cell->type) {
        case TileType::head: {
          if (_hovered_cell->tile_def != nullptr) {
            auto offset = _hovered_cell->tile_def->get_head_offset();
            DrawRectangleLinesEx(
                Rectangle{(_mtx_mouse_pos.x - offset.x) * 20.0f,
                          (_mtx_mouse_pos.y - offset.y) * 20.0f,
                          _hovered_cell->tile_def->get_width() * 20.0f,
                          _hovered_cell->tile_def->get_height() * 20.0f},
                2, WHITE);
          }
        } break;

        case TileType::body: {
          if (_hovered_cell->tile_def != nullptr) {
            auto offset = _hovered_cell->tile_def->get_head_offset();
            DrawRectangleLinesEx(
                Rectangle{(_hovered_cell->head_pos_x - offset.x) * 20.0f,
                          (_hovered_cell->head_pos_y - offset.y) * 20.0f,
                          _hovered_cell->tile_def->get_width() * 20.0f,
                          _hovered_cell->tile_def->get_height() * 20.0f},
                2, WHITE);
          }
        } break;

        default: {
          DrawRectangleLinesEx(Rectangle{_mtx_mouse_pos.x * 20.0f,
                                         _mtx_mouse_pos.y * 20.0f, 20.0f,
                                         20.0f},
                               2, WHITE);
        } break;
        }
      }
      break;

    case EDIT_MODE_TILE:
      if (_selected_tile != nullptr) {
        const auto &shader = ctx->_shaders->white_remover_apply_color();
        const auto &texture = _selected_tile->get_loaded_texture();
        auto origin = _selected_tile->get_head_offset();
        BeginShaderMode(shader);
        {
          SetShaderValueTexture(shader, GetShaderLocation(shader, "texture0"),
                                texture);
          mr::draw::draw_tile_prev(
              _selected_tile, (_mtx_mouse_pos.x - origin.x) * 20,
              (_mtx_mouse_pos.y - origin.y) * 20, 20,
              _force_mode != FORCE_MODE_PERMISSIVE || _is_tile_legal
                  ? _selected_tile->get_color()
                  : RED);
        }
        EndShaderMode();
      }
      break;
    }
  }
  EndMode2D();
}
void Tile_Page::windows() noexcept {
  auto tiles_opened = ImGui::Begin("Tiles##TilesPageTiles");
  _hovering_on_window |= ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows);

  auto button_space = ImGui::GetContentRegionAvail();
  button_space.y = 20;
  if (ImGui::Button(_edit_mode == EDIT_MODE_MATERIAL ? "Switch to tiles"
                                                     : "Switch to materials",
                    button_space)) {
    if (_edit_mode == EDIT_MODE_MATERIAL)
      _edit_mode = EDIT_MODE_TILE;
    else
      _edit_mode = EDIT_MODE_MATERIAL;
  }

  ImGui::Spacing();

  switch (_edit_mode) {
  case EDIT_MODE_TILE: {
    auto *dex = ctx->_tiledex;
    if (tiles_opened && dex != nullptr) {
      ImGui::Columns(2);

      auto *draw_list = ImGui::GetWindowDrawList();
      auto text_height = ImGui::GetTextLineHeight();

      // Tile Categories
      if (ImGui::BeginListBox("##Categories", ImGui::GetContentRegionAvail()) &&
          !dex->categories().empty()) {
        for (size_t c = 0; c < dex->categories().size(); c++) {
          const auto &category = dex->categories().at(c);

          auto cursor_pos = ImGui::GetCursorScreenPos();
          draw_list->AddRectFilled(
              cursor_pos,
              ImVec2{cursor_pos.x + 10.0f, cursor_pos.y + text_height},
              ImGui::ColorConvertFloat4ToU32(
                  ImVec4{category.color.r / 255.0f, category.color.g / 255.0f,
                         category.color.b / 255.0f, 1}));

          ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 16.0f);

          bool selected = ImGui::Selectable(category.name.c_str(),
                                            _selected_tile_category_index == c);
          if (selected) {
            _selected_tile_category_index = c;
            _selected_tile_index = 0;
            if (!dex->sorted_tiles()[c].empty())
              _selected_tile = dex->sorted_tiles()[c][0];
            _redraw_tile_texture_rt();
            _redraw_tile_specs_rt();
          }
        }

        ImGui::EndListBox();
      }

      ImGui::NextColumn();

      // Tiles
      if (ImGui::BeginListBox("##CategoryTiles",
                              ImGui::GetContentRegionAvail()) &&
          !dex->sorted_tiles()[_selected_tile_category_index].empty() &&
          _selected_tile_category_index < dex->sorted_tiles().size()) {
        const auto &category_tiles =
            dex->sorted_tiles()[_selected_tile_category_index];

        for (size_t t = 0; t < category_tiles.size(); t++) {
          auto *tiledef = category_tiles[t];

          bool selected = ImGui::Selectable(tiledef->get_name().c_str(),
                                            t == _selected_tile_index);
          if (selected) {
            _selected_tile_index = t;
            _selected_tile = tiledef;
            _redraw_tile_texture_rt();
            _redraw_tile_specs_rt();
          }

          if (_selected_tile == nullptr) {
            _selected_tile = tiledef;
            _redraw_tile_texture_rt();
            _redraw_tile_specs_rt();
          }

          if (ImGui::IsItemHovered()) {
            _hovered_tile = tiledef;
            _redraw_tile_preview_rt();

            ImGui::BeginTooltip();
            rlImGuiImageRenderTexture(&_tile_preview_rt);
            ImGui::EndTooltip();
          }
        }

        ImGui::EndListBox();
      }
    }
  } break;

  case EDIT_MODE_MATERIAL: {
    auto *mdex = ctx->_materialdex;
    if (tiles_opened && mdex != nullptr) {
      ImGui::Columns(2);

      auto *draw_list = ImGui::GetWindowDrawList();
      auto text_height = ImGui::GetTextLineHeight();

      if (ImGui::BeginListBox("##Categories", ImGui::GetContentRegionAvail()) &&
          !mdex->categories().empty()) {
        for (size_t c = 0; c < mdex->categories().size(); c++) {
          const auto &name = mdex->categories()[c];

          if (ImGui::Selectable((name + "##" + std::to_string(c)).c_str(),
                                _selected_material_category_index == c)) {
            _selected_material_category_index = c;
            _selected_material_index = 0;
            if (!mdex->sorted_materials()[c].empty())
              _selected_material = mdex->sorted_materials()[c][0];
          }
        }

        ImGui::EndListBox();
      }

      ImGui::NextColumn();

      if (ImGui::BeginListBox("##Materials", ImGui::GetContentRegionAvail()) &&
          !mdex->sorted_materials().empty() &&
          _selected_material_category_index < mdex->sorted_materials().size()) {

        const auto &materials =
            mdex->sorted_materials()[_selected_material_category_index];

        for (size_t m = 0; m < materials.size(); m++) {
          auto material = materials[m];

          const auto &color = material->get_color();

          auto cursor_pos = ImGui::GetCursorScreenPos();
          draw_list->AddRectFilled(
              cursor_pos,
              ImVec2{cursor_pos.x + 10.0f, cursor_pos.y + text_height},
              ImGui::ColorConvertFloat4ToU32(ImVec4{
                  color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, 1}));

          ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 16.0f);
          if (ImGui::Selectable(
                  (std::string(" ") + material->get_name()).c_str(),
                  _selected_material_index == m)) {
            _selected_material_index = m;
            _selected_material = material;
          }

          if (_selected_material == nullptr) {
            _selected_material = material;
          }
        }
        ImGui::EndListBox();
      }
    }
  } break;
  }

  ImGui::End();

  // Tile texture window

  auto texture_opened = ImGui::Begin("Texture##TilesPageTexture");
  _hovering_on_window |= ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows);

  if (texture_opened) {
    rlImGuiImageRenderTextureFit(&_tile_texture_rt, false);
  }

  ImGui::End();

  // Tile specs window

  auto specs_opened = ImGui::Begin("Specs##TilesPageSpecs");
  _hovering_on_window |= ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows);

  if (specs_opened) {
    rlImGuiImageRenderTextureFit(&_tile_specs_rt, false);
  }

  ImGui::End();
}
void Tile_Page::order_level_redraw() noexcept { _should_redraw = true; }
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
  f3->print(false, true);

  f3->print("Layer Pointer: Global");
  f3->print("L ");
  f3->print((int)ctx->level_layer_, true);

  f3->print("Tile Category Index ");
  f3->print(_selected_tile_category_index, true);

  f3->print("Tile Index ");
  f3->print(_selected_tile_index, true);

  f3->print("Material Category Index ");
  f3->print(_selected_material_category_index, true);

  f3->print("Material Index ");
  f3->print(_selected_material_index, true);

  f3->print("Hovered Tile ");
  if (_hovered_tile)
    f3->print(_hovered_tile->get_name(), true);
  else
    f3->print("NULL", MAGENTA, true);

  f3->print("Selected Tile ");
  if (_selected_tile)
    f3->print(_selected_tile->get_name(), true);
  else
    f3->print("NULL", MAGENTA, true);

  f3->print("Selected Material ");
  if (_selected_material)
    f3->print(_selected_material->get_name(), true);
  else
    f3->print("NULL", MAGENTA, true);

  f3->print("Hovered Cell ");
  if (_hovered_cell) {
    switch (_hovered_cell->type) {
    case TileType::head: {
      auto *def = _hovered_cell->tile_def;

      if (def == nullptr) {
        f3->print("Undefined Tile \"", true);
        f3->print(_hovered_cell->und_name, true);
        f3->print("\"", true);
      } else {
        f3->print(def->get_name(), true);
      }
    } break;

    case TileType::body: {
      auto *def = _hovered_cell->tile_def;

      if (def == nullptr) {
        f3->print("Undefined Tile \"", true);
        f3->print(_hovered_cell->und_name, true);
        f3->print("\"", true);
      } else {
        f3->print(def->get_name(), true);
        f3->print(std::string(" (") +
                      std::to_string(_hovered_cell->head_pos_x) + ", " +
                      std::to_string(_hovered_cell->head_pos_z) + ", " +
                      std::to_string(_hovered_cell->head_pos_y) + ")",
                  true);
      }
    } break;

    case TileType::material: {
      auto *def = _hovered_cell->material_def;

      if (def == nullptr) {
        f3->print("Undefined Material \"", true);
        f3->print(_hovered_cell->und_name, true);
        f3->print("\"", true);
      } else {
        f3->print(def->get_name(), true);
      }
    } break;

    default: {
      f3->print("Default Material (", true);
      f3->print(ctx->get_selected_level()->default_material.c_str(), ORANGE,
                true);
      f3->print(")", true);
    } break;
    }
  } else {
    f3->print("NULL", MAGENTA, true);
  }

  f3->print("Is Tile Legal ");
  f3->print(_is_tile_legal, true);

  f3->print("Edit Mode ");
  if (_edit_mode == EDIT_MODE_TILE)
    f3->print(" Tile", MAGENTA, true);
  else if (_edit_mode == EDIT_MODE_MATERIAL)
    f3->print(" Material", MAGENTA, true);
  else
    f3->print(" Unknown", MAGENTA, true);

  f3->print("Force Mode ");
  if (_force_mode == FORCE_MODE_PERMISSIVE)
    f3->print(" Permissive", MAGENTA, true);
  else if (_force_mode == FORCE_MODE_WITH_GEO)
    f3->print(" With Geometry", MAGENTA, true);
  else if (_force_mode == FORCE_MODE_WITHOUT_GEO)
    f3->print(" Without Geometry", MAGENTA, true);
  else
    f3->print(" Unknown", MAGENTA, true);
}

void Tile_Page::on_level_loaded() noexcept {
  _should_redraw1 = true;
  _should_redraw2 = true;
  _should_redraw3 = true;
  _should_redraw_tile1 = true;
  _should_redraw_tile2 = true;
  _should_redraw_tile3 = true;
  _should_redraw = true;
}

void Tile_Page::on_level_unloaded() noexcept {
  _should_redraw1 = true;
  _should_redraw2 = true;
  _should_redraw3 = true;
  _should_redraw_tile1 = true;
  _should_redraw_tile2 = true;
  _should_redraw_tile3 = true;
  _should_redraw = true;
}

void Tile_Page::on_level_selected() noexcept {
  _should_redraw1 = true;
  _should_redraw2 = true;
  _should_redraw3 = true;
  _should_redraw_tile1 = true;
  _should_redraw_tile2 = true;
  _should_redraw_tile3 = true;
  _should_redraw = true;
}

void Tile_Page::on_page_selected() noexcept {
  _should_redraw1 = true;
  _should_redraw2 = true;
  _should_redraw3 = true;
  _should_redraw_tile1 = true;
  _should_redraw_tile2 = true;
  _should_redraw_tile3 = true;
  _should_redraw = true;
}

void Tile_Page::on_mtx_pos_changed() {
  const auto *level = ctx->get_selected_level();

  if (level != nullptr) {
    _is_tile_legal = mr::utils::is_tile_legal(
        _selected_tile, level->get_const_tile_matrix(),
        level->get_const_geo_matrix(), _mtx_mouse_pos.x, _mtx_mouse_pos.y,
        ctx->level_layer_);
  }
}

Tile_Page::Tile_Page(context *ctx)
    : LevelPage(ctx), _should_redraw(true), _should_redraw1(true),
      _should_redraw2(true), _should_redraw3(true), _should_redraw_tile1(true),
      _should_redraw_tile2(true), _should_redraw_tile3(true),
      _hovering_on_window(false), _is_tile_legal(false), _edit_mode(0),
      _force_mode(0), _selected_tile_category_index(0), _selected_tile_index(0),
      _selected_material_category_index(0), _selected_material_index(0),
      _tile_preview_rt({0}), _tile_texture_rt({0}), _tile_specs_rt({0}),
      _selected_tile(nullptr), _selected_material(nullptr),
      _hovered_tile(nullptr), _previously_drawn_preview(nullptr),
      _previously_drawn_texture(nullptr), _hovered_cell(nullptr) {}

Tile_Page::~Tile_Page() {
  if (_tile_preview_rt.id != 0)
    UnloadRenderTexture(_tile_preview_rt);
  if (_tile_texture_rt.id != 0)
    UnloadRenderTexture(_tile_texture_rt);
  if (_tile_specs_rt.id != 0)
    UnloadRenderTexture(_tile_specs_rt);
}

}; // namespace mr::pages
