#include <string>
#include <iostream>

#include <imgui.h>
#include <raylib.h>
#include <rlImGui.h>

#include <MobitRenderer/default_array.h>
#include <MobitRenderer/pages.h>
#include <MobitRenderer/utils.h>
#include <MobitRenderer/sdraw.h>
#include <MobitRenderer/draw.h>
#include <MobitRenderer/quad.h>

namespace mr::pages {

void Props_Page::resize_indices() noexcept {
  const auto *level = ctx->get_selected_level();
  if (level == nullptr) return;

  _selected.resize(level->props.size());
  _hidden.resize(level->props.size());
}
void Props_Page::process() noexcept {
  if (ctx == nullptr) return;

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
      _should_redraw_props = true;
    }
  }

  update_mtx_mouse_pos();

 

  _hovering_on_window = false;
}

void Props_Page::_redraw_tile_preview_rt() noexcept {
  if (_hovered_tile == _previously_drawn_tile_texture)
    return;

  _previously_drawn_tile_texture = _hovered_tile;

  mr::utils::unload_rendertexture(_tile_texture_rt);

  if (_hovered_tile == nullptr)
    return;

  _tile_texture_rt = LoadRenderTexture(_hovered_tile->calculate_width(20),
                                       _hovered_tile->calculate_height(20));

  BeginTextureMode(_tile_texture_rt);
  {
    ClearBackground(Color{0, 0, 0, 0});

    const auto &texture = _hovered_tile->get_loaded_texture();

    if (_hovered_tile->get_type() == TileDefType::box) {
      const auto &shader = ctx->_shaders->white_remover_rgb_recolor();

      DrawTexturePro(texture,
        Rectangle{0,
                  1.0f * _hovered_tile->get_width() *
                      _hovered_tile->get_height() * 20,
                  1.0f * _hovered_tile->calculate_width(20),
                  1.0f * _hovered_tile->calculate_height(20)},
        Rectangle{0, 0,
                  1.0f * _hovered_tile->calculate_width(20),
                  1.0f * _hovered_tile->calculate_height(20)},
        Vector2{0, 0}, 0, 
        WHITE
      );
    } else {
      const auto &shader = ctx->_shaders->voxel_struct();
      BeginShaderMode(shader);
      {
        SetShaderValueTexture(shader, GetShaderLocation(shader, "texture0"),
                              texture);

        auto layers = _hovered_tile->get_repeat().size();
        SetShaderValue(shader, GetShaderLocation(shader, "layers"), &layers,
                       SHADER_UNIFORM_INT);

        float height = _hovered_tile->calculate_height(20) * 1.0f /
                       _hovered_tile->get_texture().height;
        SetShaderValue(shader, GetShaderLocation(shader, "height"), &height,
                       SHADER_UNIFORM_FLOAT);

        float width = _hovered_tile->calculate_width(20) * 1.0f /
                      _hovered_tile->get_texture().width;
        SetShaderValue(shader, GetShaderLocation(shader, "width"), &width,
                       SHADER_UNIFORM_FLOAT);

        float depth = -(0.8f / layers);
        SetShaderValue(shader, GetShaderLocation(shader, "depth"), &depth,
                       SHADER_UNIFORM_FLOAT);

        int offset = 0;
        SetShaderValue(shader, GetShaderLocation(shader, "depthOffset"), &offset, SHADER_UNIFORM_INT);

        DrawTexturePro(
            texture,
            Rectangle{0, 0, (float)_hovered_tile->get_texture().width,
                      (float)_hovered_tile->get_texture().height},
            Rectangle{0, 0, (float)_hovered_tile->calculate_width(20),
                      (float)_hovered_tile->calculate_height(20)},
            Vector2{0, 0}, 0, _hovered_tile->get_color());
      }
      EndShaderMode();
    }
  }
  EndTextureMode();
}
void Props_Page::_redraw_prop_preview_rt() noexcept {
  if (_hovered_prop == _previously_drawn_prop_texture) return;

  _previously_drawn_prop_texture = _hovered_prop;

  mr::utils::unload_rendertexture(_prop_texture_rt);

  if (_hovered_prop == nullptr) return;

  const auto &texture = _hovered_prop->get_loaded_texture();
  if (!_hovered_prop->is_loaded()) return;

  const auto width = _hovered_prop->get_pixel_width();
  const auto height = _hovered_prop->get_pixel_height();

  _prop_texture_rt = LoadRenderTexture(width, height);

  BeginTextureMode(_prop_texture_rt);
  {
    ClearBackground(Color{0, 0, 0, 0});

    const PropSettings settings;

    mr::sdraw::draw_prop_preview(
      _hovered_prop,
      &settings,
      ctx->_shaders,
      Quad(Rectangle{0, 0, (float)width, (float)height}),
      1.0f/30.0f,
      0
    );
  }
  EndTextureMode();
}

