#include <cstdint>
#include <memory>
#include <iostream>

#include <raylib.h>
#include <raymath.h>
#include <imgui.h>
#include <rlImGui.h>

#include <MobitRenderer/draw.h>
#include <MobitRenderer/matrix.h>
#include <MobitRenderer/pages.h>

namespace mr::pages {

void Tile_Page::_redraw_tile_preview_rt() noexcept {
  if (
    _hovered_tile == nullptr || 
    _hovered_tile == _previously_drawn_preview
  ) return;

  _previously_drawn_preview = _hovered_tile;

  if (_tile_preview_rt.id != 0) 
    UnloadRenderTexture(_tile_preview_rt);

  _tile_preview_rt = LoadRenderTexture(
    _hovered_tile->get_width() * 16, 
    _hovered_tile->get_height() * 16
  );

  BeginTextureMode(_tile_preview_rt);
  {
    ClearBackground(Color{0, 0, 0, 0});
    
    const auto &shader = ctx->_shaders->white_remover_apply_color();
    const auto &texture = _hovered_tile->get_loaded_texture();
    BeginShaderMode(shader);
    {
      SetShaderValueTexture(shader, GetShaderLocation(shader, "texture0"), texture);
      mr::draw::draw_tile_prev(_hovered_tile, 0, 0, 16, WHITE);
    }
    EndShaderMode();
  }
  EndTextureMode();
}
void Tile_Page::_redraw_tile_texture_rt() noexcept {
  if (
    _selected_tile == nullptr || 
    _selected_tile == _previously_drawn_texture
  ) return;

  _previously_drawn_texture = _selected_tile;

  if (_tile_texture_rt.id != 0) 
    UnloadRenderTexture(_tile_texture_rt);

  _tile_texture_rt = LoadRenderTexture(
    _selected_tile->calculate_width(20), 
    _selected_tile->calculate_height(20)
  );

  BeginTextureMode(_tile_texture_rt);
  {
    ClearBackground(Color{0, 0, 0, 0});
    
    const auto &texture = _selected_tile->get_loaded_texture();
    
    if (_selected_tile->get_type() == TileDefType::box) {
      const auto &shader = ctx->_shaders->white_remover_rgb_recolor();

      BeginShaderMode(shader);
      {
        SetShaderValueTexture(shader, GetShaderLocation(shader, "texture0"), texture);

        DrawTexturePro(
          texture,
          Rectangle{
            0,
            1.0f * _selected_tile->get_width() * _selected_tile->get_height() * 20,
            1.0f * _selected_tile->calculate_width(20),
            1.0f * _selected_tile->calculate_height(20)
          },
          Rectangle{
            0,
            0,
            1.0f * _selected_tile->calculate_width(20),
            1.0f * _selected_tile->calculate_height(20)
          },
          Vector2{0, 0},
          0,
          ctx->_tiledex->colors().at(_selected_tile->get_category())
        );
      }
      EndShaderMode();
    } else {
      const auto &shader = ctx->_shaders->voxel_struct_tinted();
      BeginShaderMode(shader);
      {
        SetShaderValueTexture(shader, GetShaderLocation(shader, "texture0"), texture);
        
        auto layers = _selected_tile->get_repeat().size();
        SetShaderValue(shader, GetShaderLocation(shader, "layers"), &layers, SHADER_UNIFORM_INT);

        float height = _selected_tile->calculate_height(20)*1.0f / _selected_tile->get_texture().height;
        SetShaderValue(shader, GetShaderLocation(shader, "height"), &height, SHADER_UNIFORM_FLOAT);
        
        float width = _selected_tile->calculate_width(20)*1.0f / _selected_tile->get_texture().width;
        SetShaderValue(shader, GetShaderLocation(shader, "width"), &width, SHADER_UNIFORM_FLOAT);
        
        auto depth = 2;
        SetShaderValue(shader, GetShaderLocation(shader, "depth"), &depth, SHADER_UNIFORM_INT);
        
        DrawTexturePro(
          texture,
          Rectangle{
            0, 
            0, 
            (float)_selected_tile->get_texture().width, 
            (float)_selected_tile->get_texture().height
          },
          Rectangle{
            0, 
            0, 
            (float)_selected_tile->calculate_width(20),
            (float)_selected_tile->calculate_height(20)
          },
          Vector2{0, 0},
          0,
          ctx->_tiledex->colors().at(_selected_tile->get_category())
        );

        // draw_tile_tinted(_selected_tile, 0, 0, 20, ctx->_tiledex->colors().at(_selected_tile->get_category()));
      }
      EndShaderMode();
    }
  }
  EndTextureMode();
}
void Tile_Page::_redraw_tile_specs_rt() noexcept {
  if (
    _selected_tile == nullptr || 
    _selected_tile == _previously_drawn_specs
  ) return;

  _previously_drawn_specs = _selected_tile;

  if (_tile_specs_rt.id != 0) 
    UnloadRenderTexture(_tile_specs_rt);

  _tile_specs_rt = LoadRenderTexture(
    _selected_tile->calculate_width(10), 
    _selected_tile->calculate_height(10)
  );

  BeginTextureMode(_tile_specs_rt);
  {
    for (int8_t x = 0; x < _selected_tile->get_width(); x++) {
      for (int8_t y = 0; y < _selected_tile->get_height(); y++) {
        auto spec = _selected_tile->get_specs()[y + x*_selected_tile->get_width()];

        if (spec < 0) continue;

        if (spec == 1) DrawRectangleLinesEx(
          Rectangle{x * 10.0f, y * 10.0f, 10.0f, 10.0f},
          1,
          WHITE
        );
      }
    }

    // specs 2

    if (_selected_tile->get_specs2().size() > 0) {
      for (int8_t x = 0; x < _selected_tile->get_width(); x++) {
        for (int8_t y = 0; y < _selected_tile->get_height(); y++) {
          auto spec = _selected_tile->get_specs2()[y + x*_selected_tile->get_width()];

          if (spec < 0) continue;

          if (spec == 1) DrawRectangleLinesEx(
            Rectangle{x * 10.0f + 2, y * 10.0f + 2, 6.0f, 6.0f},
            1,
            GREEN
          );
        }
      }
    }
  }
  EndTextureMode();
}

void Tile_Page::process() {  
  auto wheel = GetMouseWheelMove();

  if (!_hovering_on_window) {
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

    if (IsKeyPressed(KEY_L)) {
      ctx->level_layer_ = (ctx->level_layer_ + 1) % 3;
      _should_redraw = true;
    }
  }

  _hovering_on_window = false;
}

void Tile_Page::draw() noexcept {
  ClearBackground(DARKGRAY);

  auto &camera = ctx->get_camera();
  const auto *level = ctx->get_selected_level();
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
    BeginTextureMode(ctx->_textures->get_main_level_viewport());
    ClearBackground(Color{200, 200, 200, 255});

    const auto &shader = ctx->_shaders->white_remover_apply_color();
    BeginShaderMode(shader);
    {
      const auto layer = ctx->level_layer_;

      if (layer != 2) {
        SetShaderValueTexture(
          shader, 
          GetShaderLocation(shader, "texture0"), 
          ctx->_textures->geo_layer3.get().texture
        );
        DrawTexture(
          ctx->_textures->geo_layer3.get().texture, 
          0, 
          0, 
          Color{40, 40, 40, 200}
        );
      }

      if (layer != 1) {
        SetShaderValueTexture(
          shader, 
          GetShaderLocation(shader, "texture0"), 
          ctx->_textures->geo_layer2.get().texture
        );
        DrawTexture(
          ctx->_textures->geo_layer2.get().texture, 
          0, 
          0, 
          Color{20, 20, 20, 200}
        );
      }

      if (layer != 0) {
        SetShaderValueTexture(
          shader, 
          GetShaderLocation(shader, "texture0"), 
          ctx->_textures->geo_layer1.get().texture
        );
        DrawTexture(
          ctx->_textures->geo_layer1.get().texture, 
          0, 
          0, 
          Color{0, 0, 0, 120}
        );
      }

      // Foreground

      if (layer == 2) {
        SetShaderValueTexture(
          shader, 
          GetShaderLocation(shader, "texture0"), 
          ctx->_textures->geo_layer3.get().texture
        );
        DrawTexture(
          ctx->_textures->geo_layer3.get().texture, 
          0, 
          0, 
          Color{0, 0, 0, 220}
        );
      }

      if (layer == 1) {
        SetShaderValueTexture(
          shader, 
          GetShaderLocation(shader, "texture0"), 
          ctx->_textures->geo_layer2.get().texture
        );
        DrawTexture(
          ctx->_textures->geo_layer2.get().texture, 
          0, 
          0, 
          Color{0, 0, 0, 220}
        );
      }

      if (layer == 0) {
        SetShaderValueTexture(
          shader, 
          GetShaderLocation(shader, "texture0"), 
          ctx->_textures->geo_layer1.get().texture
        );
        DrawTexture(
          ctx->_textures->geo_layer1.get().texture, 
          0, 
          0, 
          Color{0, 0, 0, 220}
        );
      }
    }
    EndShaderMode();
    
    EndTextureMode();

    _should_redraw = false;
  }

