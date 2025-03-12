#include <queue>
#include <vector>
#include <thread>
#include <atomic>
#include <string>
#include <random>
#include <cmath>
#include <stdexcept>
#include <algorithm>

#include <raylib.h>

#include <spdlog/spdlog.h>

#include <MobitRenderer/vec.h>
#include <MobitRenderer/dirs.h>
#include <MobitRenderer/level.h>
#include <MobitRenderer/state.h>
#include <MobitRenderer/matrix.h>
#include <MobitRenderer/renderer.h>
#include <MobitRenderer/definitions.h>

#define RENDER_PROGRESS_TILES 1
#define RENDER_PROGRESS_MATERIALS 2
#define RENDER_PROGRESS_PROPS 3
#define RENDER_PROGRESS_EXTRA 4
#define RENDER_PROGRESS_EFFECTS 5
#define RENDER_PROGRESS_LIGHT 6
#define RENDER_PROGRESS_TEXT 7
#define RENDER_PROGRESS_DONE 8

namespace mr::renderer {

render_error::render_error(const std::string &msg) : msg_(msg) {}
render_error::render_error(const std::string &msg, const std::exception &e) : msg_(msg+": "+e.what()) {}
const char *render_error::what() const noexcept { return msg_.c_str(); }

Renderer::Renderer(
    std::shared_ptr<Dirs> dirs,
    std::shared_ptr<spdlog::logger> logger,
    TileDex *tiledex,
    PropDex *propdex,
    MaterialDex *materialdex,
    CastLibs *castlibs
) : 
    _dirs(dirs), 
    _logger(logger),
    _tiles(tiledex), 
    _props(propdex), 
    _materials(materialdex), 
    _castlibs(castlibs),

    _vflip_texture_loc(0),
    _white_remover_texture_loc(0),
    _white_remover_vflip_texture_loc(0),

    _bevel_texture_loc(0),
    _bevel_tex_size_loc(0),
    _bevel_thick_loc(0),
    _bevel_highlight_loc(0),
    _bevel_shadow_loc(0),
    _bevel_vflip_loc(0),

    _invb_texture_loc(0),
    _invb_vertices_loc(0),
    _invb_tex_coord_loc(0),

    _red_encoder_texture_loc(0),
    _red_encoder_lightmap_loc(0),
    _rgb_apply_palette_lightmap_loc(0),
    _red_encoder_depth_loc(0),
    _red_encoder_flipv_loc(0),

    _rgb_apply_palette_texture_loc(0),
    _rgb_apply_palette_palette_loc(0),
    _rgb_apply_palette_depth_loc(0),
    _rgb_apply_palette_vflip_loc(0),

    _palette_sky_texture_loc(0),

    _sill_texture_loc(0),
    _sill_invert_loc(0),
    _sill_vflip_loc(0),

    _cross_sill_texture_loc(0),
    _cross_sill_sill_loc(0),
    _cross_sill_invert_loc(0),
    _cross_sill_vflip_loc(0),
    
    _preparation_done(false),
    _initialized(false),

    _level(nullptr),

    _shaders_initialized(false),
    _layers_initialized(false),
    _dc_layers_initialized(false),
    _ga_layers_initialized(false),
    _gb_layers_initialized(false),
    _quadified_initialized(false),
    _lightmap_initialized(false),
    _final_initialized(false),

    _layers_cleaned(false),
    _dc_layers_cleaned(false),
    _ga_layers_cleaned(false),
    _gb_layers_cleaned(false),
    _quadified_cleaned(false),
    _lightmap_cleaned(false),
    _final_cleaned(false),

    _tiles_to_render1({}),
    _tiles_to_render2({}),
    _tiles_to_render3({}),

    _layers_init_progress(0),
    _layers_clean_progress(0),
    _layers_compose_progress(29),
    _light_render_progress(0),
    _quadify_progress(0),

    _render_progress(0),

    _tile_layer_progress(0),
    _rand(0),

    _camera_index(0),
    _camera(nullptr),

