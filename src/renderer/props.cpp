#include <vector>
#include <thread>
#include <atomic>
#include <string>
#include <random>
#include <numeric>
#include <stdexcept>
#include <algorithm>

#ifdef IS_DEBUG_BUILD
#include <iostream>
#endif

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

void Renderer::_draw_prop(Prop *prop) noexcept {
    if (prop == nullptr) return;

    if (prop->prop_def == nullptr && prop->tile_def == nullptr) return;

    auto quad = prop->quad - _level->cameras[0].get_position();

    if (prop->prop_def != nullptr) {

    } else if (prop->tile_def != nullptr) {
        auto *def = prop->tile_def;

        if (def->get_type() != TileDefType::voxel_struct) return;

        auto &texture = def->get_loaded_texture();
        if (!def->is_texture_loaded()) return;

        float width = def->calculate_width();
        float height = def->calculate_height();

        Vector2 vertices[4] = { quad.topright, quad.topleft, quad.bottomleft, quad.bottomright };
        float coords[4] = {
            0, 
            0, 
            width / texture.width, 
            height / texture.height
        };

        uint8_t starting_depth = static_cast<uint8_t>(abs(prop->depth));
        uint8_t l = 0;

        while (l < def->get_repeat().size() && starting_depth < 30) {
            for (size_t s = 0; s < def->get_repeat()[l]; s++) {
                if (starting_depth >= 30) break;

                BeginTextureMode(_layers[starting_depth]);
                // BeginShaderMode(_white_remover);
                // SetShaderValueTexture(_white_remover, _white_remover_texture_loc, texture);
                BeginShaderMode(_invb);
                SetShaderValueTexture(_invb, _invb_texture_loc, texture);
                SetShaderValueV(_invb, _invb_vertices_loc, vertices, SHADER_UNIFORM_VEC2, 4);
                SetShaderValueV(_invb, _invb_tex_coord_loc, coords, SHADER_UNIFORM_FLOAT, 4);

                mr::draw::draw_texture(texture, quad);

                EndShaderMode();
                EndTextureMode();

                starting_depth++;
            }

            l++;
            coords[1] += height / texture.height;
            coords[3] += height / texture.height;
        }
    }
}

}