void Props_Page::draw() noexcept { 
  if (ctx == nullptr) return;

  const auto *level = ctx->get_selected_level();

  if (level == nullptr) return;

  ClearBackground(DARKGRAY);

  auto &camera = ctx->get_camera();
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

  if (_should_redraw_tile1) {
    BeginTextureMode(ctx->_textures->tile_layer1.get());

    ClearBackground(WHITE);

    mr::sdraw::draw_tile_prevs_layer(
      ctx->_shaders,
      level->get_const_geo_matrix(),
      level->get_const_tile_matrix(),
      ctx->_tiledex,
      ctx->_materialdex,
      0,
      20
    );

    EndTextureMode();

    _should_redraw_tile1 = false;
    _should_redraw = true;
  }

  if (_should_redraw_tile2) {
    BeginTextureMode(ctx->_textures->tile_layer2.get());

    ClearBackground(WHITE);

    mr::sdraw::draw_tile_prevs_layer(
      ctx->_shaders,
      level->get_const_geo_matrix(),
      level->get_const_tile_matrix(),
      ctx->_tiledex,
      ctx->_materialdex,
      1,
      20
    );

    EndTextureMode();

    _should_redraw_tile1 = false;
    _should_redraw = true;
  }

  if (_should_redraw_tile3) {
    BeginTextureMode(ctx->_textures->tile_layer3.get());

    ClearBackground(WHITE);

    mr::sdraw::draw_tile_prevs_layer(
      ctx->_shaders,
      level->get_const_geo_matrix(),
      level->get_const_tile_matrix(),
      ctx->_tiledex,
      ctx->_materialdex,
      2,
      20
    );

    EndTextureMode();

    _should_redraw_tile1 = false;
    _should_redraw = true;
  }

  if (_should_redraw_props) {
    
    BeginTextureMode(ctx->_textures->props.get());
    ClearBackground(Color{0, 0, 0, 0});
    // ClearBackground(WHITE);

    for (auto &prop : level->props) {
      if (prop->tile_def == nullptr && prop->prop_def == nullptr) continue;

      prop->load_texture();
      if (!prop->is_loaded()) continue;

      mr::sdraw::draw_prop_preview(prop.get(), ctx->_shaders, ctx->level_layer_ * 10);
    }

    EndTextureMode();

    _should_redraw_props = false;
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
          SetShaderValueTexture(
            color_shader, 
            GetShaderLocation(color_shader, "texture0"), 
            ctx->_textures->geo_layer3.get().texture
          );
          
          DrawTexture(
            ctx->_textures->geo_layer3.get().texture, 
            0, 
            0, 
            Color{50, 50, 50, 255}
          );
        }
        EndShaderMode();

        const auto &bkg_shader = ctx->_shaders->white_remover_apply_alpha();
        BeginShaderMode(bkg_shader);
        {
          SetShaderValueTexture(
            bkg_shader, 
            GetShaderLocation(bkg_shader, "texture0"), 
            ctx->_textures->geo_layer3.get().texture
          );

          int alpha = 200;

          SetShaderValue(bkg_shader, GetShaderLocation(bkg_shader, "alpha"), &alpha, SHADER_UNIFORM_INT);

          DrawTexture(
            ctx->_textures->tile_layer3.get().texture, 
            0, 
            0, 
            WHITE
          );
        }
        EndShaderMode();
      }

      if (layer != 1) {
        const auto &color_shader = ctx->_shaders->white_remover_apply_color();
        BeginShaderMode(color_shader);
        {
          SetShaderValueTexture(
            color_shader, 
            GetShaderLocation(color_shader, "texture0"), 
            ctx->_textures->geo_layer2.get().texture
          );
          
          DrawTexture(
            ctx->_textures->geo_layer2.get().texture, 
            0, 
            0, 
            Color{20, 20, 20, 200}
          );
        }
        EndShaderMode();

        const auto &bkg_shader = ctx->_shaders->white_remover_apply_alpha();
        BeginShaderMode(bkg_shader);
        {
          SetShaderValueTexture(
            bkg_shader, 
            GetShaderLocation(bkg_shader, "texture0"), 
            ctx->_textures->geo_layer2.get().texture
          );

          int alpha = 200;

          SetShaderValue(bkg_shader, GetShaderLocation(bkg_shader, "alpha"), &alpha, SHADER_UNIFORM_INT);

          DrawTexture(
            ctx->_textures->tile_layer2.get().texture, 
            0, 
            0, 
            WHITE
          );
        }
        EndShaderMode();
      }

      if (layer != 0) {
        const auto &color_shader = ctx->_shaders->white_remover_apply_color();
        BeginShaderMode(color_shader);
        {
          SetShaderValueTexture(
            color_shader, 
            GetShaderLocation(color_shader, "texture0"), 
            ctx->_textures->geo_layer1.get().texture
          );
          
          DrawTexture(
            ctx->_textures->geo_layer1.get().texture, 
            0, 
            0, 
            Color{0, 0, 0, 230}
          );
        }
        EndShaderMode();

        const auto &bkg_shader = ctx->_shaders->white_remover_apply_alpha();
        BeginShaderMode(bkg_shader);
        {
          SetShaderValueTexture(
            bkg_shader, 
            GetShaderLocation(bkg_shader, "texture0"), 
            ctx->_textures->geo_layer1.get().texture
          );

          int alpha = 230;

          SetShaderValue(bkg_shader, GetShaderLocation(bkg_shader, "alpha"), &alpha, SHADER_UNIFORM_INT);

          DrawTexture(
            ctx->_textures->tile_layer1.get().texture, 
            0, 
            0, 
            WHITE
          );
        }
        EndShaderMode();
      }

      // Foreground

      if (layer == 2) {
        const auto &color_shader = ctx->_shaders->white_remover_apply_color();
        BeginShaderMode(color_shader);
        {
          SetShaderValueTexture(
            color_shader, 
            GetShaderLocation(color_shader, "texture0"), 
            ctx->_textures->geo_layer3.get().texture
          );
          
          DrawTexture(
            ctx->_textures->geo_layer3.get().texture, 
            0, 
            0, 
            Color{0, 0, 0, 220}
          );
        }
        EndShaderMode();

        const auto &bkg_shader = ctx->_shaders->white_remover();
        BeginShaderMode(bkg_shader);
        {
          SetShaderValueTexture(
            color_shader, 
            GetShaderLocation(color_shader, "texture0"), 
            ctx->_textures->geo_layer3.get().texture
          );

          DrawTexture(
            ctx->_textures->tile_layer3.get().texture, 
            0, 
            0, 
            WHITE
          );
        }
        EndShaderMode();
      }

      if (layer == 1) {
        const auto &color_shader = ctx->_shaders->white_remover_apply_color();
        BeginShaderMode(color_shader);
        {
          SetShaderValueTexture(
            color_shader, 
            GetShaderLocation(color_shader, "texture0"), 
            ctx->_textures->geo_layer2.get().texture
          );
          
          DrawTexture(
            ctx->_textures->geo_layer2.get().texture, 
            0, 
            0, 
            Color{0, 0, 0, 220}
          );
        }
        EndShaderMode();

        const auto &bkg_shader = ctx->_shaders->white_remover();
        BeginShaderMode(bkg_shader);
        {
          SetShaderValueTexture(
            color_shader, 
            GetShaderLocation(color_shader, "texture0"), 
            ctx->_textures->geo_layer2.get().texture
          );

          DrawTexture(
            ctx->_textures->tile_layer2.get().texture, 
            0, 
            0, 
            WHITE
          );
        }
        EndShaderMode();
      }

      if (layer == 0) {
        const auto &color_shader = ctx->_shaders->white_remover_apply_color();
        BeginShaderMode(color_shader);
        {
          SetShaderValueTexture(
            color_shader, 
            GetShaderLocation(color_shader, "texture0"), 
            ctx->_textures->geo_layer1.get().texture
          );
          
          DrawTexture(
            ctx->_textures->geo_layer1.get().texture, 
            0, 
            0, 
            Color{0, 0, 0, 220}
          );
        }
        EndShaderMode();

        const auto &bkg_shader = ctx->_shaders->white_remover();
        BeginShaderMode(bkg_shader);
        {
          SetShaderValueTexture(
            color_shader, 
            GetShaderLocation(color_shader, "texture0"), 
            ctx->_textures->geo_layer1.get().texture
          );

          DrawTexture(
            ctx->_textures->tile_layer1.get().texture, 
            0, 
            0, 
            WHITE
          );
        }
        EndShaderMode();
      }
    
      DrawTexture(ctx->_textures->props.get().texture, 0, 0, WHITE);

    }
    EndTextureMode();

    _should_redraw = false;
  }

  ClearBackground(DARKGRAY); 
  
  BeginMode2D(camera);

  DrawTexture(viewport.texture, 0, 0, WHITE);

  mr::draw::draw_double_frame(
    level->get_pixel_width(), 
    level->get_pixel_height()
  );
  
  // Props layers

  // const auto &shader = ctx->_shaders->white_remover();
  // const auto &texture = ctx->_textures->props.get().texture;

  // BeginShaderMode(shader);
  // SetShaderValueTexture(shader, GetShaderLocation(shader, "texture0"), texture);
  // DrawTexture(texture, -200, -200, WHITE);
  // EndShaderMode();

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


  EndMode2D();
}

