#include <stdexcept>

#include <raylib.h>

#include <MobitRenderer/definitions.h>
#include <MobitRenderer/state.h>
#include <MobitRenderer/sdraw.h>
#include <MobitRenderer/draw.h>
#include <MobitRenderer/quad.h>

namespace mr::sdraw {

void draw_tile_as_prop(
    const TileDef *def,
    const shaders *_shaders,
    const Quad &quad
) noexcept {
    if (def == nullptr) return;
    if (_shaders == nullptr) return;

    const auto &texture = def->get_texture();
    if (texture.id == 0) return;

    const auto &shader = _shaders->voxel_struct_tinted();
    BeginShaderMode(shader);
    {
        SetShaderValueTexture(shader, GetShaderLocation(shader, "texture0"), texture);
        
        auto layers = def->get_repeat().size();
        SetShaderValue(shader, GetShaderLocation(shader, "layers"), &layers, SHADER_UNIFORM_INT);

        float height = def->calculate_height(20)*1.0f / def->get_texture().height;
        SetShaderValue(shader, GetShaderLocation(shader, "height"), &height, SHADER_UNIFORM_FLOAT);
        
        float width = def->calculate_width(20)*1.0f / def->get_texture().width;
        SetShaderValue(shader, GetShaderLocation(shader, "width"), &width, SHADER_UNIFORM_FLOAT);
        
        auto depth = 2;
        SetShaderValue(shader, GetShaderLocation(shader, "depth"), &depth, SHADER_UNIFORM_INT);
        
        mr::draw::draw_texture(texture, quad);
    }
    EndShaderMode();
}

void draw_prop_preview(
    const PropDef *def,
    const PropSettings *settings,
    const shaders *_shaders,
    const Quad &quad
) noexcept {
    if (def == nullptr) return;
    if (settings == nullptr) return;
    if (_shaders == nullptr) return;
    if (!def->is_loaded()) return;

    switch (def->type) {
        case PropType::standard:
        {
        #ifdef IS_DEBUG_BUILD
            const Standard *standard = dynamic_cast<const Standard*>(def);
            if (standard == nullptr) 
                throw std::runtime_error("Prop definition was not a Standard* (conflicts with the prop type)");
        #else
            const Standard *standard = static_cast<const Standard*>(def);
        #endif

            draw_standard_prop_preview(standard, settings, _shaders, quad); 
        }
        break;

        case PropType::varied_standard:
        {
        #ifdef IS_DEBUG_BUILD
            const VariedStandard *varied_standard = dynamic_cast<const VariedStandard*>(def);
            if (varied_standard == nullptr) 
                throw std::runtime_error("Prop definition was not a VariedStandard* (conflicts with the prop type)");
        #else
            const VariedStandard *varied_standard = static_cast<const VariedStandard*>(def);
        #endif

            draw_varied_standard_prop_preview(varied_standard, settings, _shaders, quad);
        }
        break;

        case PropType::soft:
        {
        #ifdef IS_DEBUG_BUILD
            const Soft *soft = dynamic_cast<const Soft*>(def);
            if (soft == nullptr) 
                throw std::runtime_error("Prop definition was not a Soft* (conflicts with the prop type)");
        #else
            const Soft *soft = static_cast<const Soft*>(def);
        #endif

            draw_soft_prop_preview(soft, settings, _shaders, quad);
        }
        break;

        case PropType::varied_soft:
        {
        #ifdef IS_DEBUG_BUILD
            const VariedSoft *varied_soft = dynamic_cast<const VariedSoft*>(def);
            if (varied_soft == nullptr) 
                throw std::runtime_error("Prop definition was not a VariedSoft* (conflicts with the prop type)");
        #else
            const VariedSoft *varied_soft = static_cast<const VariedSoft*>(def);
        #endif

            draw_varied_soft_prop_preview(varied_soft, settings, _shaders, quad);
        }
        break;

        case PropType::decal:
        {
        #ifdef IS_DEBUG_BUILD
            const Decal *decal = dynamic_cast<const Decal*>(def);
            if (decal == nullptr) 
                throw std::runtime_error("Prop definition was not a Decal* (conflicts with the prop type)");
        #else
            const Decal *decal = static_cast<const Decal*>(def);
        #endif

            draw_decal_prop_preview(decal, settings, _shaders, quad);
        }
        break;

        case PropType::varied_decal:
        {
        #ifdef IS_DEBUG_BUILD
            const VariedDecal *varied_decal = dynamic_cast<const VariedDecal*>(def);
            if (varied_decal == nullptr) 
                throw std::runtime_error("Prop definition was not a VariedDecal* (conflicts with the prop type)");
        #else
            const VariedDecal *varied_decal = static_cast<const VariedDecal*>(def);
        #endif

            draw_varied_decal_prop_preview(varied_decal, settings, _shaders, quad);
        }
        break;

        default:
        {
            const auto &texture = def->get_texture();
            const auto &shader = _shaders->default_prop();
            
            BeginShaderMode(shader);
            SetShaderValueTexture(shader, GetShaderLocation(shader, "texture0"), texture);
            mr::draw::draw_texture(texture, quad);
            EndShaderMode();
        }
        break;
    }
}

void draw_prop_preview(
    const Prop *prop,
    const shaders *_shaders
) noexcept {
    if (prop == nullptr) return;
    if (_shaders == nullptr) return;

    if (prop->prop_def != nullptr && prop->tile_def == nullptr) {
        draw_prop_preview(prop->prop_def, &prop->settings, _shaders, prop->quad);
    } else if (prop->prop_def == nullptr && prop->tile_def != nullptr) {
        draw_tile_as_prop(prop->tile_def, _shaders, prop->quad);
    }
}

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
    
