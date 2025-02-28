#include <queue>
#include <vector>
#include <thread>
#include <atomic>
#include <string>
#include <random>
#include <numeric>
#include <stdexcept>
#include <algorithm>

#include <raylib.h>

#include <spdlog/spdlog.h>

#include <MobitRenderer/vec.h>
#include <MobitRenderer/dirs.h>
#include <MobitRenderer/draw.h>
#include <MobitRenderer/level.h>
#include <MobitRenderer/utils.h>
#include <MobitRenderer/state.h>
#include <MobitRenderer/matrix.h>
#include <MobitRenderer/renderer.h>
#include <MobitRenderer/definitions.h>

namespace mr::renderer {

bool Renderer::_frame_render_materials_layer(uint8_t layer, int threshold) {
    if (threshold <= 0 || layer > 2) return true;

    bool done = false;

    switch (_material_progress) {
    case 0: done = true; _render_bricks_layer(layer); break;
    case 1: done = true; _render_standard_layer(layer); break;
    case 2: done = true; _render_chaotic_stone_layer(layer); break;
    default: return true;
    }

    if (done) {
        _material_progress_x = _material_progress_y = 0;
        _material_progress++;
        if (_material_progress < 3) return false;
    }
    else return false;

    
    
    return true;
}

bool Renderer::_frame_draw_materials_layer_unified(
    std::queue<Render_TileCell> *cells,
    uint8_t layer,
    int threshold
) {
    if (cells == nullptr || cells->empty() || layer > 2) return true;

    int progress = 0;

    // if (_material_progress == 0) {
    //     BeginTextureMode(_material_canvas);
    //     ClearBackground(WHITE);
    //     EndTextureMode();
    // }

    BeginTextureMode(_material_canvas);
    // BeginTextureMode(_layers[layer * 10]);
    while (progress < threshold && !cells->empty()) {
        const auto &material = cells->front();
    
        const auto *def = material.cell->material_def;
        mr::CastMember *member = nullptr;
        Texture2D texture;

        const auto x = material.x * 20.0f - _camera->get_position().x;
        const auto y = material.y * 20.0f - _camera->get_position().y;
        
        if (def == nullptr) goto next;

        if (def->get_name() == "Standard") {
            DrawRectangle(x, y, 20, 20, Color{0, 255, 0, 255});
            goto next;
        } 
        else if (def->get_name() == "Bricks") {
            member = _castlibs->member("bricksTexture");
            if (member == nullptr) goto next;
            texture = member->get_loaded_texture();
            if (!member->is_loaded()) goto next;
            DrawTextureRec(texture, {0,0,20,20}, {x, y}, WHITE);
        }
        else {
            member = _castlibs->member(material.cell->material_def->get_name()+"Texture");
            if (member == nullptr) goto next;
    
            texture = member->get_loaded_texture();
            if (!member->is_loaded()) goto next;
    
            DrawTexture(texture, x, y, WHITE);
        }
        
        next:
        cells->pop();
        progress++;
        // _material_progress++;
    }
    EndTextureMode();

    if (cells->empty()) {

        float size[2] = { 
            static_cast<float>(_material_canvas.texture.width), 
            static_cast<float>(_material_canvas.texture.height) 
        };
        int highlight = 1, shadow = 1, vflip = 0, thick = 1;

        BeginTextureMode(_layers[layer * 10]);
        BeginShaderMode(_bevel);
        SetShaderValueTexture(_bevel, _bevel_texture_loc, _material_canvas.texture);
        SetShaderValueV(_bevel, _bevel_tex_size_loc, size, SHADER_UNIFORM_FLOAT, 2);
        SetShaderValue(_bevel, _bevel_thick_loc, &thick, SHADER_UNIFORM_INT);
        SetShaderValue(_bevel, _bevel_highlight_loc, &highlight, SHADER_UNIFORM_INT);
        SetShaderValue(_bevel, _bevel_shadow_loc, &shadow, SHADER_UNIFORM_INT);
        SetShaderValue(_bevel, _bevel_vflip_loc, &vflip, SHADER_UNIFORM_INT);

        BeginShaderMode(_white_remover);
        SetShaderValueTexture(_white_remover, _white_remover_texture_loc, _material_canvas.texture);
        
        DrawTexture(_material_canvas.texture, 0, 0, WHITE);
    
        EndShaderMode();
        EndTextureMode();
    
        for (int x = 1; x < 10; x++) {
            BeginTextureMode(_layers[layer * 10 + x]);
            BeginShaderMode(_white_remover);
            SetShaderValueTexture(_white_remover, _white_remover_texture_loc, _material_canvas.texture);

            DrawTexture(_material_canvas.texture, 0, 0, WHITE);
            
            EndShaderMode();
            EndTextureMode();
        }

        return true;
    }

    return false;
}

bool Renderer::_frame_render_bricks_layer(uint8_t layer, int threshold) {
    if (layer > 2) return true;

    auto *member = _castlibs->member("bricksTexture");
    if (member == nullptr) return true;

    const auto &texture = member->get_loaded_texture();
    if (!member->is_loaded()) return true;

    bool skip_default = _level->default_material != "Bricks";
    const MaterialDef *def = _materials->material("Bricks");
    int progress = 0;

    const auto &mtx = _level->get_const_tile_matrix();
    const auto &geos = _level->get_const_geo_matrix();

    for (_material_progress_x; _material_progress_x < columns; _material_progress_x++) {
        for (_material_progress_y = 0; _material_progress_y < rows; _material_progress_y++) {
            const int mx = _material_progress_x;
            const int my = _material_progress_y;

            if (!mtx.is_in_bounds(mx, my, layer)) continue;

            const auto &cell = mtx.get_const(static_cast<matrix_t>(mx), static_cast<matrix_t>(my), layer);
            if (!geos.get_const(static_cast<matrix_t>(mx), static_cast<matrix_t>(my), layer).is_solid()) continue;

            if (cell.type != TileType::_default && cell.type != TileType::material) continue;
            if (cell.type == TileType::_default && skip_default) continue;
            if (cell.type == TileType::material && cell.material_def != def) continue;

            const float x = _material_progress_x*20.0f;
            const float y = _material_progress_y*20.0f;

            for (int l = 0; l < 10; l++) {
                BeginTextureMode(_layers[layer * 10 + l]);
                DrawTexture(texture, x, y, WHITE);
                EndTextureMode();
            }

            if (progress++ >= threshold) return false;
        }
    }

    return true;
}

void Renderer::_render_bricks_layer(uint8_t layer) {
    if (layer > 2) return;

    auto *member = _castlibs->member("bricksTexture");
    if (member == nullptr) return;

    const auto &texture = member->get_loaded_texture();
    if (!member->is_loaded()) return;

    bool skip_default = _level->default_material != "Bricks";
    const MaterialDef *def = _materials->material("Bricks");

    const auto &mtx = _level->get_const_tile_matrix();
    const auto &geos = _level->get_const_geo_matrix();

    auto rt = LoadRenderTexture(_level->get_pixel_width(), _level->get_pixel_height());

    BeginTextureMode(rt);
    ClearBackground(WHITE);

    for (int x = 0; x < columns; x++) {
        for (int y = 0; y < rows; y++) {
            int mx = x + static_cast<int>(_camera->get_position().x/20);
            int my = y + static_cast<int>(_camera->get_position().y/20);

            if (!mtx.is_in_bounds(mx, my, layer)) continue;

            if (!geos.get_const(static_cast<matrix_t>(mx), static_cast<matrix_t>(my), layer).is_solid()) continue;
            const auto &cell = mtx.get_const(static_cast<matrix_t>(mx), static_cast<matrix_t>(my), layer);

            if (cell.type != TileType::_default && cell.type != TileType::material) continue;
            if (cell.type == TileType::_default && skip_default) continue;
            if (cell.type == TileType::material && cell.material_def != def) continue;

            DrawTexturePro(
                texture,
                Rectangle{ 0, 0, 20, 20 },
                Rectangle{ x * 20.0f, y * 20.0f, 20.0f, 20.0f },
                Vector2{ 0, 0 },
                0,
                WHITE
            );

        }
    }
    EndTextureMode();

    for (int l = 0; l < 10; l++) {
        BeginTextureMode(_layers[layer * 10 + l]);
        BeginShaderMode(_white_remover_vflip);
        SetShaderValueTexture(_white_remover_vflip, _white_remover_vflip_texture_loc, rt.texture);
        DrawTexture(rt.texture, 0, 0, WHITE);
        EndShaderMode();
        EndTextureMode();
    }

    UnloadRenderTexture(rt);
}

void Renderer::_render_standard_layer(uint8_t layer) {
    if (layer > 2) return;

    const auto rt = LoadRenderTexture(_level->get_pixel_width(), _level->get_pixel_height());
    
    const auto *def = _materials->material("Standard");
    if (def == nullptr) return;

    bool skip_default = def->get_name() != _level->default_material;

    const auto &mtx = _level->get_const_tile_matrix();
    const auto &geos = _level->get_const_geo_matrix();

    BeginTextureMode(rt);
    ClearBackground(WHITE);

    for (int x = 0; x < columns; x++) {
        for (int y = 0; y < rows; y++) {
            auto mx = x - static_cast<int>(_camera->get_position().x/20);
            auto my = y - static_cast<int>(_camera->get_position().y/20);

            if (!mtx.is_in_bounds(mx, my, layer)) continue;
            if (!geos.get_const(static_cast<matrix_t>(mx), static_cast<matrix_t>(my), layer).is_solid()) continue;

            const auto &cell = mtx.get_const(
                static_cast<matrix_t>(mx), 
                static_cast<matrix_t>(my), 
                layer
            );

            if (cell.type != TileType::_default && cell.type != TileType::material) continue;
            if (cell.type == TileType::_default && skip_default) continue;
            if (cell.type == TileType::material && cell.material_def != def) continue;

            DrawRectangle(mx * 20, my * 20, 20, 20, Color{0, 255, 0, 255});
        }
    }
    
    EndTextureMode();

    BeginTextureMode(_layers[layer * 10]);
    
    BeginShaderMode(_white_remover);
    SetShaderValueTexture(_white_remover, _white_remover_texture_loc, rt.texture);
    DrawTexture(rt.texture, 0, 0, WHITE);
    EndShaderMode();

    EndTextureMode();

    UnloadRenderTexture(rt);
}

void Renderer::_render_chaotic_stone_layer(uint8_t layer) {
    if (layer > 2) return;

    const auto rt = LoadRenderTexture(_level->get_pixel_width(), _level->get_pixel_height());
    
    const auto *def = _materials->material("Chaotic Stone");
    if (def == nullptr) return;

    auto *smallstone = _tiles->tile("Small Stone");
    auto *squarestone = _tiles->tile("Square Stone");

    if (smallstone == nullptr || squarestone == nullptr) return;

    const auto &smalltexture = smallstone->get_loaded_texture();
    const auto &squaretexture = squarestone->get_loaded_texture();

    if (!smallstone->is_texture_loaded()) return;
    if (!squarestone->is_texture_loaded()) return;

    bool skip_default = def->get_name() != _level->default_material;

    const auto &mtx = _level->get_const_tile_matrix();
    const auto &geos = _level->get_const_geo_matrix();

    // true means taken, false means free
    Matrix<bool> space(columns, rows, 1);

    const auto fitsbig = [&](matrix_t x, matrix_t y, matrix_t mx, matrix_t my) {
        const auto *tl_geo = geos.get_const_ptr(mx, my, layer);
        const auto *tl_tile = mtx.get_const_ptr(mx, my, layer);

        const auto *tr_geo = geos.get_const_ptr(mx + 1, my, layer);
        const auto *tr_tile = mtx.get_const_ptr(mx + 1, my, layer);

        const auto *br_geo = geos.get_const_ptr(mx + 1, my + 1, layer);
        const auto *br_tile = mtx.get_const_ptr(mx + 1, my + 1, layer);

        const auto *bl_geo = geos.get_const_ptr(mx, my + 1, layer);
        const auto *bl_tile = mtx.get_const_ptr(mx, my + 1, layer);

        bool tl_taken = 
            space.get_copy(x, y, 0) ||
            tl_geo == nullptr || 
            tl_tile == nullptr || 
            (tl_tile->type != TileType::material && tl_tile->type != TileType::material) ||
            !tl_geo->is_solid();

        bool tr_taken = 
            space.get_copy(x + 1, y, 0) ||
            tr_geo == nullptr || 
            tr_tile == nullptr || 
            (tr_tile->type != TileType::material && tr_tile->type != TileType::material) ||
            !tr_geo->is_solid();

        bool bl_taken = 
            space.get_copy(x, y + 1, 0) ||
            bl_geo == nullptr || 
            bl_tile == nullptr || 
            (bl_tile->type != TileType::material && bl_tile->type != TileType::material) ||
            !bl_geo->is_solid();

        bool br_taken = 
            space.get_copy(x + 1, y + 1, 0) ||
            br_geo == nullptr || 
            br_tile == nullptr || 
            (br_tile->type != TileType::material && br_tile->type != TileType::material) ||
            !br_geo->is_solid();


        if (tl_tile->type == TileType::material && tl_tile->material_def != def) tl_taken = true;
        else if (tl_tile->type == TileType::_default && skip_default) tl_taken = true;

        if (tr_tile->type == TileType::material && tr_tile->material_def != def) tr_taken = true;
        else if (tr_tile->type == TileType::_default && skip_default) tr_taken = true;

        if (br_tile->type == TileType::material && br_tile->material_def != def) br_taken = true;
        else if (br_tile->type == TileType::_default && skip_default) br_taken = true;

        if (bl_tile->type == TileType::material && bl_tile->material_def != def) bl_taken = true;
        else if (bl_tile->type == TileType::_default && skip_default) bl_taken = true;

        space.set_noexcept(x    , y    , 0, tl_taken);
        space.set_noexcept(x + 1, y    , 0, tr_taken);
        space.set_noexcept(x    , y + 1, 0, bl_taken);
        space.set_noexcept(x + 1, y + 1, 0, br_taken);

        return !tr_taken && !br_taken && !bl_taken && !tl_taken;
    };

    const auto setbig = [&space](matrix_t x, matrix_t y) {
        space.set_noexcept(x    , y    , 0, true);
        space.set_noexcept(x + 1, y    , 0, true);
        space.set_noexcept(x    , y + 1, 0, true);
        space.set_noexcept(x + 1, y + 1, 0, true);
    };

    const float small_width = smallstone->calculate_width();
    const float small_height = smallstone->calculate_height();

    const float square_width = squarestone->calculate_width();
    const float square_height = squarestone->calculate_height();

    BeginTextureMode(rt);
    ClearBackground(WHITE);
    BeginShaderMode(_white_remover);

    for (int x = 0; x < columns; x++) {
        for (int y = 0; y < rows; y++) {
            auto mx = x + static_cast<int>(_camera->get_position().x/20);
            auto my = y + static_cast<int>(_camera->get_position().y/20);

            if (!mtx.is_in_bounds(mx, my, layer)) continue;
            
            const auto &geo = geos.get_const(static_cast<matrix_t>(mx), static_cast<matrix_t>(my), layer);
            
            if (!geo.is_solid()) {
                space.set_noexcept(x, y, 0, true);
                continue;
            }

            const auto &cell = mtx.get_const(
                static_cast<matrix_t>(mx), 
                static_cast<matrix_t>(my), 
                layer
            );

            if (space.get_copy(x, y, 0)) continue;

            if (cell.type != TileType::material && cell.type != TileType::_default) {
                space.set_noexcept(x, y, 0, true);
                continue;
            }
            if ((cell.type == TileType::material && cell.material_def != def) || (cell.type == TileType::_default && skip_default)) {
                space.set_noexcept(x, y, 0, true);
                continue;
            }

            if (fitsbig(x, y, mx, my) && _rand.next(2) == 1) {
                SetShaderValueTexture(_white_remover, _white_remover_texture_loc, squaretexture);
                DrawTexturePro(
                    squaretexture,
                    {square_width * _rand.next(squarestone->get_rnd()),0,square_width, square_height},
                    {
                        (x - squarestone->get_buffer()) * 20.0f, 
                        (y - squarestone->get_buffer()) * 20.0f, 
                        square_width, 
                        square_height
                    },
                    {0,0},
                    0,
                    WHITE
                );
                setbig(x, y);
            } else {
                SetShaderValueTexture(_white_remover, _white_remover_texture_loc, smalltexture);
                DrawTexturePro(
                    smalltexture,
                    {small_width * _rand.next(smallstone->get_rnd()),0,small_width, small_height},
                    {
                        (x - smallstone->get_buffer()) * 20.0f, 
                        (y - smallstone->get_buffer()) * 20.0f, 
                        small_width, 
                        small_height
                    },
                    {0,0},
                    0,
                    WHITE
                );
                space.set_noexcept(x, y, 0, true);
            }
        }
    }
    EndShaderMode();
    EndTextureMode();

    for (int l = 0; l < 10; l++) {
        BeginTextureMode(_layers[layer * 10 + l]);
        
        BeginShaderMode(_white_remover_vflip);
        SetShaderValueTexture(_white_remover_vflip, _white_remover_vflip_texture_loc, rt.texture);
        DrawTexture(rt.texture, 0, 0, WHITE);
        EndShaderMode();
    
        EndTextureMode();
    }

    UnloadRenderTexture(rt);
}

}