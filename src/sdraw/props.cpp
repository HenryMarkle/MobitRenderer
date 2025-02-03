#include <raylib.h>

#include <MobitRenderer/definitions.h>
#include <MobitRenderer/state.h>
#include <MobitRenderer/sdraw.h>
#include <MobitRenderer/draw.h>
#include <MobitRenderer/quad.h>

namespace mr::sdraw {

void draw_standard_prop_preview(
    const Standard *def,
    const PropSettings *settings,
    const shaders *_shaders,
    const Quad &quad
) noexcept {
    if (def == nullptr) return;
    if (_shaders == nullptr) return;
    if (!def->is_loaded()) return;

    const auto &texture = def->get_texture();
    const auto &shader = _shaders->voxel_struct();

    BeginShaderMode(shader);

    SetShaderValueTexture(shader, GetShaderLocation(shader, "texture0"), texture);
    
    auto layers = def->repeat.size();
    SetShaderValue(shader, GetShaderLocation(shader, "layers"), &layers, SHADER_UNIFORM_INT);

    float height = def->height * 20.0f / texture.height;
    SetShaderValue(shader, GetShaderLocation(shader, "height"), &height, SHADER_UNIFORM_FLOAT);

    float width = def->width * 20.0f / texture.width;
    SetShaderValue(shader, GetShaderLocation(shader, "width"), &width, SHADER_UNIFORM_FLOAT);

    int depth = 2;
    SetShaderValue(shader, GetShaderLocation(shader, "depth"), &depth, SHADER_UNIFORM_INT);

    mr::draw::draw_texture(texture, quad);

    EndShaderMode();
}


};