    _material_progress(0),
    _material_layer_progress(0),
    _material_progress_x(0),
    _material_progress_y(0)
{}

Renderer::~Renderer() {
    if (_initialized) {
        for (size_t l = 0; l < 30; l++) {
            UnloadRenderTexture(_layers[l]);
            UnloadRenderTexture(_dc_layers[l]);
            UnloadRenderTexture(_ga_layers[l]);
            UnloadRenderTexture(_gb_layers[l]);
            UnloadRenderTexture(_quadified_layers[l]);
        }

        UnloadRenderTexture(_final);
        UnloadRenderTexture(_final_lightmap);
        UnloadRenderTexture(_composed_lightmap);
        UnloadRenderTexture(_composed_layers);
        UnloadRenderTexture(_material_canvas);

        UnloadShader(_vflip);
        UnloadShader(_white_remover);
        UnloadShader(_ink);
        UnloadShader(_white_remover_vflip);
        UnloadShader(_composer);
        UnloadShader(_bevel);
        UnloadShader(_invb);
        UnloadShader(_red_encoder);
        UnloadShader(_rgb_apply_palette);
        UnloadShader(_palette_sky);
        UnloadShader(_sill);
        UnloadShader(_cross_sill);
    }

    if (_preparation_thread.joinable()) _preparation_thread.join();
}

void Renderer::initialize() {
    _logger->info("[Renderer] initializing");
    
    if (_initialized) {
        _logger->info("[Renderer] (initialization) unloading shaders");
        
        UnloadShader(_white_remover);
        UnloadShader(_bevel);
        UnloadShader(_invb);
    } else {
        _logger->info("[Renderer] (initialization) loading render textures");

        for (size_t l = 0; l < 30; l++) {
            _layers[l]    = LoadRenderTexture(final_width, final_height);
            _dc_layers[l] = LoadRenderTexture(final_width, final_height);
            _ga_layers[l] = LoadRenderTexture(final_width, final_height);
            _ga_layers[l] = LoadRenderTexture(final_width, final_height);
            _gb_layers[l] = LoadRenderTexture(final_width, final_height);
        }

        _final = LoadRenderTexture(final_width, final_height);
        _composed_layers = LoadRenderTexture(final_width, final_height);
    }

    _logger->info("[Renderer] (initialization) initializing render textures");
    
    for (size_t l = 0; l < 30; l++) {
        BeginTextureMode(_layers[l]);
        ClearBackground(WHITE);
        EndTextureMode();

        BeginTextureMode(_layers[l]);
        ClearBackground(WHITE);
        EndTextureMode();

        BeginTextureMode(_layers[l]);
        ClearBackground(WHITE);
        EndTextureMode();
        
        BeginTextureMode(_layers[l]);
        ClearBackground(WHITE);
        EndTextureMode();

        BeginTextureMode(_layers[l]);
        ClearBackground(WHITE);
        EndTextureMode();
    }
    
    BeginTextureMode(_final);
    ClearBackground(WHITE);
    EndTextureMode();

    BeginTextureMode(_composed_layers);
    ClearBackground(WHITE);
    EndTextureMode();

    _logger->info("[Renderer] (initialization) reloading shaders");

    const auto &shaders_dir = _dirs->get_shaders();

    _white_remover = LoadShader(nullptr, (shaders_dir / "white_remover.frag").string().c_str());
    _bevel = LoadShader(nullptr, (shaders_dir / "bevel.frag").string().c_str());
    _invb = LoadShader((shaders_dir / "invb.vert").string().c_str(), (shaders_dir / "invb.frag").string().c_str());

    _initialized = true;
}

bool Renderer::frame_initialize(int threshold) {
    if (_initialized) return true;

    if (!_shaders_initialized) {
        const auto &shaders_dir = _dirs->get_shaders();

        _vflip = LoadShader(nullptr, (shaders_dir / "vflip.frag").string().c_str());
        _white_remover = LoadShader(nullptr, (shaders_dir / "white_remover.frag").string().c_str());
        _white_remover_vflip = LoadShader(nullptr, (shaders_dir / "white_remover_vflip.frag").string().c_str());
        _ink = LoadShader(nullptr, (shaders_dir / "white_remover_apply_color.frag").string().c_str());
        _composer = LoadShader(nullptr, (shaders_dir / "white_remover_apply_white_tint_vflip.frag").string().c_str());
        _bevel = LoadShader(nullptr, (shaders_dir / "bevel.frag").string().c_str());
        _invb = LoadShader((shaders_dir / "invb.vert").string().c_str(), (shaders_dir / "invb.frag").string().c_str());
        _red_encoder = LoadShader(nullptr, (shaders_dir / "red_encoder.frag").string().c_str());
        _rgb_apply_palette = LoadShader(nullptr, (shaders_dir / "rgb_apply_palette.frag").string().c_str());
        _palette_sky = LoadShader(nullptr, (shaders_dir / "palette_sky.frag").string().c_str());
        _sill = LoadShader(nullptr, (shaders_dir / "binary_map.frag").string().c_str());
        _cross_sill = LoadShader(nullptr, (shaders_dir / "cross_binary_map.frag").string().c_str());
        
        _vflip_texture_loc = GetShaderLocation(_vflip, "texture0");
        
        _white_remover_texture_loc = GetShaderLocation(_white_remover, "texture0");
        _white_remover_vflip_texture_loc = GetShaderLocation(_white_remover_vflip, "texture0");

        _ink_texture_loc = GetShaderLocation(_ink, "texture0");
        
        _composer_texture_loc = GetShaderLocation(_composer, "texture0");
        _composer_tint_loc = GetShaderLocation(_composer, "tint");
        
        _bevel_texture_loc = GetShaderLocation(_bevel, "texture0");
        _bevel_tex_size_loc = GetShaderLocation(_bevel, "texSize");
        _bevel_thick_loc = GetShaderLocation(_bevel, "edgeThickness");
        _bevel_highlight_loc = GetShaderLocation(_bevel, "highlights");
        _bevel_shadow_loc = GetShaderLocation(_bevel, "shadows");
        _bevel_vflip_loc = GetShaderLocation(_bevel, "vflip");
        
        _invb_texture_loc = GetShaderLocation(_invb, "textureSampler");
        _invb_vertices_loc = GetShaderLocation(_invb, "vertex_pos");
        _invb_tex_coord_loc = GetShaderLocation(_invb, "tex_coord_pos");
        
        _red_encoder_texture_loc = GetShaderLocation(_red_encoder, "texture0");
        _red_encoder_lightmap_loc = GetShaderLocation(_red_encoder, "lightmap");
        _red_encoder_depth_loc = GetShaderLocation(_red_encoder, "depth");
        _red_encoder_flipv_loc = GetShaderLocation(_red_encoder, "flipv");
        
        _rgb_apply_palette_texture_loc = GetShaderLocation(_rgb_apply_palette, "texture0");
        _rgb_apply_palette_palette_loc = GetShaderLocation(_rgb_apply_palette, "palette");
        _rgb_apply_palette_lightmap_loc = GetShaderLocation(_rgb_apply_palette, "lightmap");
        _rgb_apply_palette_depth_loc = GetShaderLocation(_rgb_apply_palette, "depth");
        _rgb_apply_palette_vflip_loc = GetShaderLocation(_rgb_apply_palette, "vflip");

        _palette_sky_texture_loc = GetShaderLocation(_palette_sky, "texture0");

        _sill_texture_loc = GetShaderLocation(_sill, "texture0");
        _sill_invert_loc = GetShaderLocation(_sill, "invert");
        _sill_vflip_loc = GetShaderLocation(_sill, "vflip");

        _cross_sill_texture_loc = GetShaderLocation(_cross_sill, "texture0");
        _cross_sill_sill_loc = GetShaderLocation(_cross_sill, "map");
        _cross_sill_invert_loc = GetShaderLocation(_cross_sill, "invert");
        _cross_sill_vflip_loc = GetShaderLocation(_cross_sill, "vflip");

        _shaders_initialized = true;
        return false;
    }

    if (!_layers_initialized) {
        int progress = 0;

        while (progress < threshold && (_layers_init_progress) < 30) {
            _layers[_layers_init_progress] = LoadRenderTexture(final_width, final_height);
            
            _layers_init_progress++;
            progress++;
        }

        _layers_initialized = _layers_init_progress > 29;
        if (_layers_initialized) _layers_init_progress = 0;
        return false;
    }

    if (!_dc_layers_initialized) {
        int progress = 0;

        while (progress < threshold && (_layers_init_progress) < 30) {
            _dc_layers[_layers_init_progress] = LoadRenderTexture(final_width, final_height);
            
            _layers_init_progress++;
            progress++;
        }

        _dc_layers_initialized = _layers_init_progress > 29;
        if (_dc_layers_initialized) _layers_init_progress = 0;
        return false;
    }

    if (!_ga_layers_initialized) {
        int progress = 0;

        while (progress < threshold && (_layers_init_progress) < 30) {
            _ga_layers[_layers_init_progress] = LoadRenderTexture(final_width, final_height);

            _layers_init_progress++;
            progress++;
        }

        _ga_layers_initialized = _layers_init_progress > 29;
        if (_ga_layers_initialized) _layers_init_progress = 0;
        return false;
    }

    if (!_gb_layers_initialized) {
        int progress = 0;

        while (progress < threshold && (_layers_init_progress) < 30) {
            _gb_layers[_layers_init_progress] = LoadRenderTexture(final_width, final_height);
        
            _layers_init_progress++;
            progress++;
        }

        _gb_layers_initialized = _layers_init_progress > 29;
        if (_gb_layers_initialized) _layers_init_progress = 0;
        return false;
    }

    if (!_quadified_initialized) {
        int progress = 0;

        while (progress < threshold && (_layers_init_progress) < 30) {
            _quadified_layers[_layers_init_progress] = LoadRenderTexture(final_width, final_height);
        
            _layers_init_progress++;
            progress++;
        }

        _quadified_initialized = _layers_init_progress > 29;
        if (_quadified_initialized) _layers_init_progress = 0;
        return false;
    }

    if (!_lightmap_initialized) {
        _composed_lightmap = LoadRenderTexture(final_width, final_height);
        _final_lightmap = LoadRenderTexture(final_width, final_height);

        _lightmap_initialized = true;
    }

    if (!_final_initialized) {
        _final = LoadRenderTexture(final_width, final_height);
        _composed_layers = LoadRenderTexture(final_width, final_height);
        _material_canvas = LoadRenderTexture(final_width, final_height);
        _final_initialized = true;
        return false;
    }

    _initialized = true;
    return true;
}

bool Renderer::frame_cleanup(int threshold) {
    if (!_initialized) return true;
    if (_cleaned_up) return true;

    if (!_layers_cleaned) {
        int progress = 0;

        while (progress < threshold && _layers_clean_progress < 30) {
            BeginTextureMode(_layers[_layers_clean_progress]);
            ClearBackground(WHITE);
            EndTextureMode();

            _layers_clean_progress++;
            progress++;
        }

        _layers_cleaned = _layers_clean_progress > 29;
        if (_layers_cleaned) _layers_clean_progress = 0;
        return false;
    }

    if (!_dc_layers_cleaned) {
        int progress = 0;

        while (progress < threshold && _layers_clean_progress < 30) {
            BeginTextureMode(_dc_layers[_layers_clean_progress]);
            ClearBackground(WHITE);
            EndTextureMode();
        
            _layers_clean_progress++;
            progress++;
        }

        _dc_layers_cleaned = _layers_clean_progress > 29;
        if (_dc_layers_cleaned) _layers_clean_progress = 0;
        return false;
    }

    if (!_ga_layers_cleaned) {
        int progress = 0;

        while (progress < threshold && _layers_clean_progress < 30) {
            BeginTextureMode(_ga_layers[_layers_clean_progress]);
            ClearBackground(WHITE);
            EndTextureMode();

            _layers_clean_progress++;
            progress++;
        }

        _ga_layers_cleaned = _layers_clean_progress > 29;
        if (_ga_layers_cleaned) _layers_clean_progress = 0;
        return false;
    }

    if (!_gb_layers_cleaned) {
        int progress = 0;

        while (progress < threshold && _layers_clean_progress < 30) {
            BeginTextureMode(_gb_layers[_layers_clean_progress]);
            ClearBackground(WHITE);
            EndTextureMode();

            _layers_clean_progress++;
            progress++;
        }

        _gb_layers_cleaned = _layers_clean_progress > 29;
        if (_gb_layers_cleaned) _layers_clean_progress = 0;
        return false;
    }

    if (!_lightmap_cleaned) {
        BeginTextureMode(_final_lightmap);
        ClearBackground(BLACK);
        EndTextureMode();

        BeginTextureMode(_composed_lightmap);
        ClearBackground(BLACK);
        EndTextureMode();

        _lightmap_cleaned = true;
    }

    _layers_clean_progress = 0;

    if (!_final_cleaned) {
        BeginTextureMode(_final);
        ClearBackground(WHITE);
        EndTextureMode();
        _final_cleaned = true;
        return false;
    }

    _layers_clean_progress = 0;

    _render_progress = 0;
    _tile_layer_progress = 0;
    _light_render_progress = 0;

    _cleaned_up = true;

    _layers_cleaned = false;
    _dc_layers_cleaned = false;
    _ga_layers_cleaned = false;
    _gb_layers_cleaned = false;
    _final_cleaned = false;
    
    return true;
}

void Renderer::frame_compose(int threshold) {
    if (!_initialized) return;

    int progress = 0;

    BeginTextureMode(_composed_layers);
    
    if (_layers_compose_progress == 29) ClearBackground(WHITE);

    while (progress < threshold && (_layers_compose_progress) >= 0) {
        const auto &l = _layers[_layers_compose_progress];
        
        BeginShaderMode(_composer);
        SetShaderValueTexture(_composer, _composer_texture_loc, l.texture);
        float tint = (_layers_compose_progress) / 32.0f;
        SetShaderValue(_composer, _composer_tint_loc, &tint, SHADER_UNIFORM_FLOAT);
        DrawTexture(l.texture, 5 - _layers_compose_progress, 5 - _layers_compose_progress, WHITE);
        EndShaderMode();

        _layers_compose_progress--;
        progress--;
    }
    EndTextureMode();

    if (_layers_compose_progress <= -1) { _layers_compose_progress = 29; }
}

void Renderer::frame_compose(
    int min_layer,
    int max_layer,
    float offsetx,
    float offsety,
    bool fog,
    int threshold
) {
    if (!_initialized) return;

    int progress = 0;

    BeginTextureMode(_composed_layers);
    while (
        progress < threshold && 
        _layers_compose_progress >= 0
    ) {
        if (_layers_compose_progress == 29) ClearBackground(WHITE);

        if (_layers_compose_progress <= max_layer && _layers_compose_progress >= min_layer) {
            const auto &l = _layers[_layers_compose_progress];
            
            BeginShaderMode(_composer);
            SetShaderValueTexture(_composer, _composer_texture_loc, l.texture);
            float tint = fog * (_layers_compose_progress) / 32.0f;
            SetShaderValue(_composer, _composer_tint_loc, &tint, SHADER_UNIFORM_FLOAT);
            DrawTexture(l.texture, offsetx * (5 - _layers_compose_progress), offsety * (5 - _layers_compose_progress), WHITE);
            EndShaderMode();
        }

        _layers_compose_progress--;
        progress--;
    }
    EndTextureMode();

    if (_layers_compose_progress <= -1) { _layers_compose_progress = 29; }
}

#ifdef FEATURE_PALETTES
void Renderer::frame_compose_palette(
    int min_layer,
    int max_layer,
    float offsetx,
    float offsety,
    bool fog,
    const Palette &palette,
    int threshold
) {
    if (!_initialized) return;

    int progress = 0;

    BeginTextureMode(_composed_layers);
    if (_layers_compose_progress == 29) {
        BeginShaderMode(_palette_sky);
        SetShaderValueTexture(_palette_sky, _palette_sky_texture_loc, palette.get_texture());
        DrawTexturePro(
            palette.get_texture(), 
            {0,0,32,16}, 
            {0,0,final_width,final_height},
            {0,0},
            0, 
            WHITE
        );
        EndShaderMode();
    }

    while (
        progress < threshold && 
        _layers_compose_progress >= 0
    ) {
        if (_layers_compose_progress <= max_layer && _layers_compose_progress >= min_layer) {
            const auto &l = _quadified_layers[_layers_compose_progress];
            
            BeginShaderMode(_rgb_apply_palette);
            SetShaderValueTexture(_rgb_apply_palette, _rgb_apply_palette_texture_loc, l.texture);
            SetShaderValueTexture(_rgb_apply_palette, _rgb_apply_palette_palette_loc, palette.get_texture());
            SetShaderValueTexture(_rgb_apply_palette, _rgb_apply_palette_lightmap_loc, _final_lightmap.texture);
            SetShaderValue(_rgb_apply_palette, _rgb_apply_palette_depth_loc, &_layers_compose_progress, SHADER_UNIFORM_INT);
            int vflip = 1;
            SetShaderValue(_rgb_apply_palette, _rgb_apply_palette_vflip_loc, &vflip, SHADER_UNIFORM_INT);

            DrawTexture(l.texture, 5 - offsetx * _layers_compose_progress, 5 - offsety * _layers_compose_progress, WHITE);
            EndShaderMode();
        }

        _layers_compose_progress--;
        progress--;
    }
    EndTextureMode();

    if (_layers_compose_progress <= -1) { _layers_compose_progress = 29; }
}
#endif

bool Renderer::frame_quadify_layers(int threshold) {
    if (!_initialized) return false;

    if (_quadify_progress == 29) _quadify_progress = 0;

    int progress = 0;

    auto camera = _level->cameras[0];
    camera.set_position(Vector2{0, 0});
    const auto quadify = [](Vector2 pos, float radius, int degree, int depth) {
        const float scaled_radius = radius * (depth - 5);
        const float rotated_angle = (degree - 90) * (PI / 180.0f);

        return Vector2{
            pos.x + scaled_radius * (float)cos(rotated_angle),
            pos.y + scaled_radius * (float)sin(rotated_angle)
        }; 
    };

    auto quad = Quad(
        Vector2{0,0},
        Vector2{final_width, 0},
        Vector2{final_width, final_height},
        Vector2{0, final_height}
    );
    
    // quad.topleft -= Vector2{5, 5};
    // quad.topright -= Vector2{5, 5};
    // quad.bottomright -= Vector2{5, 5};
    // quad.bottomleft -= Vector2{5, 5};

    while (progress < threshold && _quadify_progress < 30) {

        quad.topleft = Vector2 { 1,  1} * _quadify_progress + quadify(camera.get_top_left_point(), camera.get_top_left_radius(), camera.get_top_left_angle(), _quadify_progress);
        quad.topright = Vector2 {-1,  1} * _quadify_progress + quadify(camera.get_top_right_point(), camera.get_top_right_radius(), camera.get_top_right_angle(), _quadify_progress);;
        quad.bottomright = Vector2 {-1, -1} * _quadify_progress + quadify(camera.get_bottom_right_point(), camera.get_bottom_right_radius(), camera.get_bottom_right_angle(), _quadify_progress);;
        quad.bottomleft = Vector2 { 1, -1} * _quadify_progress + quadify(camera.get_bottom_left_point(), camera.get_bottom_left_radius(), camera.get_bottom_left_angle(), _quadify_progress);;

        BeginTextureMode(_quadified_layers[_quadify_progress]);
        ClearBackground(WHITE);

        BeginShaderMode(_vflip);
        SetShaderValueTexture(_vflip, _vflip_texture_loc, _layers[_quadify_progress].texture);

        mr::draw::draw_texture(
            _layers[_quadify_progress].texture,
            quad
        );

        EndShaderMode();

        EndTextureMode();

        progress++;
        _quadify_progress++;
    }

    return _quadify_progress >= 30;
}

bool Renderer::frame_render_light(int threshold) {
    if (!_initialized) return false;

    if (_light_render_progress == 29) _light_render_progress = 0;

    float rad = (_level->light_angle - 90) * (PI / 180.0f);
    float angle_x = -cos(rad);
    float angle_y = sin(rad);

    if (std::abs(angle_x) < 1e-6) {
        angle_x = 0;
    }
    if (std::abs(angle_y) < 1e-6) {
        angle_y = 0;
    }

    auto projection_angle = Vector2{ angle_x, angle_y };

    int cinvert = 1;
    int finvert = 1; // does not do anything
    
    int cvflip = 1;
    int fvflip = 1;

    int progress = 1;

    auto dest = Rectangle {
        0,
        0,
        final_width,
        final_height
    };

    while (progress < threshold && _light_render_progress < 30) {
        if (_light_render_progress == 0) {
            BeginTextureMode(_composed_lightmap);
            ClearBackground(BLACK);
            EndTextureMode();

            BeginTextureMode(_final_lightmap);
            ClearBackground(BLACK);
            EndTextureMode();
        }

        const auto &texture = _quadified_layers[_light_render_progress].texture;

        BeginTextureMode(_final_lightmap);
        BeginShaderMode(_cross_sill);
        SetShaderValueTexture(_cross_sill, _cross_sill_texture_loc, texture);
        SetShaderValueTexture(_cross_sill, _cross_sill_sill_loc, _composed_lightmap.texture);
        SetShaderValue(_cross_sill, _cross_sill_invert_loc, &finvert, SHADER_UNIFORM_INT);
        SetShaderValue(_cross_sill, _cross_sill_vflip_loc, &fvflip, SHADER_UNIFORM_INT);

        DrawTexturePro(
            texture,
            dest,
            dest,
            Vector2 {0, 0},
            0,
            WHITE
        );

        EndShaderMode();
        EndTextureMode();


        BeginTextureMode(_composed_lightmap);
        BeginShaderMode(_sill);
        SetShaderValueTexture(_sill, _sill_texture_loc, texture);
        SetShaderValue(_sill, _sill_invert_loc, &cinvert, SHADER_UNIFORM_INT);
        SetShaderValue(_sill, _sill_vflip_loc, &cvflip, SHADER_UNIFORM_INT);

        const auto angle = projection_angle * (_level->light_flatness + _light_render_progress - 1);

        DrawTexturePro(
            texture,
            dest,
            Rectangle {angle.x, angle.y, final_width, final_height},
            // Rectangle {0, 3.0f+_light_render_progress, final_width, final_height},
            Vector2 {0, 0},
            0,
            WHITE
        );

        EndShaderMode();
        EndTextureMode();

        progress++;
        _light_render_progress++;
    }

    return _light_render_progress >= 29;
}

void Renderer::frame_render_final(int threshold) {
    if (!_initialized) return;
    if (_layers_compose_progress <= -1) return;
    if (_layers_compose_progress == 29) ClearBackground(WHITE);

    int progress = 0;
    int flipv = 1;

    BeginTextureMode(_final);
    while (progress < threshold && (_layers_compose_progress) >= 0) {
        const auto &l = _quadified_layers[_layers_compose_progress].texture;
      
        BeginShaderMode(_red_encoder);
        SetShaderValueTexture(_red_encoder, _red_encoder_texture_loc, l);
        SetShaderValueTexture(_red_encoder, _red_encoder_lightmap_loc, _final_lightmap.texture);
        SetShaderValue(_red_encoder, _red_encoder_depth_loc, &_layers_compose_progress, SHADER_UNIFORM_INT);
        SetShaderValue(_red_encoder, _red_encoder_flipv_loc, &flipv, SHADER_UNIFORM_INT);

        DrawTexture(l, 0, 0, WHITE);

        EndShaderMode();

        _layers_compose_progress--;
        progress--;
    }
    EndTextureMode();

    if (_layers_compose_progress <= -1) { _layers_compose_progress = 29; }
}

void Renderer::load(const Level *level) {
    _logger->info("[Renderer] loading level");

    if (level == nullptr) throw std::invalid_argument("level is nullptr");

    _level = level;
}

void Renderer::_prepare() {
    if (_level == nullptr) return;

    _rand = RandomGen(_level->seed);

    const auto &mtx = _level->get_const_tile_matrix();
    const auto &geos = _level->get_const_geo_matrix();

    _tiles_to_render1.clear();
    _tiles_to_render2.clear();
    _tiles_to_render3.clear();

    _materials_to_render.clear();

    _tiles_to_render1.resize(_config.cameras.empty() ? _level->cameras.size() : _config.cameras.size());
    _tiles_to_render2.resize(_config.cameras.empty() ? _level->cameras.size() : _config.cameras.size());
    _tiles_to_render3.resize(_config.cameras.empty() ? _level->cameras.size() : _config.cameras.size());

    for (int c = 0; c < _tiles_to_render1.size(); c++) {
        const auto &camera = _config.cameras.empty() ? _level->cameras[c] : _level->cameras[_config.cameras[c]];

        for (int x = 0; x < columns; x++) {
            for (int y = 0; y < rows; y++) {
                const int mx = x + static_cast<int>(camera.get_position().x/20);
                const int my = y + static_cast<int>(camera.get_position().y/20);

                if (mtx.is_in_bounds(mx, my, 0)) {
                    const auto *cell1 = mtx.get_const_ptr(static_cast<matrix_t>(mx), static_cast<matrix_t>(my), 0);

                    if (cell1 != nullptr && cell1->type == TileType::head && cell1->tile_def != nullptr) {
                        _tiles_to_render1[c]
                            .push_back(
                                Render_TileCell{ 
                                    _rand.next(100000), 
                                    static_cast<matrix_t>(x), 
                                    static_cast<matrix_t>(y), 
                                    0, 
                                    cell1 
                                }
                            );
                    }
                }

                if (mtx.is_in_bounds(mx, my, 1)) {
                    const auto *cell2 = mtx.get_const_ptr(static_cast<matrix_t>(mx), static_cast<matrix_t>(my), 1);

                    if (cell2 != nullptr && cell2->type == TileType::head && cell2->tile_def != nullptr) {
                        _tiles_to_render2[c]
                            .push_back(
                                Render_TileCell{ 
                                    _rand.next(100000), 
                                    static_cast<matrix_t>(x), 
                                    static_cast<matrix_t>(y), 
                                    1, 
                                    cell2 
                                }
                            );
                    }
                }

                if (mtx.is_in_bounds(mx, my, 2)) {
                    const auto *cell3 = mtx.get_const_ptr(static_cast<matrix_t>(mx), static_cast<matrix_t>(my), 2);
        
                    if (cell3 != nullptr && cell3->type == TileType::head && cell3->tile_def != nullptr) {
                        _tiles_to_render3[c]
                            .push_back(
                                Render_TileCell{ 
                                    _rand.next(100000), 
                                    static_cast<matrix_t>(x), 
                                    static_cast<matrix_t>(y), 
                                    2, 
                                    cell3 
                                }
                            );
                    }
                }

            }
        }
    }

    const auto tiles_sort_alg = [](const Render_TileCell &c1, const Render_TileCell &c2) { return c1.rnd < c2.rnd; };

    for (int c = 0; c < _tiles_to_render1.size(); c++) {
        std::sort(
            _tiles_to_render1[c].begin(), 
            _tiles_to_render1[c].end(), 
            tiles_sort_alg
        );
    
        std::sort(
            _tiles_to_render2[c].begin(), 
            _tiles_to_render2[c].end(), 
            tiles_sort_alg
        );
    
        std::sort(
            _tiles_to_render3[c].begin(), 
            _tiles_to_render3[c].end(), 
            tiles_sort_alg
        );
    }

    size_t cams_num = _config.cameras.empty() ? _level->cameras.size() : _config.cameras.size();
    const auto *default_material = _materials->material(_level->default_material);

    auto materials_to_render = std::vector<std::vector<std::vector<std::vector<Render_MaterialCell>>>>(3);

    materials_to_render.reserve(cams_num);

    const auto materials_sort_alg = [](Render_MaterialCell const &p1, Render_MaterialCell const &p2) { 
        return p1.rnd < p2.rnd; 
    };

    for (size_t c = 0; c < cams_num; c++) {
        materials_to_render[c].resize(3);

        materials_to_render[c][0].resize(18);
        materials_to_render[c][1].resize(18);
        materials_to_render[c][2].resize(18);

        const auto &camera = _config.cameras.empty() ? _level->cameras[c] : _level->cameras[_config.cameras[c]];

        for (int x = 0; x < columns; x++) {
            for (int y = 0; y < rows; y++) {
                int cx = x + static_cast<int>(camera.get_position().x/20);
                int cy = y + static_cast<int>(camera.get_position().y/20);

                if (!mtx.is_in_bounds(cx, cy, 0)) continue;

                matrix_t mx = static_cast<matrix_t>(cx);
                matrix_t my = static_cast<matrix_t>(cy);

                const auto &tile1 = mtx.get_const(mx, my, 0);
                const auto &tile2 = mtx.get_const(mx, my, 1);
                const auto &tile3 = mtx.get_const(mx, my, 2);

                const auto &geo1 = geos.get_const(mx, my, 0);
                const auto &geo2 = geos.get_const(mx, my, 1);
                const auto &geo3 = geos.get_const(mx, my, 2);
            
                if (!geo1.is_air()) {
                    if (tile1.type == TileType::material && tile1.material_def != nullptr) 
                    {
                        materials_to_render
                            [c]
                            [0]
                            [static_cast<size_t>(tile1.material_def->get_type())]
                            .push_back(Render_MaterialCell(_rand.next(100000), mx, my, 0, x, y, 0, geo1, &tile1));
                    }
                    else if (tile1.type == TileType::_default) {
                        materials_to_render
                            [c]
                            [0]
                            [static_cast<size_t>(default_material->get_type())]
                            .push_back(Render_MaterialCell(_rand.next(100000), mx, my, 0, x, y, 0, geo1, &tile1));
                    }
                }

                if (!geo2.is_air()) {
                    if (tile2.type == TileType::material && tile2.material_def != nullptr) 
                    {
                        materials_to_render
                            [c]
                            [1]
                            [static_cast<size_t>(tile2.material_def->get_type())]
                            .push_back(Render_MaterialCell(_rand.next(100000), mx, my, 1, x, y, 1, geo2, &tile2));
                    }
                    else if (tile2.type == TileType::_default) {
                        materials_to_render
                            [c]
                            [1]
                            [static_cast<size_t>(default_material->get_type())]
                            .push_back(Render_MaterialCell(_rand.next(100000), mx, my, 1, x, y, 1, geo2, &tile2));
                    }
                }

                if (!geo3.is_air()) {
                    if (tile3.type == TileType::material && tile3.material_def != nullptr) 
                    {
                        materials_to_render
                            [c]
                            [2]
                            [static_cast<size_t>(tile3.material_def->get_type())]
                            .push_back(Render_MaterialCell(_rand.next(100000), mx, my, 0, x, y, 2, geo3, &tile3));
                    }
                    else if (tile3.type == TileType::_default) {
                        materials_to_render
                            [c]
                            [2]
                            [static_cast<size_t>(default_material->get_type())]
                            .push_back(Render_MaterialCell(_rand.next(100000), mx, my, 0, x, y, 2, geo3, &tile3));
                    }
                }
            }
        }
    
        for (int t = 0; t < 18; t++) {
            std::sort(
                materials_to_render[c][0][t].begin(),
                materials_to_render[c][0][t].end(),
                materials_sort_alg
            );

            std::sort(
                materials_to_render[c][1][t].begin(),
                materials_to_render[c][1][t].end(),
                materials_sort_alg
            );

            std::sort(
                materials_to_render[c][2][t].begin(),
                materials_to_render[c][2][t].end(),
                materials_sort_alg
            );
        }
    }

    _materials_to_render.resize(cams_num);

    for (size_t c = 0; c < cams_num; c++) {
        _materials_to_render[c].resize(3);

        _materials_to_render[c][0].resize(18);
        _materials_to_render[c][1].resize(18);
        _materials_to_render[c][2].resize(18);

        for (int t = 0; t < 18; t++) {
            for (auto &m : materials_to_render[c][0][t]) {
                _materials_to_render[c][0][t].push(m);
            }

            for (auto &m : materials_to_render[c][1][t]) {
                _materials_to_render[c][1][t].push(m);
            }

            for (auto &m : materials_to_render[c][2][t]) {
                _materials_to_render[c][2][t].push(m);
            }
        }
    }


    const auto machinery  = _tiles->category_tiles().find("Machinery");
    const auto machinery2 = _tiles->category_tiles().find("Machinery2");

    if (machinery != _tiles->category_tiles().end()) for (auto t : machinery->second) {
        if (t == nullptr) continue;
        if (t->get_width() >= 4 || t->get_height() >= 4) continue;
        _random_machines.push_back(t);
    }

    _preparation_done = true;
}

void Renderer::prepare() {
    if (_preparation_thread.joinable()) _preparation_thread.join();

    // _prepare();
    // _preparation_done = true;
    _preparation_thread = std::thread([this]() {this->_prepare(); this->_preparation_done = true;});
}

void Renderer::_set_render_progress(int step) {
    if (step == _render_progress) return;

    _render_progress = step;

    if (_logger == nullptr) return;

    switch (_render_progress) {
        case 1: _logger->info("[Renderer] rendering tiles"); break;
        case 2: _logger->info("[Renderer] rendering materials"); break;
        case 3: _logger->info("[Renderer] rendering props"); break;
        case 4: _logger->info("[Renderer] rendering geometry extras"); break;
        case 5: _logger->info("[Renderer] rendering effects"); break;
        case 6: _logger->info("[Renderer] rendering light"); break;
        case 7: _logger->info("[Renderer] generating text"); break;
        case 8: _logger->info("[Renderer] done"); break;
    }

}

bool Renderer::frame_render() {
    if (!_preparation_done) return false;

    if (_camera == nullptr) _camera = &_level->cameras[0];

    if (_render_progress == 0) _set_render_progress(RENDER_PROGRESS_TILES);

    if (_render_progress == RENDER_PROGRESS_TILES) {
        _draw_tiles_layer(_tile_layer_progress);
        _tile_layer_progress++;

        if (_tile_layer_progress >= 3) _set_render_progress(RENDER_PROGRESS_MATERIALS);
        return false;
    }

    if (_render_progress == RENDER_PROGRESS_MATERIALS) {

        if (_frame_render_materials_layer(_material_layer_progress, 600)) {
            _material_layer_progress++;
            _material_progress = 0;
            
            if (_material_layer_progress >= 3) {
                _set_render_progress(RENDER_PROGRESS_EXTRA);
            }
        }

        return false;
    }

    if (_render_progress == RENDER_PROGRESS_EXTRA) {
        _render_poles_layer(0);
        _render_poles_layer(1);
        _render_poles_layer(2);

        _set_render_progress(RENDER_PROGRESS_PROPS);

        return false;
    }

    if (_render_progress == RENDER_PROGRESS_PROPS) {

        for (auto &p : _level->props) {
            _draw_prop(p.get());
        }

        _set_render_progress(RENDER_PROGRESS_EFFECTS);
        return false;
    }

    if (_render_progress == RENDER_PROGRESS_EFFECTS) {

        _set_render_progress(RENDER_PROGRESS_LIGHT);
        return false;
    }

    if (_render_progress == RENDER_PROGRESS_LIGHT) {

        _set_render_progress(RENDER_PROGRESS_DONE);
        return false;
    }

    return _render_progress == RENDER_PROGRESS_DONE;
}

bool Renderer::_is_material(int x, int y, int z) {
    const auto &geos = _level->get_const_geo_matrix();
    const auto &tiles = _level->get_const_tile_matrix();

    if (!geos.is_in_bounds(x, y, z)) return false;
    if (!tiles.is_in_bounds(x, y, z)) return false;

    const matrix_t 
        mx = static_cast<matrix_t>(x), 
        my = static_cast<matrix_t>(y), 
        mz = static_cast<matrix_t>(z);

    const auto &geo = geos.get_const(mx, my, mz);

    if (!geo.is_solid() && !geo.is_slope()) 
        return false;

    const auto &tile = tiles.get_const(mx, my, mz);

    return tile.type == TileType::material || tile.type == TileType::_default;
}

bool Renderer::_is_material(int x, int y, int z, const MaterialDef *def) {
    const auto &geos = _level->get_const_geo_matrix();
    const auto &tiles = _level->get_const_tile_matrix();

    if (!geos.is_in_bounds(x, y, z)) return false;
    if (!tiles.is_in_bounds(x, y, z)) return false;

    const matrix_t 
        mx = static_cast<matrix_t>(x), 
        my = static_cast<matrix_t>(y), 
        mz = static_cast<matrix_t>(z);

    const auto &geo = geos.get_const(mx, my, mz);

    if (!geo.is_solid() && !geo.is_slope()) 
        return false;

    const auto &tile = tiles.get_const(mx, my, mz);

    if (tile.type == TileType::material) {
        return tile.material_def == def;
    } else if (tile.type == TileType::_default) {
        return def->get_name() == _level->default_material;
    }

    return false;
}

void Renderer::_render_poles_layer(uint8_t layer) {
    if (layer > 2) return;

    const auto &geos = _level->get_const_geo_matrix();

    BeginTextureMode(_layers[layer * 10 + 4]);

    for (int x = 0; x < columns; x++) {
        for (int y = 0; y < rows; y++) {
            const int mx = x + static_cast<int>(_camera->get_position().x/20);
            const int my = y + static_cast<int>(_camera->get_position().y/20);

            if (!geos.is_in_bounds(mx, my, layer)) continue;

            const auto &geo = geos.get_const(static_cast<matrix_t>(mx), static_cast<matrix_t>(my), layer);

            // if (geo.is_solid()) continue;

            if (geo.has_feature(GeoFeature::vertical_pole)) {
                DrawRectangleRec(
                    Rectangle{x * 20.0f + 8, y * 20.0f, 4.0f, 20.0f}, 
                    Color{255, 0, 0, 255}
                );
            }

            if (geo.has_feature(GeoFeature::horizontal_pole)) {
                DrawRectangleRec(
                    Rectangle{x * 20.0f, y * 20.0f + 8, 20.0f, 4.0f}, 
                    Color{255, 0, 0, 255}
                );
            }
        }
    }

    EndTextureMode();
}

};