    const auto &shader = def->color_treatment == PropColorTreatment::bevel 
        ? _shaders->voxel_struct_tinted() 
        : _shaders->voxel_struct();

    BeginShaderMode(shader);

    SetShaderValueTexture(shader, GetShaderLocation(shader, "texture0"), texture);
    
    auto layers = def->repeat.size();
    SetShaderValue(shader, GetShaderLocation(shader, "layers"), &layers, SHADER_UNIFORM_INT);

    float height = def->height * 20.0f / texture.height;
    SetShaderValue(shader, GetShaderLocation(shader, "height"), &height, SHADER_UNIFORM_FLOAT);

    float width = def->width * 20.0f / texture.width;
    SetShaderValue(shader, GetShaderLocation(shader, "width"), &width, SHADER_UNIFORM_FLOAT);

    float depth = -(0.8f / layers);
    SetShaderValue(shader, GetShaderLocation(shader, "depth"), &depth, SHADER_UNIFORM_FLOAT);

    mr::draw::draw_texture(texture, quad, WHITE);

    EndShaderMode();
}

void draw_varied_standard_prop_preview(
    const VariedStandard *def,
    const PropSettings *settings,
    const shaders *_shaders,
    const Quad &quad
) noexcept {
    if (def == nullptr) return;
    if (_shaders == nullptr) return;
    if (!def->is_loaded()) return;

    const auto &texture = def->get_texture();
    const auto &shader = _shaders->varied_voxel_struct();

    BeginShaderMode(shader);

    SetShaderValueTexture(shader, GetShaderLocation(shader, "texture0"), texture);
    
    SetShaderValue(shader, GetShaderLocation(shader, "variation"), &settings->variation, SHADER_UNIFORM_INT);

    auto layers = def->repeat.size();
    SetShaderValue(shader, GetShaderLocation(shader, "layers"), &layers, SHADER_UNIFORM_INT);

    float height = def->height * 20.0f / texture.height;
    SetShaderValue(shader, GetShaderLocation(shader, "height"), &height, SHADER_UNIFORM_FLOAT);

    float width = def->width * 20.0f / texture.width;
    SetShaderValue(shader, GetShaderLocation(shader, "width"), &width, SHADER_UNIFORM_FLOAT);

    float depth = -0.06f;
    SetShaderValue(shader, GetShaderLocation(shader, "depth"), &depth, SHADER_UNIFORM_FLOAT);

    const auto prop_width = def->get_pixel_width();

    mr::draw::draw_texture(texture, quad);

    EndShaderMode();
}