  BeginMode2D(camera);
  {
    DrawTexture(viewport.texture, 0, 0, WHITE);
    
    // if (ctx->get_config_const().grid.visible) {
    //   mr::draw_nested_grid(72, 43, Color{130, 130, 130, 200});
    // }

    mr::draw::draw_double_frame(
      level->get_pixel_width(), 
      level->get_pixel_height()
    );
  }
  EndMode2D();
}
void Tile_Page::windows() noexcept {
  auto tiles_opened = ImGui::Begin("Tiles##TilesPageTiles");
  _hovering_on_window |= ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows);

  auto *dex = ctx->_tiledex;
  if (tiles_opened && dex != nullptr) {
    ImGui::Columns(2);

    auto *draw_list = ImGui::GetWindowDrawList();
    auto text_height = ImGui::GetTextLineHeight();

    // Tile Categories
    if (ImGui::BeginListBox("##Categories", ImGui::GetContentRegionAvail())) {
      for (size_t c = 0; c < dex->categories().size(); c++) {
        const auto &category = dex->categories().at(c);

        auto cursor_pos = ImGui::GetCursorScreenPos();
        draw_list->AddRectFilled(
          cursor_pos,
          ImVec2{cursor_pos.x + 10.0f, cursor_pos.y + text_height},
          ImGui::ColorConvertFloat4ToU32(ImVec4{category.color.r/255.0f, category.color.g/255.0f, category.color.b/255.0f, 1})
        );

        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 16.0f);

        bool selected = ImGui::Selectable(category.name.c_str(), _selected_category_index == c);
        if (selected) {
          _selected_category_index = c;
          _selected_tile_index = 0;
        }
      }
      
      ImGui::EndListBox();
    }

    ImGui::NextColumn();

    // Tiles
    if (ImGui::BeginListBox("##CategoryTiles", ImGui::GetContentRegionAvail())) {
      const auto &category_tiles = dex->sorted_tiles().at(_selected_category_index);

      for (size_t t = 0; t < category_tiles.size(); t++) {
        auto *tiledef = category_tiles[t];

        bool selected = ImGui::Selectable(tiledef->get_name().c_str(), t == _selected_tile_index);
        if (selected) {
          _selected_tile_index = t;
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
  f3->print(ctx->get_config_const().grid.visible, true);

  f3->print("Layer Pointer: Global");
  f3->print("L ");
  f3->print((int)ctx->level_layer_, true);

  f3->print("Selected ");
  f3->print(_selected_tile, true);

  f3->print("Hovered ");
  f3->print(_hovered_tile, true);
}

Tile_Page::Tile_Page(context *ctx) : 
    Page(ctx),
    _should_redraw(true), 
    _should_redraw1(true),
    _should_redraw2(true),
    _should_redraw3(true),
    _should_redraw_tile1(true),
    _should_redraw_tile2(true),
    _should_redraw_tile3(true),
    _hovering_on_window(false),
    _selected_category_index(0),
    _selected_tile_index(0),
    _tile_preview_rt({0}),
    _tile_texture_rt({0}),
    _tile_specs_rt({0}),
    _selected_tile(nullptr),
    _hovered_tile(nullptr),
    _previously_drawn_preview(nullptr),
    _previously_drawn_texture(nullptr) {}

Tile_Page::~Tile_Page() {
  if (_tile_preview_rt.id != 0) UnloadRenderTexture(_tile_preview_rt);
  if (_tile_texture_rt.id != 0) UnloadRenderTexture(_tile_texture_rt);
  if (_tile_specs_rt.id != 0) UnloadRenderTexture(_tile_specs_rt);
}

};