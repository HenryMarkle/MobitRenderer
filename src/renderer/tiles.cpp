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
#include <MobitRenderer/sdraw.h>
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

    float ox = (x - offset.x - def->get_buffer()) * 20.0f - _level->cameras[0].get_position().x;
    float oy = (y - offset.y - def->get_buffer()) * 20.0f - _level->cameras[0].get_position().y;

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
                if (comm >= 30) break;
                
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
        int limit = mr::utils::clamp(l + 9 + !def->get_specs2().empty() * 10, 0, 29);

        while (l < limit) {
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

    case TileDefType::voxel_struct_random_displace_vertical:
    case TileDefType::voxel_struct_random_displace_horizontal:
    {
        auto sublayer = layer * 10;

        float width = def->calculate_width();
        float height = def->calculate_height();

        auto src = Rectangle {
            0,
            0,
            width,
            height
        };

        auto target = Rectangle {
            ox,
            oy,
            width,
            height
        };

        Rectangle src1, src2, target1, target2;

        if (def->get_type() == TileDefType::voxel_struct_random_displace_vertical) {
            auto s = _level->seed + x;
            auto displace = _rand.next(static_cast<int>(height));

            src1 = src2 = src;

            src1.height = displace;

            src2.y = src.y + displace;
            src2.height = src.height - displace;
            
            
            target1 = target2 = target;
            target1.y = target.y + target.height - displace;
            target2.height = target.height - displace;
        } else {
            auto s = _level->seed + y;
            auto displace = _rand.next(static_cast<int>(width));

            src1 = src2 = src;

            src1.height = displace;

            src2.x = src.x + displace;
            src2.width = src.width - displace;
            
            
            target1 = target2 = target;
            target1.x = target.x + target.width - displace;
            target2.width = target.width - displace;
        }

        //                   v    _frontImg was used instead
        BeginTextureMode(_layers[sublayer]);
        BeginShaderMode(_white_remover);
        SetShaderValueTexture(_white_remover, _white_remover_texture_loc, texture);

        DrawTexturePro(
            texture,
            src1,
            target1,
            Vector2{0, 0},
            0,
            WHITE
        );

        DrawTexturePro(
            texture,
            src2,
            target2,
            Vector2{0, 0},
            0,
            WHITE
        );

        EndShaderMode();
        EndTextureMode();

        auto d = -1;

        for (auto l = 0; l < def->get_repeat().size(); l++)
        {
            for (auto repeat = 0; repeat < def->get_repeat()[l]; repeat++)
            {
                d++;

                if (d + sublayer > 29) goto out;

                BeginTextureMode(_layers[d + sublayer]);
                BeginShaderMode(_white_remover);
                SetShaderValueTexture(_white_remover, _white_remover_texture_loc, texture);

                DrawTexturePro(
                    texture,
                    Rectangle {
                        src1.x,
                        src1.y + src1.height*l,
                        src1.width,
                        src1.height
                    },
                    target1,
                    Vector2 {0, 0},
                    0,
                    WHITE
                );

                DrawTexturePro(
                    texture,
                    Rectangle {
                        src2.x,
                        src2.y + src2.height*l,
                        src2.width,
                        src2.height
                    },
                    target2,
                    Vector2 {0, 0},
                    0,
                    WHITE
                );

                EndShaderMode();
                EndTextureMode();

                auto src11 = src1;
                auto src22 = src2;

                src11.x = src1.x + width;
                src11.y = src1.height * l + height;

                src22.x = src2.x + width;
                src22.y = src2.height * l + height;

                bool colored = def->get_tags().count("colored");
                bool eff1 = def->get_tags().count("effectColorA");
                bool eff2 = def->get_tags().count("effectColorB");

                if (colored && !eff1 && !eff2) {
                    BeginTextureMode(_dc_layers[d + sublayer]);
                    BeginShaderMode(_white_remover);
                    SetShaderValueTexture(_white_remover, _white_remover_texture_loc, texture);

                    DrawTexturePro(
                        texture,
                        src11,
                        target1,
                        Vector2 {0, 0},
                        0,
                        WHITE
                    );
    
                    DrawTexturePro(
                        texture,
                        src22,
                        target2,
                        Vector2 {0, 0},
                        0,
                        WHITE
                    );

                    EndShaderMode();
                    EndTextureMode();
                }
            
                if (eff1) {
                    BeginTextureMode(_ga_layers[d + sublayer]);
                    mr::sdraw::draw_texture_darkest(texture, src11, target1);
                    mr::sdraw::draw_texture_darkest(texture, src22, target2);
                    EndTextureMode();
                }

                if (eff2) {
                    BeginTextureMode(_gb_layers[d + sublayer]);
                    mr::sdraw::draw_texture_darkest(texture, src11, target1);
                    mr::sdraw::draw_texture_darkest(texture, src22, target2);
                    EndTextureMode();
                }
            }
        }

        out: {}
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