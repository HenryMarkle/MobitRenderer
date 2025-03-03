#include <queue>
#include <vector>
#include <thread>
#include <atomic>
#include <string>
#include <random>
#include <numeric>
#include <stdexcept>
#include <algorithm>

#if IS_DEBUG_BUILD
#include <iostream>
#endif

#include <raylib.h>

#include <spdlog/spdlog.h>

#include <MobitRenderer/vec.h>
#include <MobitRenderer/quad.h>
#include <MobitRenderer/dirs.h>
#include <MobitRenderer/draw.h>
#include <MobitRenderer/level.h>
#include <MobitRenderer/utils.h>
#include <MobitRenderer/state.h>
#include <MobitRenderer/matrix.h>
#include <MobitRenderer/renderer.h>
#include <MobitRenderer/definitions.h>

#define CONNECTION_VERTICAL static_cast<uint8_t>(0b00101)
#define CONNECTION_HORIZONTAL static_cast<uint8_t>(0b01010)
#define CONNECTION_CROSS static_cast<uint8_t>(0b01111)
#define CONNECTION_TRB static_cast<uint8_t>(0b00111)
#define CONNECTION_BLT static_cast<uint8_t>(0b01101)
#define CONNECTION_LTR static_cast<uint8_t>(0b01110)
#define CONNECTION_RBL static_cast<uint8_t>(0b01011)
#define CONNECTION_RB static_cast<uint8_t>(0b00011)
#define CONNECTION_BL static_cast<uint8_t>(0b01001)
#define CONNECTION_LT static_cast<uint8_t>(0b01100)
#define CONNECTION_TR static_cast<uint8_t>(0b00110)
#define CONNECTION_L static_cast<uint8_t>(0b01000)
#define CONNECTION_R static_cast<uint8_t>(0b00010)
#define CONNECTION_T static_cast<uint8_t>(0b00100)
#define CONNECTION_B static_cast<uint8_t>(0b00001)
#define CONNECTION_SLOPE_NW static_cast<uint8_t>(0b10000)
#define CONNECTION_SLOPE_NE static_cast<uint8_t>(0b11000)
#define CONNECTION_SLOPE_ES static_cast<uint8_t>(0b11100)
#define CONNECTION_SLOPE_SW static_cast<uint8_t>(0b11110)
#define CONNECTION_SINGLE static_cast<uint8_t>(0b00000)
#define CONNECTION_PLATFORM static_cast<uint8_t>(0b11111)
#define CONNECTION_GLASS static_cast<uint8_t>(0b10111)