void draw_soft_prop_preview(
    const Soft *def,
    const PropSettings *settings,
    const shaders *_shaders,
    const Quad &quad
) noexcept {
    if (def == nullptr) return;
    if (_shaders == nullptr) return;
    if (!def->is_loaded()) return;

    const auto texture = def->get_texture();
    const auto shader = _shaders->soft();
    
    BeginShaderMode(shader);
    {
        SetShaderValueTexture(shader, GetShaderLocation(shader, "texture0"), texture);
        mr::draw::draw_texture(texture, quad);
    }
    EndShaderMode();
}

void draw_varied_soft_prop_preview(
    const VariedSoft *def,
    const PropSettings *settings,
    const shaders *_shaders,
    const Quad &quad
) noexcept {
    if (def == nullptr) return;
    if (_shaders == nullptr) return;
    if (!def->is_loaded()) return;

    const auto texture = def->get_texture();
    const auto shader = _shaders->varied_soft();
    
    BeginShaderMode(shader);
    {
        SetShaderValueTexture(shader, GetShaderLocation(shader, "texture0"), texture);
        SetShaderValue(shader, GetShaderLocation(shader, "variation"), &settings->variation, SHADER_UNIFORM_INT);

        const float width = def->variations > 1 ? def->get_pixel_width() / (float)texture.width : 1.0f;
        SetShaderValue(shader, GetShaderLocation(shader, "width"), &width, SHADER_UNIFORM_FLOAT);

        if (def->colorize) {
            mr::draw::draw_texture(
                texture, 
                Rectangle{
                    0, 
                    0, 
                    (float)texture.width, 
                    texture.height / 2.0f
                }, 
                quad
            );
        } else {
            mr::draw::draw_texture(texture, quad);
        }
    }
    EndShaderMode();
}

void draw_decal_prop_preview(
    const Decal *def,
    const PropSettings *settings,
    const shaders *_shaders,
    const Quad &quad
) noexcept {
    if (def == nullptr) return;
    if (_shaders == nullptr) return;
    if (!def->is_loaded()) return;

    const auto texture = def->get_texture();

    const auto shader = def->tags.find("customColor") == def->tags.end() 
        ? _shaders->default_prop() 
        : _shaders->white_remover_apply_color();

    BeginShaderMode(shader);
    {
        SetShaderValueTexture(shader, GetShaderLocation(shader, "texture0"), texture);

        mr::draw::draw_texture(texture, quad, WHITE);
    }
    EndShaderMode();
}

void draw_varied_decal_prop_preview(
    const VariedDecal *def,
    const PropSettings *settings,
    const shaders *_shaders,
    const Quad &quad
) noexcept {
    if (def == nullptr) return;
    if (_shaders == nullptr) return;
    if (!def->is_loaded()) return;

    const auto texture = def->get_texture();

    const auto shader = def->tags.find("customColor") == def->tags.end() 
        ? _shaders->default_prop() 
        : _shaders->white_remover_apply_color();

    BeginShaderMode(shader);
    {
        SetShaderValueTexture(shader, GetShaderLocation(shader, "texture0"), texture);

        const auto width = def->pixel_width;

        mr::draw::draw_texture(
            texture, 
            Rectangle{
                0.0f + (width * settings->variation),
                0,
                (float)width,
                (float)texture.height
            }, 
            quad, 
            WHITE
        );
    }
    EndShaderMode();
}

};