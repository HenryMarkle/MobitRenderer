#include <filesystem>
#include <cmath>

#include <raylib.h>
#include <imgui.h>
#include <rlimgui.h>

#include <MobitRenderer/pages.h>
#include <MobitRenderer/state.h>
#include <MobitRenderer/sdraw.h>

#define PAINT_SHADOWS_MODE 0
#define PAINT_LIGHT_MODE 1

inline Vector2 projection_angle(int degree) {
    degree += 90;
    degree *= -1;

    float rad = degree / 360.0f * PI * 2;

    return Vector2{-cos(rad), sin(rad)};
}

namespace mr::pages {

void Light_Page::_update_projection_angle() noexcept {
    if (ctx == nullptr) return;

    const auto *level = ctx->get_selected_level();

    if (level == nullptr) return; 

    _projection_angle = (projection_angle(level->light_angle) * level->light_flatness * 10) + Vector2{-300, -300};
}

void Light_Page::on_level_loaded() noexcept {
    _should_redraw = true;
}
void Light_Page::on_level_unloaded() noexcept {
    _should_redraw = true;
}
void Light_Page::on_level_selected() noexcept {
    _should_redraw = true;
    _update_projection_angle();
}
void Light_Page::on_page_selected() noexcept {
    _should_redraw = true;
}

void Light_Page::process() {
    if (ctx == nullptr) return;

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

      if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && _paint_mode != PAINT_SHADOWS_MODE) _paint_mode = PAINT_SHADOWS_MODE;
      else if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT) && _paint_mode != PAINT_LIGHT_MODE) _paint_mode = PAINT_LIGHT_MODE;
    }

    const auto &brushes = ctx->_textures->light_editor.brushes();
    
    if (brushes.size() > 0) {
        if (IsKeyPressed(KEY_R)) {
            if (_brush_index != 0) {
                _brush_index--;
            } else if (ctx->get_config()->list_wrap) {
                _brush_index = brushes.size() - 1;
            }
        } else if (IsKeyPressed(KEY_F)) {
            if (_brush_index < brushes.size() - 1) {
                _brush_index = ++_brush_index % brushes.size();
            } else if (ctx->get_config()->list_wrap) {
                _brush_index = 0;
            }
        }
    }

    if (IsKeyDown(KEY_W)) {
        _brush_height -= _brush_growth_speed + (_brush_growth_speed*2*IsKeyDown(KEY_LEFT_SHIFT));
        _half_brush_height = _brush_height / 2.0f;

        _brush_growth_speed += _brush_growth_accelaration;
    } else if (IsKeyDown(KEY_S)) {
        _brush_height += _brush_growth_speed + (_brush_growth_speed*2*IsKeyDown(KEY_LEFT_SHIFT));
        _half_brush_height = _brush_height / 2.0f;

        _brush_growth_speed += _brush_growth_accelaration;
    }

    if (IsKeyDown(KEY_D)) {
        _brush_width += _brush_growth_speed + (_brush_growth_speed*2*IsKeyDown(KEY_LEFT_SHIFT));
        _half_brush_width = _brush_width / 2.0f;

        _brush_growth_speed += _brush_growth_accelaration;
    } else if (IsKeyDown(KEY_A)) {
        _brush_width -= _brush_growth_speed + (_brush_growth_speed*2*IsKeyDown(KEY_LEFT_SHIFT));
        _half_brush_width = _brush_width / 2.0f;

        _brush_growth_speed += _brush_growth_accelaration;
    }

    if (IsKeyDown(KEY_Q)) {
        _brush_rotation -= _brush_growth_speed + (_brush_growth_speed*2*IsKeyDown(KEY_LEFT_SHIFT));
        _brush_growth_speed += _brush_growth_accelaration;
    } else if (IsKeyDown(KEY_E)) {
        _brush_rotation += _brush_growth_speed + (_brush_growth_speed*2*IsKeyDown(KEY_LEFT_SHIFT));
        _brush_growth_speed += _brush_growth_accelaration;
    }

    if (
        IsKeyReleased(KEY_W) || 
        IsKeyReleased(KEY_S) || 
        IsKeyReleased(KEY_A) || 
        IsKeyReleased(KEY_D) ||
        IsKeyReleased(KEY_Q) ||
        IsKeyReleased(KEY_E)
    ) {
        _brush_growth_speed = 3;
    }

    if (IsKeyDown(KEY_LEFT_CONTROL)) {
        _custom_rotation = true;
        _brush_rotation = atan2(mouse_pos.y - _brush_pinned_pos.y, mouse_pos.x - _brush_pinned_pos.x) * RAD2DEG + 90.0f;
        
        if (!_click_lock) _brush_pinned_pos = mouse_pos;
        _click_lock = true;
    } else {
        _custom_rotation = false;
        _click_lock = false;
    }

    auto *level = ctx->get_selected_level();

    if (level != nullptr) {
        if (IsKeyDown(KEY_T)) {
            if (level->light_flatness > 1) level->light_flatness--;
            _update_projection_angle();
        } else if (IsKeyDown(KEY_Y)) {
            if (level->light_flatness < 10) level->light_flatness++;
            _update_projection_angle();
        }

        if (IsKeyDown(KEY_G)) {
            if (++level->light_angle == 360) level->light_angle = 0;
            _update_projection_angle();
        } else if (IsKeyDown(KEY_H)) {
            if (--level->light_angle == 0) level->light_angle = 360;
            _update_projection_angle();
        }
    }

  
    _update_mtx_mouse_pos();
  
    _hovering_on_window = false;
}
void Light_Page::draw() noexcept {
    ClearBackground(DARKGRAY);
    
    auto *level = ctx->get_selected_level();
    if (level == nullptr) return;

    auto &camera = ctx->get_camera();
    const auto &viewport = ctx->_textures->get_main_level_viewport();

    if (_should_redraw) {
        BeginTextureMode(ctx->_textures->geo_layer1.get());

        ClearBackground(WHITE);

        mr::draw::draw_geo_and_poles_layer(
            ctx->get_selected_level()->get_const_geo_matrix(),
            0,
            BLACK
        );

        EndTextureMode();

        //

        BeginTextureMode(ctx->_textures->geo_layer2.get());

        ClearBackground(WHITE);
    
        mr::draw::draw_geo_and_poles_layer(
          ctx->get_selected_level()->get_const_geo_matrix(),
          1,
          BLACK
        );
    
        EndTextureMode();

        //

        BeginTextureMode(ctx->_textures->geo_layer3.get());

        ClearBackground(WHITE);
    
        mr::draw::draw_geo_and_poles_layer(
          ctx->get_selected_level()->get_const_geo_matrix(),
          2,
          BLACK
        );
    
        EndTextureMode();

        //

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

        //

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

        //

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

        //

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

        // Compose

        BeginTextureMode(ctx->_textures->get_main_level_viewport());
        ClearBackground(Color{200, 200, 200, 255});

        const auto &color_shader = ctx->_shaders->white_remover_apply_color();
        const auto &bkg_shader = ctx->_shaders->white_remover_apply_alpha();

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

        //

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

        //

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

        //

        DrawTexture(ctx->_textures->props.get().texture, 0, 0, WHITE);

        EndTextureMode();

        _should_redraw = false;
    }
    
    if (ctx->_textures->light_editor.brushes().size() > 0) {
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) || IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
            const auto &lightmap = level->get_lightmap();
            const auto &shader = ctx->_shaders->white_remover_apply_color();
            const auto &texture = ctx->_textures->light_editor.brushes()[_brush_index].get();
            auto mouse_pos = GetScreenToWorld2D(GetMousePosition(), camera);
    
            BeginTextureMode(lightmap);
            BeginShaderMode(shader);
            SetShaderValueTexture(shader, GetShaderLocation(shader, "texture0"), texture);
            DrawTexturePro(
                texture,
                Rectangle{
                    0, 
                    0, 
                    static_cast<float>(texture.width), 
                    static_cast<float>(texture.height)
                },
                Rectangle{
                    _custom_rotation ? _brush_pinned_pos.x : mouse_pos.x + 300, 
                    _custom_rotation ? _brush_pinned_pos.y : mouse_pos.y + 300, 
                    _brush_width, 
                    _brush_height
                },
                {_half_brush_width, _half_brush_height}, 
                _brush_rotation, 
                _paint_mode == PAINT_SHADOWS_MODE ? BLACK : WHITE
            );
            EndShaderMode();
            EndTextureMode();
        }
    } 

    BeginMode2D(camera);
    DrawTexture(ctx->_textures->get_main_level_viewport().texture, 0, 0, WHITE);
    
    const auto &lightmap = level->get_lightmap();
    {
        const auto &shader = ctx->_shaders->apply_alpha_vflip();
    
        BeginShaderMode(shader);
        SetShaderValueTexture(shader, GetShaderLocation(shader, "texture0"), lightmap.texture);
        
        int alpha = 120;
        SetShaderValue(shader, GetShaderLocation(shader, "alpha"), &alpha, SHADER_UNIFORM_INT);
    
        DrawTexture(lightmap.texture, -300, -300, WHITE);
        EndShaderMode();
    }
    {
        const auto &shader = ctx->_shaders->white_remover_apply_color_vflip();
    
        BeginShaderMode(shader);
        SetShaderValueTexture(shader, GetShaderLocation(shader, "texture0"), lightmap.texture);
    
        DrawTextureV(
            lightmap.texture, 
            _projection_angle, 
            Color {0, 0, 0, 100}
        );
        EndShaderMode();
    }

    mr::draw::draw_double_frame(-300, -300, level->get_pixel_width() + 300, level->get_pixel_height() + 300);

    //

    if (ctx->_textures->light_editor.brushes().size() > 0) {
        const auto shader = ctx->_shaders->white_remover_apply_color();
        const auto &texture = ctx->_textures->light_editor.brushes()[_brush_index].get();

        auto mouse_pos = GetScreenToWorld2D(GetMousePosition(), camera);
    
        BeginShaderMode(shader);
        SetShaderValueTexture(shader, GetShaderLocation(shader, "texture0"), texture);
        DrawTexturePro(
            texture,
            Rectangle{
                0, 
                0, 
                static_cast<float>(texture.width), 
                static_cast<float>(texture.height)
            },
            Rectangle{
                _custom_rotation ? _brush_pinned_pos.x : mouse_pos.x, 
                _custom_rotation ? _brush_pinned_pos.y : mouse_pos.y, 
                _brush_width, 
                _brush_height
            },
            {_half_brush_width, _half_brush_height}, 
            _brush_rotation, 
            _paint_mode == PAINT_SHADOWS_MODE ? Color{255, 0, 0, 110} : Color{255, 120, 120, 110}
        );
        EndShaderMode();
    }
    
    EndMode2D();
}
void Light_Page::windows() noexcept {
    auto brushes_opened = ImGui::Begin("Brushes##LightEditorBrushes");
    _hovering_on_window |= ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows);

    auto *draw_list = ImGui::GetWindowDrawList();

    if (brushes_opened) {
        if (ImGui::BeginListBox("##List", ImGui::GetContentRegionAvail())) {
            
            size_t counter = 0;
            for (auto &brush : ctx->_textures->light_editor.brushes()) {
                std::string id("##");

                auto available_width = ImGui::GetContentRegionAvail().x;
                
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
                        counter == _brush_index 
                            ? ImVec4{0, 1, 0.1f, 0.77f} 
                            : (hovered ? ImVec4{1, 1, 1, 0.57f} :  ImVec4{0, 0, 0, 0})
                    )
                );
                ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.0f);
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10.0f);
                
                rlImGuiImageSize(brush.get_ptr(), available_width - 20, available_width - 20);
     
                if (ImGui::GetMouseClickedCount(ImGuiMouseButton_Left) && hovered) { _brush_index = counter; }
                
                counter++;
            }
            
            ImGui::EndListBox();
        }
    }

    ImGui::End();
    
    auto options_opened = ImGui::Begin("Options##LightEditorOptions");
    _hovering_on_window |= ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows);
    
    auto *level = ctx->get_selected_level();
    if (options_opened && level != nullptr) {
        ImGui::SetNextItemWidth(100);
        if (ImGui::InputInt("Angle", &level->light_angle)) {
            if (level->light_angle >= 360) level->light_angle = 0;
            if (level->light_angle <= 0) level->light_angle = 360;
        }

        ImGui::SetNextItemWidth(100);
        if (ImGui::InputInt("Flatness", &level->light_flatness)) {
            if (level->light_flatness > 10) level->light_flatness = 1;
            if (level->light_flatness < 1) level->light_flatness = 10;
        }

        ImGui::Spacing();

        auto button_space = ImGui::GetContentRegionAvail();
        button_space.y = 20;

        if (ImGui::Button("Reset Angle", button_space)) level->light_angle = 180;
        if (ImGui::Button("Reset Flatness", button_space)) level->light_flatness = 1;
    }
    
    ImGui::End();
}
void Light_Page::f3() const noexcept {
    auto f3 = ctx->f3_;

    f3->print("Brush ");
    f3->print(_brush_index, true);

    f3->print("Brush Width ");
    f3->print(_brush_width, true);

    f3->print("Brush Height ");
    f3->print(_brush_height, true);

    f3->print("Mode ");
    if (_paint_mode == PAINT_LIGHT_MODE) f3->print("Light", true);
    else f3->print("Shadow", true);

    const auto *level = ctx->get_selected_level();

    if (level != nullptr) {
        f3->print("Angle ");
        f3->print(level->light_angle, true);
        
        f3->print("Flatness ");
        f3->print(level->light_flatness, true);
    }
}

Light_Page::Light_Page(context *ctx) : 
    LevelPage(ctx), 
    _hovering_on_window(false),
    _should_redraw(true),
    _custom_rotation(false),
    _click_lock(false),
    _brush_index(0),
    _brush_width(200),
    _brush_height(200),
    _half_brush_width(_brush_width/2),
    _half_brush_height(_brush_height/2),
    _brush_growth_speed(3),
    _brush_growth_accelaration(0.1f),
    _brush_rotation(0),
    _paint_mode(0),
    _projection_angle(Vector2{0, 0}),
    _brush_pinned_pos(Vector2{0, 0})
{}
Light_Page::~Light_Page() {}

};