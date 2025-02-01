#include <imgui.h>
#include <raylib.h>
#include <rlImGui.h>

#include <MobitRenderer/pages.h>
#include <MobitRenderer/utils.h>

namespace mr::pages {

void Props_Page::process() noexcept { _hovering_on_window = false; }

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

        auto depth = 2;
        SetShaderValue(shader, GetShaderLocation(shader, "depth"), &depth,
                       SHADER_UNIFORM_INT);

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
void Props_Page::_redraw_prop_preview_rt() noexcept {}

void Props_Page::draw() noexcept { ClearBackground(DARKGRAY); }

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
    if (ImGui::BeginListBox("##CategoryTiles",
                            ImGui::GetContentRegionAvail())) {
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

          ImGui::BeginTooltip();
          // rlImGuiImageRenderTexture(&_tile_preview_rt);
          ImGui::EndTooltip();
        }
      }

      ImGui::EndListBox();
    }
  }

  ImGui::End();
}

void Props_Page::f3() const noexcept {}

Props_Page::Props_Page(context *ctx)
    : LevelPage(ctx), _selected_tile(nullptr), _hovered_tile(nullptr),
      _selected_prop(nullptr), _hovered_prop(nullptr), _selected_tile_index(0),
      _selected_tile_category_index(0), _selected_prop_index(0),
      _selected_prop_category_index(0), _hovering_on_window(true),
      _should_redraw(true), _previously_drawn_tile_texture(nullptr),
      _previously_drawn_prop_texture(nullptr), _tile_texture_rt({0}),
      _prop_texture_rt({0}) {}
Props_Page::~Props_Page() {
  mr::utils::unload_rendertexture(_tile_texture_rt);
  mr::utils::unload_rendertexture(_prop_texture_rt);
}

}; // namespace mr::pages
