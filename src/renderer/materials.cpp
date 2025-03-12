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


namespace mr::renderer {

bool Renderer::_frame_render_materials_layer(uint8_t layer, int threshold) {
    if (threshold <= 0 || layer > 2) return true;

    bool done = false;

    switch (_material_progress) {
    case 0: done = _frame_render_unified_layer(layer, threshold); break;
    case 1: done = true; _render_chaotic_stone_layer(layer); break;
    case 2: done = _frame_render_pipe_layer(layer, threshold); break;
    default: return true;
    }

    if (!done) return false;

    _material_progress_x = _material_progress_y = 0;
    _material_progress++;

    return _material_progress >= 3;
}

bool Renderer::_frame_render_unified_layer(uint8_t layer, int threshold) {
    if (layer > 2 || threshold <= 0) return true;

    auto &queue = _materials_to_render[_camera_index][layer][0];
    int progress = 0;
    uint8_t sublayer = layer * 10;
    auto *default_material = _materials->material(_level->default_material);
    MaterialDef *def = nullptr;
    Rectangle rect;
    CastMember *tileset = nullptr, *mat_texture = nullptr;

    Texture2D ts_texture, texture;

    std::pair<ivec2, ivec2> profl;
    int gt_at_v = 0, gt_at_h = 0;
    Rectangle pst_rect;

    while (progress < threshold && !queue.empty()) {
        
        const auto &cell = queue.front();

        if (cell.geo.is_air()) goto skip;
        if (
            (cell.tile->type == TileType::material && cell.tile->material_def == nullptr) || 
            (
                cell.tile->type == TileType::_default && 
                (
                    default_material == nullptr || default_material->get_type() != MaterialRenderType::unified
                )
            )
        ) goto skip;

        def = cell.tile->type == TileType::material ? cell.tile->material_def : default_material;

        if (
            def->get_name() == "Concrete" || 
            def->get_name() == "RainStone" || 
            def->get_name() == "Bricks" || 
            def->get_name() == "Tiny Signs" || 
            def->get_name() == "Cliff" ||
            def->get_name() == "Non-Slip Metal" ||
            def->get_name() == "BulkMetal" || 
            def->get_name() == "MassiveBulkMetal" || 
            def->get_name() == "Asphalt")
        {
            mat_texture = _castlibs->member(def->get_name() + "Texture");
            if (mat_texture == nullptr) continue;
            texture = mat_texture->get_loaded_texture();
            if (!mat_texture->is_loaded()) continue;

            if (cell.geo.is_solid()) {
                BeginTextureMode(_layers[sublayer]);
                BeginShaderMode(_white_remover);
                SetShaderValueTexture(_white_remover, _white_remover_texture_loc, texture);
                DrawTexturePro(
                    texture,
                    Rectangle {
                        (cell.x * 20) % texture.width * 1.0f,
                        (cell.y * 20) % texture.height * 1.0f,
                        20.0f,
                        20.0f
                    },
                    Rectangle {
                        cell.x * 20.0f,
                        cell.y * 20.0f,
                        20.0f,
                        20.0f
                    },
                    Vector2 {0, 0},
                    0,
                    WHITE
                );
                EndShaderMode();
                EndTextureMode();
            }
        }

        rect = { cell.x * 20.0f, cell.y * 20.0f, 20.0f, 20.0f };
        
        if (def->get_name() == "Scaffolding") tileset = _castlibs->member("ScaffoldingDR");
        else if (def->get_name() == "Invisible") tileset = _castlibs->member("Superstructure");
        else tileset = _castlibs->member("tileSet" + def->get_name());

        if (tileset == nullptr) continue;
        ts_texture = tileset->get_loaded_texture();
        if (!tileset->is_loaded()) continue;

        if (cell.geo.is_solid()) {

            for (int f = 1; f <= 4; f++) {
                switch (f) {
                case 1: 
                    profl = { ivec2{-1, 0}, ivec2{0, -1} }; 
                    gt_at_v = 2; 
                    pst_rect = {
                        rect.x,
                        rect.y,
                        rect.width - 10,
                        rect.height - 10
                    }; 
                break;
                case 2: 
                    profl = { ivec2{ 1, 0}, ivec2{0, -1} }; 
                    gt_at_v = 4; 
                    pst_rect = {
                        rect.x + 10,
                        rect.y,
                        rect.width - 10,
                        rect.height - 10
                    }; 
                break;
                case 3:
                    profl = { ivec2{ 1, 0}, ivec2{0, 1} }; 
                    gt_at_v = 6; 
                    pst_rect = {
                        rect.x + 10,
                        rect.y + 10,
                        rect.width - 10,
                        rect.height - 10
                    }; 
                break;
                case 4:
                    profl = { ivec2{-1, 0}, ivec2{0, 1} }; 
                    gt_at_v = 8; 
                    pst_rect = {
                        rect.x,
                        rect.y + 10,
                        rect.width - 10,
                        rect.height - 10
                    };
                break;
                }

                uint8_t id = 0;

                auto first = _is_material(
                    cell.mx + profl.first.x, 
                    cell.my + profl.first.y, 
                    layer, 
                    def
                );
                auto second = _is_material(
                    cell.mx + profl.second.x, 
                    cell.my + profl.second.y, 
                    layer, 
                    def
                );
            
                if (first) id |= 0b00000010;
                if (second) id |= 0b00000001;

                if (id == 3) {
                    if (
                        _is_material(
                            cell.mx + profl.first.x + profl.second.x,
                            cell.my + profl.first.y + profl.second.y,
                            layer,
                            def
                        )
                    ) {
                        gt_at_h = 10;
                        gt_at_v = 2;
                    } else {
                        gt_at_h = 8;
                    }
                } else {
                    // if (id == 0 || id == 1 || id == 3) gt_at_h = id * 2;
                    // else gt_at_h = 0;
                    switch (id) {
                        case 0b00000000: gt_at_h = 2; break;
                        case 0b00000001: gt_at_h = 4; break;
                        case 0b00000010: gt_at_h = 6; break;
                        default: gt_at_h = 0; break;
                    }
                }

                if (gt_at_h == 4) {
                    if (gt_at_v == 6) {
                        gt_at_v = 4;
                    } else if (gt_at_v == 8) {
                        gt_at_v = 2;
                    }
                } else if (gt_at_h == 6) {
                    if (gt_at_v == 4 || gt_at_v == 8) {
                        gt_at_v -= 2;
                    }
                }

                auto gt_rect = Rectangle {
                    (gt_at_h - 1)*10.0f,
                    (gt_at_v - 1)*10.0f,
                    10.0f, 10.0f
                };

                BeginTextureMode(_layers[sublayer]);
                BeginShaderMode(_white_remover);
                SetShaderValueTexture(_white_remover, _white_remover_texture_loc, ts_texture);
                DrawTexturePro(
                    ts_texture,
                    gt_rect,
                    Rectangle {
                        pst_rect.x,
                        pst_rect.y,
                        pst_rect.width,
                        pst_rect.height
                    },
                    // pst_rect,
                    Vector2 {0, 0},
                    0,
                    WHITE
                );
                EndShaderMode();
                EndTextureMode();

                for (int l = 1; l < 10; l++) {
                    BeginTextureMode(_layers[sublayer + l]);
                    BeginShaderMode(_white_remover);
                    SetShaderValueTexture(_white_remover, _white_remover_texture_loc, ts_texture);
                    DrawTexturePro(
                        ts_texture,
                        Rectangle {
                            gt_rect.x + 120,
                            gt_rect.y,
                            gt_rect.width,
                            gt_rect.height
                        },
                        Rectangle {
                            pst_rect.x,
                            pst_rect.y,
                            pst_rect.width,
                            pst_rect.height
                        },
                        // pst_rect,
                        Vector2 {0, 0},
                        0,
                        WHITE
                    );
                    EndShaderMode();
                    EndTextureMode();
                }
            }

        }

        skip:

        queue.pop();
        progress++;
    }

    return queue.empty();
}

bool Renderer::_frame_render_pipe_layer(uint8_t layer, int threshold) {
    /*
        Only 2 materials are actually renderered: Small Pipes and Trash.
        This is actually how it's rendererd.
    */

    if (layer > 2 || threshold <= 0) return true;

    auto &queue = _materials_to_render[_camera_index][layer][static_cast<size_t>(MaterialRenderType::pipe)];
    int progress = 0;
    uint8_t sublayer = layer * 10;
    auto *default_material = _materials->material(_level->default_material);

    CastMember 
        *pipe_tiles = _castlibs->member("pipeTiles2"), 
        *trash_tiles = _castlibs->member("trashTiles3"), 
        *large_trash_tiles = _castlibs->member("largeTrashTiles"), 
        *dirt_tiles = _castlibs->member("dirtTiles"), 
        *sandy_dirt_tiles = _castlibs->member("sandyDirtTiles"),
        *assorted_trash = _castlibs->member("assortedTrash");

    const auto get_tiles = [=](MaterialDef const *def){
        if (def->get_name() == "Small Pipes") return pipe_tiles;
        if (def->get_name() == "Trash") return trash_tiles;
        if (def->get_name() == "LargeTrash") return large_trash_tiles;
        if (def->get_name() == "MegaTrash") return large_trash_tiles;
        if (def->get_name() == "Dirt") return dirt_tiles;
        if (def->get_name() == "Sandy Dirt") return dirt_tiles;

        return _castlibs->member(def->get_name() + "Tiles");
    };

    static const auto CONNECTION_VERTICAL   = static_cast<uint8_t>(0b00101);
    static const auto CONNECTION_HORIZONTAL = static_cast<uint8_t>(0b01010);
    static const auto CONNECTION_CROSS      = static_cast<uint8_t>(0b01111);
    static const auto CONNECTION_TRB        = static_cast<uint8_t>(0b00111);
    static const auto CONNECTION_BLT        = static_cast<uint8_t>(0b01101);
    static const auto CONNECTION_LTR        = static_cast<uint8_t>(0b01110);
    static const auto CONNECTION_RBL        = static_cast<uint8_t>(0b01011);
    static const auto CONNECTION_RB         = static_cast<uint8_t>(0b00011);
    static const auto CONNECTION_BL         = static_cast<uint8_t>(0b01001);
    static const auto CONNECTION_LT         = static_cast<uint8_t>(0b01100);
    static const auto CONNECTION_TR         = static_cast<uint8_t>(0b00110);
    static const auto CONNECTION_L          = static_cast<uint8_t>(0b01000);
    static const auto CONNECTION_R          = static_cast<uint8_t>(0b00010);
    static const auto CONNECTION_T          = static_cast<uint8_t>(0b00100);
    static const auto CONNECTION_B          = static_cast<uint8_t>(0b00001);
    static const auto CONNECTION_SLOPE_NW   = static_cast<uint8_t>(0b10000);
    static const auto CONNECTION_SLOPE_NE   = static_cast<uint8_t>(0b11000);
    static const auto CONNECTION_SLOPE_ES   = static_cast<uint8_t>(0b11100);
    static const auto CONNECTION_SLOPE_SW   = static_cast<uint8_t>(0b11110);
    static const auto CONNECTION_SINGLE     = static_cast<uint8_t>(0b00000);
    static const auto CONNECTION_PLATFORM   = static_cast<uint8_t>(0b11111);
    static const auto CONNECTION_GLASS      = static_cast<uint8_t>(0b10111);

    const auto &mtx = _level->get_const_tile_matrix();
    const auto &geos = _level->get_const_geo_matrix();

    const auto get_connection = [this, &mtx, &geos, layer](MaterialDef const* def, int mx, int my){
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

    static const auto get_src_pos = [](uint8_t connection){
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

    Rectangle src;
    bool trash;

    while (progress < threshold && !queue.empty()) {
        
        const auto &cell = queue.front();

        if (cell.geo.is_air()) continue;

        auto *tiles = get_tiles(cell.tile->material_def);
        if (tiles == nullptr) continue;
        const Texture2D &texture = tiles->get_loaded_texture();
        if (!tiles->is_loaded()) continue;

        trash = cell.tile->material_def->get_name() == "Trash";

        uint8_t connection = get_connection(cell.tile->material_def, cell.mx, cell.my);

        if (connection == 0) continue;

        auto pos = get_src_pos(connection);

        if (cell.tile->material_def->get_name() == "Small Pipes") {
            BeginTextureMode(_layers[sublayer + 5]);
            DrawRectangleLinesEx(
                Rectangle { cell.x * 20.0f, cell.y * 20.0f, 20.0f, 20.0f },
                2,
                Color { 0, 255, 0, 255 }
            );
            EndTextureMode();
        }

        src = Rectangle {
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
            Rectangle { cell.x*20.0f, cell.y*20.0f, 20.0f, 20.0f },
            Vector2 { 0, 0 },
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
            Rectangle { cell.x*20.0f, cell.y*20.0f, 20.0f, 20.0f },
            Vector2 { 0, 0 },
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
            Rectangle { cell.x*20.0f, cell.y*20.0f, 20.0f, 20.0f },
            Vector2 { 0, 0 },
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
            Rectangle { cell.x*20.0f, cell.y*20.0f, 20.0f, 20.0f },
            Vector2 { 0, 0 },
            0,
            WHITE
        );
        EndShaderMode();
        EndTextureMode();

        if (trash) {
            const Texture2D texture2 = assorted_trash->get_loaded_texture();
            if (!assorted_trash->is_loaded()) {
                for (int l = 0; l < 3; l++) {
                    const auto s = sublayer + _rand.next(10);
                    const auto srcx = _rand.next(48);
                    const auto middle = Vector2{
                        cell.x * 20.0f + 1.0f + _rand.next(18) * (_rand.next(2) == 1 ? -1 : 1),
                        cell.y * 20.0f        + _rand.next(18) * (_rand.next(2) == 1 ? -1 : 1)
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
    
                    BeginTextureMode(_layers[s]);
                    BeginShaderMode(_ink);
                    SetShaderValueTexture(_ink, _ink_texture_loc, texture2);
                    mr::draw::draw_texture(
                        texture2,
                        src,
                        Quad(target).rotated(_rand.next(360))
                    );
                    EndShaderMode();
                    EndTextureMode();
                }
            }
        }
    
        progress++;
        queue.pop();
    }

    return queue.empty();
}

bool Renderer::_frame_render_tiles_layer(uint8_t layer, int threshold) {
    if (layer > 2 || threshold <= 0) return true;

    auto &queue = _materials_to_render[_camera_index][layer][0];
    int progress = 0;
    uint8_t sublayer = layer * 10;
    auto *default_material = _materials->material(_level->default_material);

    const auto &tiles = _level->get_const_tile_matrix();
    const auto &geos = _level->get_const_geo_matrix();
    Matrix<bool> taken(_level->get_width(), _level->get_height(), 1);

    const auto fits = [&taken, &tiles, &geos, layer, default_material](const TileDef *tile, const MaterialDef *mat, matrix_t mx, matrix_t my){
        for (int w = 0; w < tile->get_width(); w++) {
            for (int h = 0; h < tile->get_height(); h++) {
                if (taken.get_copy(mx + w, my + h, 0)) return false;

                const auto *geo = geos.get_const_ptr(mx + w, my + h, layer);
                if (geo == nullptr) return false;
                if (geo->is_air()) return false;
                if (!geo->is_solid() && (tile->get_width() > 1 || tile->get_height() > 1)) return false;
                
                const auto &tilecell = tiles.get_const(mx + w, my + h, layer);
                if (
                    tilecell.type == TileType::material && tilecell.material_def != mat || 
                    tilecell.type == TileType::_default && mat != default_material
                ) return false;
            }
        }

        return true;
    };

    const auto take = [&taken](const TileDef *tile, matrix_t mx, matrix_t my) {
        for (int w = 0; w < tile->get_width(); w++) {
            for (int h = 0; h < tile->get_height(); h++) {
                taken.set_noexcept(mx + w, my + h, 0, true);
            }
        }
    };

    MaterialDef 
        *chaotic_stone = _materials->material("Chaotic Stone"),
        *tiled_stone = _materials->material("Tiled Stone"),
        *random_machines = _materials->material("Random Machines");

    TileDef 
        *square_stone = _tiles->tile("Square Stone"), 
        *small_stone = _tiles->tile("Small Stone");

    while (progress < threshold, !queue.empty()) {

        const auto &cell = queue.front();

        if (cell.tile->material_def == chaotic_stone) {
            
        } else if (cell.tile->material_def == tiled_stone) {

        } else if (cell.tile->material_def == random_machines) {
            
        }

        progress++;
        queue.pop();
    }

    return queue.empty();
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

    static const auto CONNECTION_VERTICAL = static_cast<uint8_t>(0b00101);
    static const auto CONNECTION_HORIZONTAL = static_cast<uint8_t>(0b01010);
    static const auto CONNECTION_CROSS = static_cast<uint8_t>(0b01111);
    static const auto CONNECTION_TRB = static_cast<uint8_t>(0b00111);
    static const auto CONNECTION_BLT = static_cast<uint8_t>(0b01101);
    static const auto CONNECTION_LTR = static_cast<uint8_t>(0b01110);
    static const auto CONNECTION_RBL = static_cast<uint8_t>(0b01011);
    static const auto CONNECTION_RB = static_cast<uint8_t>(0b00011);
    static const auto CONNECTION_BL = static_cast<uint8_t>(0b01001);
    static const auto CONNECTION_LT = static_cast<uint8_t>(0b01100);
    static const auto CONNECTION_TR = static_cast<uint8_t>(0b00110);
    static const auto CONNECTION_L = static_cast<uint8_t>(0b01000);
    static const auto CONNECTION_R = static_cast<uint8_t>(0b00010);
    static const auto CONNECTION_T = static_cast<uint8_t>(0b00100);
    static const auto CONNECTION_B = static_cast<uint8_t>(0b00001);
    static const auto CONNECTION_SLOPE_NW = static_cast<uint8_t>(0b10000);
    static const auto CONNECTION_SLOPE_NE = static_cast<uint8_t>(0b11000);
    static const auto CONNECTION_SLOPE_ES = static_cast<uint8_t>(0b11100);
    static const auto CONNECTION_SLOPE_SW = static_cast<uint8_t>(0b11110);
    static const auto CONNECTION_SINGLE = static_cast<uint8_t>(0b00000);
    static const auto CONNECTION_PLATFORM = static_cast<uint8_t>(0b11111);
    static const auto CONNECTION_GLASS = static_cast<uint8_t>(0b10111);

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

    static const auto CONNECTION_VERTICAL = static_cast<uint8_t>(0b00101);
    static const auto CONNECTION_HORIZONTAL = static_cast<uint8_t>(0b01010);
    static const auto CONNECTION_CROSS = static_cast<uint8_t>(0b01111);
    static const auto CONNECTION_TRB = static_cast<uint8_t>(0b00111);
    static const auto CONNECTION_BLT = static_cast<uint8_t>(0b01101);
    static const auto CONNECTION_LTR = static_cast<uint8_t>(0b01110);
    static const auto CONNECTION_RBL = static_cast<uint8_t>(0b01011);
    static const auto CONNECTION_RB = static_cast<uint8_t>(0b00011);
    static const auto CONNECTION_BL = static_cast<uint8_t>(0b01001);
    static const auto CONNECTION_LT = static_cast<uint8_t>(0b01100);
    static const auto CONNECTION_TR = static_cast<uint8_t>(0b00110);
    static const auto CONNECTION_L = static_cast<uint8_t>(0b01000);
    static const auto CONNECTION_R = static_cast<uint8_t>(0b00010);
    static const auto CONNECTION_T = static_cast<uint8_t>(0b00100);
    static const auto CONNECTION_B = static_cast<uint8_t>(0b00001);
    static const auto CONNECTION_SLOPE_NW = static_cast<uint8_t>(0b10000);
    static const auto CONNECTION_SLOPE_NE = static_cast<uint8_t>(0b11000);
    static const auto CONNECTION_SLOPE_ES = static_cast<uint8_t>(0b11100);
    static const auto CONNECTION_SLOPE_SW = static_cast<uint8_t>(0b11110);
    static const auto CONNECTION_SINGLE = static_cast<uint8_t>(0b00000);
    static const auto CONNECTION_PLATFORM = static_cast<uint8_t>(0b11111);
    static const auto CONNECTION_GLASS = static_cast<uint8_t>(0b10111);

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
                EndShaderMode();
                EndTextureMode();
            }
        }
    }
}

void Renderer::_render_bigmetal_layer(uint8_t layer) {
    if (layer > 2) return;

    auto *tileset = _castlibs->member("tileSetBigMetal");
    if (tileset == nullptr) return;

    auto *tileset_floor = _castlibs->member("tileSetBigMetalFloor");

    const auto &texture = tileset->get_loaded_texture();
    if (!tileset->is_loaded()) return;

    Texture2D texture2;
    if (tileset_floor != nullptr) texture2 = tileset_floor->get_loaded_texture();

    const auto *def = _materials->material("BigMetal");
    if (def == nullptr) return;

    bool skip_default = _level->default_material != def->get_name();

    static const auto LEFT         = static_cast<uint8_t>(0b10000000);
    static const auto TOP          = static_cast<uint8_t>(0b01000000);
    static const auto RIGHT        = static_cast<uint8_t>(0b00100000);
    static const auto BOTTOM       = static_cast<uint8_t>(0b00010000);
    static const auto TOP_LEFT     = static_cast<uint8_t>(0b00001000);
    static const auto TOP_RIGHT    = static_cast<uint8_t>(0b00000100);
    static const auto BOTTOM_RIGHT = static_cast<uint8_t>(0b00000010);
    static const auto BOTTOM_LEFT  = static_cast<uint8_t>(0b00000001);

    auto src_v = Rectangle { 30.0f, 10.0f, 10.0f, 10.0f };
    auto src_h = Rectangle { 50.0f, 10.0f, 10.0f, 10.0f };
    
    auto src_tl = Rectangle { 10.0f, 10.0f, 10.0f, 10.0f };
    auto src_tr = Rectangle { 10.0f, 30.0f, 10.0f, 10.0f };
    auto src_br = Rectangle { 10.0f, 50.0f, 10.0f, 10.0f };
    auto src_bl = Rectangle { 10.0f, 70.0f, 10.0f, 10.0f };

    auto src_i_tl = Rectangle { 70.0f, 10.0f, 10.0f, 10.0f };
    auto src_i_tr = Rectangle { 70.0f, 30.0f, 10.0f, 10.0f };
    auto src_i_br = Rectangle { 70.0f, 50.0f, 10.0f, 10.0f };
    auto src_i_bl = Rectangle { 70.0f, 70.0f, 10.0f, 10.0f };
    auto e_src = Rectangle { 90.0f, 10.0f, 10.0f, 10.0f };
    
    const auto &tiles = _level->get_const_tile_matrix();
    const auto &geos = _level->get_const_geo_matrix();

    auto rt = LoadRenderTexture(final_width, final_height);

    BeginTextureMode(rt);
    ClearBackground(WHITE);

    for (int x = 0; x < columns; x++) {
        for (int y = 0; y < rows; y++) {
            int cx = x + static_cast<int>(_camera->get_position().x/20);
            int cy = y + static_cast<int>(_camera->get_position().y/20);
        
            if (!tiles.is_in_bounds(cx, cy, layer)) continue;

            matrix_t mx = static_cast<matrix_t>(cx);
            matrix_t my = static_cast<matrix_t>(cy);

            const auto &tile = tiles.get_const(mx, my, layer);
            const auto &geo = geos.get_const(mx, my, layer);

            if (geo.is_air()) continue;
            if (tile.type != TileType::material && tile.type != TileType::_default) continue;
            if (
                (tile.type == TileType::material && tile.material_def != def) || 
                (tile.type == TileType::_default && skip_default)
            ) continue;

            DrawRectangleRec(
                Rectangle { x * 20.0f, y * 20.0f, 20.0f, 20.0f },
                Color { 0, 255, 0, 255 }
            );

            uint8_t connection = 0;

            auto left = _is_material(mx - 1, my, layer, def);
            auto top = _is_material(mx, my - 1, layer, def);
            auto right = _is_material(mx + 1, my, layer, def);
            auto bottom = _is_material(mx, my + 1, layer, def);

            auto topleft = _is_material(mx - 1, my - 1, layer, def);
            auto topright = _is_material(mx + 1, my - 1, layer, def);
            auto bottomright = _is_material(mx + 1, my + 1, layer, def);
            auto bottomleft = _is_material(mx - 1, my + 1, layer, def);

            if (left) connection |= LEFT;
            if (top) connection |= TOP;
            if (right) connection |= RIGHT;
            if (bottom) connection |= BOTTOM;

            if (topleft) connection |= TOP_LEFT;
            if (topright) connection |= TOP_RIGHT;
            if (bottomright) connection |= BOTTOM_RIGHT;
            if (bottomleft) connection |= BOTTOM_LEFT;


            Rectangle tl_target = { x * 20.0f        , y * 20.0f        , 10.0f, 10.0f };
            Rectangle tr_target = { x * 20.0f + 10.0f, y * 20.0f        , 10.0f, 10.0f };
            Rectangle br_target = { x * 20.0f + 10.0f, y * 20.0f + 10.0f, 10.0f, 10.0f };
            Rectangle bl_target = { x * 20.0f        , y * 20.0f + 10.0f, 10.0f, 10.0f };

            Rectangle tl_src, tr_src, br_src, bl_src;
            
            switch (connection) {
                case LEFT: tl_src = bl_src = src_h; tr_src = src_tr; br_src = src_br; break;
                case TOP: tl_src = tr_src = src_tr; bl_src = src_bl; br_src = src_br; break;
                case RIGHT: tr_src = br_src = src_h; tl_src = src_tl; bl_src = src_bl; break;
                case BOTTOM: bl_src = br_src = src_v; tl_src = src_tl; tr_src = src_tr; break;

                case TOP | BOTTOM: tl_src = tr_src = br_src = bl_src = src_v; break;
                case LEFT | RIGHT: tl_src = tr_src = br_src = bl_src = src_h; break;

                case LEFT | TOP | RIGHT | BOTTOM:
                    tl_src = src_i_tl; 
                    tl_src = src_i_tr;
                    br_src = src_i_br;
                    bl_src = src_i_bl;
                break;

                case TOP | LEFT: tl_src = src_i_tl; tr_src = src_v; br_src = src_br; bl_src = src_h; break;
                case TOP | RIGHT: tl_src = src_v; tr_src = src_i_tr; br_src = src_h; bl_src = src_bl; break;
                case BOTTOM | RIGHT: tl_src = src_tl; tr_src = src_h; br_src = src_i_br; bl_src = src_v; break;
                case BOTTOM | LEFT: tl_src = src_h; tr_src = src_tr; br_src = src_v; src_i_bl; break;
            }

            if (connection & TOP_LEFT) src_tl = e_src;
            if (connection & TOP_RIGHT) src_tr = e_src;
            if (connection & BOTTOM_RIGHT) src_br = e_src;
            if (connection & BOTTOM_LEFT) src_bl = e_src;

            DrawTexturePro(
                texture,
                tl_src,
                tl_target,
                Vector2 { 0, 0 },
                0,
                WHITE
            );

            DrawTexturePro(
                texture,
                tr_src,
                tr_target,
                Vector2 { 0, 0 },
                0,
                WHITE
            );

            DrawTexturePro(
                texture,
                br_src,
                br_target,
                Vector2 { 0, 0 },
                0,
                WHITE
            );

            DrawTexturePro(
                texture,
                bl_src,
                bl_target,
                Vector2 { 0, 0 },
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

}