void Props_Page::windows() noexcept {
  auto tiles_opened = ImGui::Begin("Tiles##PropsPageTilesMenu");
  _hovering_on_window |= ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows);

  if (tiles_opened && ctx->_propdex != nullptr) {

    auto *tiles = ctx->_propdex;

    ImGui::Columns(2);

    auto *draw_list = ImGui::GetWindowDrawList();
    auto text_height = ImGui::GetTextLineHeight();

    // Tile Categories
    if (ImGui::BeginListBox("##Categories", ImGui::GetContentRegionAvail())) {
      for (size_t c = 0; c < tiles->tile_categories().size(); c++) {
        const auto &category = tiles->tile_categories().at(c);

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
        }
      }

      ImGui::EndListBox();
    }

    ImGui::NextColumn();

    // Tiles
    if (ImGui::BeginListBox("##CategoryTiles", ImGui::GetContentRegionAvail())) {
      const auto &category_tiles =
          tiles->sorted_tiles().at(_selected_tile_category_index);

      for (size_t t = 0; t < category_tiles.size(); t++) {
        auto *tiledef = category_tiles[t];

        bool selected = ImGui::Selectable(tiledef->get_name().c_str(),
                                          t == _selected_tile_index);
        if (selected) {
          _selected_tile_index = t;
          _selected_tile = tiledef;
        }

        if (ImGui::IsItemHovered()) {
          _hovered_tile = tiledef;
          _redraw_tile_preview_rt();

          ImGui::BeginTooltip();
          rlImGuiImageRenderTexture(&_tile_texture_rt);
          ImGui::EndTooltip();
        }
      }

      ImGui::EndListBox();
    }
  }

  ImGui::End();

  auto props_opened = ImGui::Begin("Props##PropsPagePropsMenu");
  _hovering_on_window |= ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows);

  if (props_opened && ctx->_propdex != nullptr) {

    auto *props = ctx->_propdex;

    ImGui::Columns(2);

    auto *draw_list = ImGui::GetWindowDrawList();
    auto text_height = ImGui::GetTextLineHeight();

    // Prop Categories
    if (ImGui::BeginListBox("##Categories", ImGui::GetContentRegionAvail()) &&
        !props->categories().empty()) {
      for (size_t c = 0; c < props->categories().size(); c++) {
        const auto &category = props->categories().at(c);

        auto cursor_pos = ImGui::GetCursorScreenPos();
        draw_list->AddRectFilled(
            cursor_pos,
            ImVec2{cursor_pos.x + 10.0f, cursor_pos.y + text_height},
            ImGui::ColorConvertFloat4ToU32(
                ImVec4{category.color.r / 255.0f, category.color.g / 255.0f,
                       category.color.b / 255.0f, 1}));

        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 16.0f);

        bool selected = ImGui::Selectable(category.name.c_str(),
                                          _selected_prop_category_index == c);
        if (selected) {
          _selected_prop_category_index = c;
          _selected_prop_index = 0;
        }
      }

      ImGui::EndListBox();
    }

    ImGui::NextColumn();

    // Props
    if (ImGui::BeginListBox("##CategoryProps",
                            ImGui::GetContentRegionAvail()) &&
        !props->categories().empty() && !props->sorted_props().empty() &&
        !props->sorted_props().at(_selected_prop_category_index).empty()) {

      const auto &category_props =
          props->sorted_props().at(_selected_prop_category_index);

      for (size_t t = 0; t < category_props.size(); t++) {
        auto *propdef = category_props[t];

        bool selected =
            ImGui::Selectable(propdef->name.c_str(), t == _selected_prop_index);
        if (selected) {
          _selected_prop_index = t;
          _selected_prop = propdef;
        }

        if (ImGui::IsItemHovered()) {
          _hovered_prop = propdef;
          _redraw_prop_preview_rt();

          ImGui::BeginTooltip();
          rlImGuiImageRenderTexture(&_prop_texture_rt);
          ImGui::EndTooltip();
        }
      }

      ImGui::EndListBox();
    }
  }

  ImGui::End();

  auto list_opened = ImGui::Begin("Props List##PropsPagePropsList");
  _hovering_on_window |= ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows);

  if (list_opened) {
    const auto *level = ctx->get_selected_level();

    if (ImGui::BeginListBox("##List", ImGui::GetContentRegionAvail()) && 
      level != nullptr && 
      !level->props.empty()) {

      const auto &props = level->props;
      
      for (size_t x = 0; x < props.size(); x++) {
        const auto &prop = props[x];
        
        std::string label("");
        std::string x_str = std::to_string(x + 3);
        label.reserve(prop->und_name->size() + x_str.size());
        label.append(*(prop->und_name.get()));
        label.append("##"+x_str);

        if (ImGui::Selectable(label.c_str(), _selected[x])) {
          if (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) {
            _selected[x] = !_selected[x];
          } else {
            _selected.fill();
            _selected[x] = true;
          }
        }
      }

      ImGui::EndListBox();
    }
  }

  ImGui::End();
}