namespace mr::renderer {

bool Renderer::_frame_render_materials_layer(uint8_t layer, int threshold) {
    if (threshold <= 0 || layer > 2) return true;

    bool done = false;

    switch (_material_progress) {
    case 0: done = true; _render_bricks_layer(layer); break;
    case 1: done = true; _render_chaotic_stone_layer(layer); break;
    case 2: done = true; _render_small_pipes_layer(layer); break;
    case 3: done = true; _render_trash_layer(layer); break;
    case 4: done = true; _render_standard_layer(layer); break;
    case 5: done = true; _render_concrete_layer(layer); break;
    default: return true;
    }

    if (done) {
        _material_progress_x = _material_progress_y = 0;
        _material_progress++;
        if (_material_progress < 6) return false;
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

void Renderer::_render_concrete_layer(u_int8_t layer) {
    if (layer > 2) return;

    const auto rt = LoadRenderTexture(_level->get_pixel_width(), _level->get_pixel_height());
    
    const auto *def = _materials->material("Concrete");
    if (def == nullptr) return;

    const auto *default_material = _materials->material(_level->default_material);

    auto *member = _castlibs->member("concreteTexture");
    if (member == nullptr) return;

    const auto &texture = member->get_loaded_texture();
    if (!member->is_loaded()) return;

    bool skip_default = def->get_name() != _level->default_material;

    const auto &mtx = _level->get_const_tile_matrix();
    const auto &geos = _level->get_const_geo_matrix();

    int flip = 1, highlight = 1, shadow = 1, thick = 1;
    float size[2] = { static_cast<float>(rt.texture.width), static_cast<float>(rt.texture.height) };

    Color red = {255, 0, 0, 255}, blue = {0, 0, 255, 255};

    BeginTextureMode(rt);
    ClearBackground(WHITE);

    for (int x = 0; x < columns; x++) {
        for (int y = 0; y < rows; y++) {
            auto mx = x + static_cast<int>(_camera->get_position().x/20);
            auto my = y + static_cast<int>(_camera->get_position().y/20);

            if (!mtx.is_in_bounds(mx, my, layer)) continue;

            const auto &geo = geos.get_const(static_cast<matrix_t>(mx), static_cast<matrix_t>(my), layer);

            if (!geo.is_solid() && !geo.is_slope()) continue;

            const auto &cell = mtx.get_const(
                static_cast<matrix_t>(mx), 
                static_cast<matrix_t>(my), 
                layer
            );

            if (cell.type != TileType::_default && cell.type != TileType::material) continue;
            if (cell.type == TileType::_default && skip_default) continue;
            if (cell.type == TileType::material && cell.material_def != def) continue;

            mr::draw::draw_geo_texture(texture, geo, x * 20.0f, y * 20.0f);
        }
    }
    
    EndTextureMode();

    BeginTextureMode(_layers[layer * 10]);
    
    BeginShaderMode(_bevel);
    SetShaderValueTexture(_bevel, _bevel_texture_loc, rt.texture);
    SetShaderValue(_bevel, _bevel_highlight_loc, &highlight, SHADER_UNIFORM_INT);
    SetShaderValue(_bevel, _bevel_shadow_loc, &shadow, SHADER_UNIFORM_INT);
    SetShaderValue(_bevel, _bevel_thick_loc, &thick, SHADER_UNIFORM_INT);
    SetShaderValue(_bevel, _bevel_vflip_loc, &flip, SHADER_UNIFORM_INT);
    SetShaderValueV(_bevel, _bevel_tex_size_loc, size, SHADER_UNIFORM_FLOAT, 2);
    DrawTexture(rt.texture, 0, 0, WHITE);
    EndShaderMode();
    EndTextureMode();

    for (int l = 1; l < 10; l++) {
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
            auto mx = x + static_cast<int>(_camera->get_position().x/20);
            auto my = y + static_cast<int>(_camera->get_position().y/20);

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

void Renderer::_render_small_pipes_layer(uint8_t layer) {
    /*
        Highly inefficient; Should list only the cells of the materials, 
        since it's not typically used excessively. 
    */

    if (layer > 2) return;

    auto *member = _castlibs->member("pipeTiles2");
    if (member == nullptr) return;

    const auto &texture = member->get_loaded_texture();
    if (!member->is_loaded()) return;

    const auto *def = _materials->material("Small Pipes");
    if (def == nullptr) return;

    size_t sublayer = layer * 10;

    const auto &mtx = _level->get_const_tile_matrix();
    const auto &geos = _level->get_const_geo_matrix();

    const auto get_connection = [this, &mtx, &geos, def, layer](int mx, int my){
        const auto *geo = geos.get_const_ptr(mx, my, layer);
        const auto *tile = geos.get_const_ptr(mx, my, layer);

        if (geo == nullptr || tile == nullptr) return static_cast<uint8_t>(0);
        if (geo->is_air()) return static_cast<uint8_t>(0);

        uint8_t connection = static_cast<uint8_t>(0b00000);

        if (!_is_material(mx, my, layer, def)) return static_cast<uint8_t>(0);

        switch (geo->type) {
        case GeoType::solid:
        {
            
            auto left   = _is_material(mx - 1, my    , layer, def);
            auto top    = _is_material(mx    , my - 1, layer, def);
            auto right  = _is_material(mx + 1, my    , layer, def);
            auto bottom = _is_material(mx    , my + 1, layer, def);

            if (
                left || 
                (
                    _rand.next(2) == 1 && _is_material(static_cast<matrix_t>(mx - 1), static_cast<matrix_t>(my), layer)
                )
            ) connection |= CONNECTION_L;

            if (
                top || 
                (
                    _rand.next(2) == 1 && _is_material(static_cast<matrix_t>(mx), static_cast<matrix_t>(my - 1), layer)
                )
            ) connection |= CONNECTION_T;
            
            if (
                right || 
                (
                    _rand.next(2) == 1 && _is_material(static_cast<matrix_t>(mx + 1), static_cast<matrix_t>(my), layer)
                )
            ) connection |= CONNECTION_R;
            
            if (
                bottom || 
                (
                    _rand.next(2) == 1 && _is_material(static_cast<matrix_t>(mx), static_cast<matrix_t>(my + 1), layer)
                )
            ) connection |= CONNECTION_B;
        }
        break;

        case GeoType::slope_nw: return CONNECTION_SLOPE_NW;
        case GeoType::slope_ne: return CONNECTION_SLOPE_NE;
        case GeoType::slope_es: return CONNECTION_SLOPE_ES;
        case GeoType::slope_sw: return CONNECTION_SLOPE_SW;
        case GeoType::platform: return CONNECTION_PLATFORM;
        case GeoType::glass: return CONNECTION_GLASS;

        default: return static_cast<uint8_t>(0);
        }

        return connection;
    };

    const auto get_src_pos = [](uint8_t connection){
        if (connection == CONNECTION_VERTICAL  ) return  1;
        if (connection == CONNECTION_HORIZONTAL) return  3;
        if (connection == CONNECTION_CROSS     ) return  5;
        if (connection == CONNECTION_TRB       ) return  7;
        if (connection == CONNECTION_BLT       ) return  9;
        if (connection == CONNECTION_LTR       ) return 11;
        if (connection == CONNECTION_RBL       ) return 13;
        if (connection == CONNECTION_RB        ) return 15;
        if (connection == CONNECTION_BL        ) return 17;
        if (connection == CONNECTION_LT        ) return 19;
        if (connection == CONNECTION_TR        ) return 21;
        if (connection == CONNECTION_L         ) return 23;
        if (connection == CONNECTION_R         ) return 25;
        if (connection == CONNECTION_T         ) return 27;
        if (connection == CONNECTION_B         ) return 29;
        if (connection == CONNECTION_SLOPE_NW  ) return 31; // SlopeNW
        if (connection == CONNECTION_SLOPE_NE  ) return 32; // SlopeNE
        if (connection == CONNECTION_SLOPE_ES  ) return 35; // SlopeES
        if (connection == CONNECTION_SLOPE_SW  ) return 37; // SlopeSW
        if (connection == CONNECTION_SINGLE    ) return 39;
        if (connection == CONNECTION_PLATFORM  ) return 41; // Platform
        if (connection == CONNECTION_GLASS     ) return 43; // Glass

        return 0;
    };

    for (int x = 0; x < columns; x++) {
        for (int y = 0; y < rows; y++) {
            int mx = x + static_cast<int>(_camera->get_position().x/20);
            int my = y + static_cast<int>(_camera->get_position().y/20);
        
            if (!mtx.is_in_bounds(mx, my, layer)) continue;

            auto connection = get_connection(mx, my);
            
            if (connection == 0) continue;

            auto pos = get_src_pos(connection);

            BeginTextureMode(_layers[sublayer + 5]);
            DrawRectangleLinesEx(
                {x * 20.0f, y * 20.0f, 20.0f, 20.0f},
                2,
                {0, 255, 0, 255}
            );
            EndTextureMode();

            auto src = Rectangle {
                pos * 20.0f + 1.0f,
                (_rand.next(4)*2 + 1) * 20.0f + 1.0f,
                20.0f,
                20.0f
            };

            BeginTextureMode(_layers[sublayer + 2]);
            BeginShaderMode(_white_remover);
            SetShaderValueTexture(_white_remover, _white_remover_texture_loc, texture);
            DrawTexturePro(
                texture,
                src,
                {x*20.0f, y*20.0f, 20.0f, 20.0f},
                {0,0},
                0,
                WHITE
            );
            EndShaderMode();
            EndTextureMode();

            BeginTextureMode(_layers[sublayer + 3]);
            BeginShaderMode(_white_remover);
            SetShaderValueTexture(_white_remover, _white_remover_texture_loc, texture);
            DrawTexturePro(
                texture,
                src,
                {x*20.0f, y*20.0f, 20.0f, 20.0f},
                {0,0},
                0,
                WHITE
            );
            EndShaderMode();
            EndTextureMode();

            BeginTextureMode(_layers[sublayer + 7]);
            BeginShaderMode(_white_remover);
            SetShaderValueTexture(_white_remover, _white_remover_texture_loc, texture);
            DrawTexturePro(
                texture,
                src,
                {x*20.0f, y*20.0f, 20.0f, 20.0f},
                {0,0},
                0,
                WHITE
            );
            EndShaderMode();
            EndTextureMode();

            BeginTextureMode(_layers[sublayer + 8]);
            BeginShaderMode(_white_remover);
            SetShaderValueTexture(_white_remover, _white_remover_texture_loc, texture);
            DrawTexturePro(
                texture,
                src,
                {x*20.0f, y*20.0f, 20.0f, 20.0f},
                {0,0},
                0,
                WHITE
            );
            EndShaderMode();
            EndTextureMode();
        }
    }
}

void Renderer::_render_trash_layer(uint8_t layer) {
    if (layer > 2) return;

    auto *member = _castlibs->member("trashTiles3");
    if (member == nullptr) return;

    auto *member2 = _castlibs->member("assortedTrash");

    const auto &texture = member->get_loaded_texture();
    if (!member->is_loaded()) return;

    Texture2D texture2;
    if (member2 != nullptr) texture2 = member2->get_loaded_texture();

    const auto *def = _materials->material("Trash");
    if (def == nullptr) return;

    size_t sublayer = layer * 10;

    Color colors[3] = {
        {255, 0, 0, 255},
        {0, 255, 0, 255},
        {0, 0, 255, 255}
    };

    const auto &mtx = _level->get_const_tile_matrix();
    const auto &geos = _level->get_const_geo_matrix();

    const auto get_connection = [this, &mtx, &geos, def, layer](int mx, int my){
        const auto *geo = geos.get_const_ptr(mx, my, layer);
        const auto *tile = geos.get_const_ptr(mx, my, layer);

        if (geo == nullptr || tile == nullptr) return static_cast<uint8_t>(0);
        if (geo->is_air()) return static_cast<uint8_t>(0);

        uint8_t connection = static_cast<uint8_t>(0b00000);

        if (!_is_material(mx, my, layer, def)) return static_cast<uint8_t>(0);

        switch (geo->type) {
        case GeoType::solid:
        {
            
            auto left   = _is_material(mx - 1, my    , layer, def);
            auto top    = _is_material(mx    , my - 1, layer, def);
            auto right  = _is_material(mx + 1, my    , layer, def);
            auto bottom = _is_material(mx    , my + 1, layer, def);

            if (
                left || 
                (
                    _rand.next(2) == 1 && _is_material(static_cast<matrix_t>(mx - 1), static_cast<matrix_t>(my), layer)
                )
            ) connection |= CONNECTION_L;

            if (
                top || 
                (
                    _rand.next(2) == 1 && _is_material(static_cast<matrix_t>(mx), static_cast<matrix_t>(my - 1), layer)
                )
            ) connection |= CONNECTION_T;
            
            if (
                right || 
                (
                    _rand.next(2) == 1 && _is_material(static_cast<matrix_t>(mx + 1), static_cast<matrix_t>(my), layer)
                )
            ) connection |= CONNECTION_R;
            
            if (
                bottom || 
                (
                    _rand.next(2) == 1 && _is_material(static_cast<matrix_t>(mx), static_cast<matrix_t>(my + 1), layer)
                )
            ) connection |= CONNECTION_B;
        }
        break;

        case GeoType::slope_nw: return CONNECTION_SLOPE_NW;
        case GeoType::slope_ne: return CONNECTION_SLOPE_NE;
        case GeoType::slope_es: return CONNECTION_SLOPE_ES;
        case GeoType::slope_sw: return CONNECTION_SLOPE_SW;
        case GeoType::platform: return CONNECTION_PLATFORM;
        case GeoType::glass: return CONNECTION_GLASS;

        default: return static_cast<uint8_t>(0);
        }

        return connection;
    };

    const auto get_src_pos = [](uint8_t connection){
        if (connection == CONNECTION_VERTICAL  ) return  1;
        if (connection == CONNECTION_HORIZONTAL) return  3;
        if (connection == CONNECTION_CROSS     ) return  5;
        if (connection == CONNECTION_TRB       ) return  7;
        if (connection == CONNECTION_BLT       ) return  9;
        if (connection == CONNECTION_LTR       ) return 11;
        if (connection == CONNECTION_RBL       ) return 13;
        if (connection == CONNECTION_RB        ) return 15;
        if (connection == CONNECTION_BL        ) return 17;
        if (connection == CONNECTION_LT        ) return 19;
        if (connection == CONNECTION_TR        ) return 21;
        if (connection == CONNECTION_L         ) return 23;
        if (connection == CONNECTION_R         ) return 25;
        if (connection == CONNECTION_T         ) return 27;
        if (connection == CONNECTION_B         ) return 29;
        if (connection == CONNECTION_SLOPE_NW  ) return 31; // SlopeNW
        if (connection == CONNECTION_SLOPE_NE  ) return 32; // SlopeNE
        if (connection == CONNECTION_SLOPE_ES  ) return 35; // SlopeES
        if (connection == CONNECTION_SLOPE_SW  ) return 37; // SlopeSW
        if (connection == CONNECTION_SINGLE    ) return 39;
        if (connection == CONNECTION_PLATFORM  ) return 41; // Platform
        if (connection == CONNECTION_GLASS     ) return 43; // Glass

        return 0;
    };

    
    for (int x = 0; x < columns; x++) {
        for (int y = 0; y < rows; y++) {
            int mx = x + static_cast<int>(_camera->get_position().x/20);
            int my = y + static_cast<int>(_camera->get_position().y/20);
        
            if (!mtx.is_in_bounds(mx, my, layer)) continue;

            auto connection = get_connection(mx, my);
            
            if (connection == 0) continue;

            auto pos = get_src_pos(connection);

            auto src = Rectangle {
                pos * 20.0f + 1.0f,
                (_rand.next(4)*2 + 1) * 20.0f + 1.0f,
                20.0f,
                20.0f
            };

            BeginTextureMode(_layers[sublayer + 2]);
            BeginShaderMode(_white_remover);
            SetShaderValueTexture(_white_remover, _white_remover_texture_loc, texture);
            mr::draw::draw_texture(texture, src, Quad(Rectangle{x*20.0f, y*20.0f, 20.0f, 20.0f}).rotated(_rand.next(360)));
            EndShaderMode();
            EndTextureMode();

            BeginTextureMode(_layers[sublayer + 3]);
            BeginShaderMode(_white_remover);
            SetShaderValueTexture(_white_remover, _white_remover_texture_loc, texture);
            mr::draw::draw_texture(texture, src, Quad(Rectangle{x*20.0f, y*20.0f, 20.0f, 20.0f}).rotated(_rand.next(360)));
            EndShaderMode();
            EndTextureMode();

            BeginTextureMode(_layers[sublayer + 7]);
            BeginShaderMode(_white_remover);
            SetShaderValueTexture(_white_remover, _white_remover_texture_loc, texture);
            mr::draw::draw_texture(texture, src, Quad(Rectangle{x*20.0f, y*20.0f, 20.0f, 20.0f}).rotated(_rand.next(360)));
            EndShaderMode();
            EndTextureMode();

            BeginTextureMode(_layers[sublayer + 8]);
            BeginShaderMode(_white_remover);
            SetShaderValueTexture(_white_remover, _white_remover_texture_loc, texture);
            mr::draw::draw_texture(texture, src, Quad(Rectangle{x*20.0f, y*20.0f, 20.0f, 20.0f}).rotated(_rand.next(360)));
            EndShaderMode();
            EndTextureMode();


            
            for (int l = 0; l < 3; l++) {
                const auto s = sublayer + _rand.next(10);
                const auto srcx = _rand.next(48);
                const auto middle = Vector2{
                    x * 20.0f + 1.0f + _rand.next(18) * -1 * (_rand.next(2) == 1),
                    y * 20.0f        + _rand.next(18) * -1 * (_rand.next(2) == 1)
                };
                const auto src = Rectangle {
                    srcx * 50.0f,
                    0,
                    50.0f,
                    50.0f
                };
                const auto target = Rectangle {
                    middle.x,
                    middle.y,
                    50.0f,
                    50.0f
                };

                if (member2 == nullptr || !member2->is_loaded()) continue;


                BeginTextureMode(_layers[s]);
                BeginShaderMode(_ink);
                SetShaderValueTexture(_ink, _ink_texture_loc, texture2);
                mr::draw::draw_texture(
                    texture2,
                    src,
                    Quad(target).rotated(_rand.next(360))
                );
                EndTextureMode();
            }
        }
    }
}

}