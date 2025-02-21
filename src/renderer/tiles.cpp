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

void Renderer::_draw_tile_origin_mtx(TileDef *def, matrix_t x, matrix_t y, uint8_t layer) noexcept {
    if (def == nullptr || layer > 2) return;

    auto texture = def->get_loaded_texture();
    if (!def->is_texture_loaded()) return;

    auto offset = def->get_head_offset();

    float ox = (x - offset.x - def->get_buffer()) * 20.0f;
    float oy = (y - offset.y - def->get_buffer()) * 20.0f;

    float width = def->calculate_width();
    float height = def->calculate_height();

    // float head_start = def->calculate_height() * (def->get_repeat().size() - 1);

    Rectangle target = Rectangle {
        ox,
        oy,
        width,
        height
    };

    switch (def->get_type()) {
    case TileDefType::voxel_struct:
    {
        Rectangle src = Rectangle {
            0,
            0,
            width,
            height
        };

        uint8_t comm = layer * 10;
        uint8_t l = 0;

        while (l < def->get_repeat().size() && comm < 30) {

            for (size_t s = 0; s < def->get_repeat()[l]; s++) {
                BeginTextureMode(_layers[comm]);
                BeginShaderMode(_white_remover);
                SetShaderValueTexture(_white_remover, _white_remover_texture_loc, texture);
                DrawTexturePro(
                    texture,
                    src,
                    target,
                    Vector2{0,0},
                    0,
                    WHITE
                );
                EndShaderMode();
                EndTextureMode();

                comm++;
            }

            l++;

            src.y += height;
        }
    }
    break;

    case TileDefType::box:
    {
        Rectangle src = Rectangle {
            0,
            0,
            width,
            height
        };

        for (int l = 0; l < 10; l++) {

            BeginTextureMode(_layers[layer * 10 + l]);
            BeginShaderMode(_white_remover);
            SetShaderValueTexture(_white_remover, _white_remover_texture_loc, texture);
    
            DrawTexturePro(
                texture,
                Rectangle {
                    0, 
                    static_cast<float>(def->get_width() * def->get_height() * 20),
                    width,
                    height
                },
                target,
                Vector2{0, 0},
                0,
                WHITE
            );
    
            EndShaderMode();
            EndTextureMode();
        }
    }
    break;

    case TileDefType::voxel_struct_rock_type:
    {
        int l = layer * 10;

        while (l < mr::utils::clamp(l + 9 + !def->get_specs2().empty() * 10, 0, 29)) {
            BeginTextureMode(_layers[l]);
            BeginShaderMode(_white_remover);
            SetShaderValueTexture(_white_remover, _white_remover_texture_loc, texture);

            DrawTexturePro(
                texture,
                Rectangle {
                    width * _rand.next(def->get_rnd()),
                    0,
                    width,
                    height
                },
                target,
                Vector2{0,0},
                0,
                WHITE
            );

            EndShaderMode();
            EndTextureMode();

            l++;
        }
    }
    break;
    }
}

void Renderer::_draw_tiles_layer(uint8_t layer) noexcept {
    switch (layer) {
    case 0:
        for (auto &c : _tiles_to_render1) _draw_tile_origin_mtx(c.cell->tile_def, c.x, c.y, 0);
    break;

    case 1:
        for (auto &c : _tiles_to_render2) _draw_tile_origin_mtx(c.cell->tile_def, c.x, c.y, 1);
    break;

    case 2:
        for (auto &c : _tiles_to_render3) _draw_tile_origin_mtx(c.cell->tile_def, c.x, c.y, 2);
    break;
    }
}

};