void Props_Page::f3() const noexcept {
  if (ctx == nullptr) return;
  if (ctx->f3_ == nullptr) return;

  const auto f3 = ctx->f3_;

  const auto &camera = ctx->get_camera();

  f3->print("Zoom ");
  f3->print(camera.zoom, true);

  f3->print("Target ");
  f3->print(camera.target, true);

  f3->print("Offset ");
  f3->print(camera.offset, true);

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

  f3->print("Hovered Prop: ");
  if (_hovered_prop) {
    f3->print(_hovered_prop->name, true);
    f3->print(" / ", true);
    f3->print(prop_type_c_str(_hovered_prop->type), true);
  }
  else f3->print("NULL", true);

  f3->print("");

  f3->print("Selected Tile: ");
  if (_selected_tile) f3->print(_selected_tile->get_name(), true);
  else f3->print("NULL", true);

  f3->print("Selected Prop: ");
  if (_selected_prop) {
    f3->print(_selected_prop->name, true);
    f3->print(" / ", true);
    f3->print(prop_type_c_str(_selected_prop->type), true);
  }
  else f3->print("NULL", true);
}

void Props_Page::on_level_loaded() noexcept {
  resize_indices();

  _should_redraw1 = true;
  _should_redraw2 = true;
  _should_redraw3 = true;
  _should_redraw_tile1 = true;
  _should_redraw_tile2 = true;
  _should_redraw_tile3 = true;
  _should_redraw_props = true;
  _should_redraw = true;
}

void Props_Page::on_level_unloaded() noexcept {
  resize_indices();

  _should_redraw1 = true;
  _should_redraw2 = true;
  _should_redraw3 = true;
  _should_redraw_tile1 = true;
  _should_redraw_tile2 = true;
  _should_redraw_tile3 = true;
  _should_redraw_props = true;
  _should_redraw = true;
}

void Props_Page::on_level_selected() noexcept {
  resize_indices();
  
  _should_redraw1 = true;
  _should_redraw2 = true;
  _should_redraw3 = true;
  _should_redraw_tile1 = true;
  _should_redraw_tile2 = true;
  _should_redraw_tile3 = true;
  _should_redraw_props = true;
  _should_redraw = true;
}

void Props_Page::on_page_selected() noexcept {
  _should_redraw1 = true;
  _should_redraw2 = true;
  _should_redraw3 = true;
  _should_redraw_tile1 = true;
  _should_redraw_tile2 = true;
  _should_redraw_tile3 = true;
  _should_redraw_props = true;
  _should_redraw = true;
}

Props_Page::Props_Page(context *ctx)
    : LevelPage(ctx), _selected_tile(nullptr), _hovered_tile(nullptr),
      _selected_prop(nullptr), _hovered_prop(nullptr), _selected_tile_index(0),
      _selected_tile_category_index(0), _selected_prop_index(0),
      _selected_prop_category_index(0), _hovering_on_window(true),
      
      _should_redraw(true),
      _should_redraw1(true),
      _should_redraw2(true),
      _should_redraw3(true),
      _should_redraw_tile1(true),
      _should_redraw_tile2(true),
      _should_redraw_tile3(true),
      _should_redraw_props(true),
      
      _previously_drawn_tile_texture(nullptr),
      _previously_drawn_prop_texture(nullptr), _tile_texture_rt({0}),
      _prop_texture_rt({0}),
      
      _selected(0, false),
      _hidden(0, false) {}

Props_Page::~Props_Page() {
  mr::utils::unload_rendertexture(_tile_texture_rt);
  mr::utils::unload_rendertexture(_prop_texture_rt);
}

}; // namespace mr::pages
