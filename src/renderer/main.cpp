#include <vector>
#include <thread>
#include <atomic>
#include <string>
#include <random>
#include <stdexcept>
#include <algorithm>

#include <raylib.h>

#include <spdlog/spdlog.h>

#include <MobitRenderer/renderer.h>
#include <MobitRenderer/dirs.h>
#include <MobitRenderer/level.h>
#include <MobitRenderer/state.h>
#include <MobitRenderer/matrix.h>
#include <MobitRenderer/definitions.h>

#define RENDER_PROGRESS_TILES 1
#define RENDER_PROGRESS_MATERIALS 2
#define RENDER_PROGRESS_PROPS 3
#define RENDER_PROGRESS_EFFECTS 4
#define RENDER_PROGRESS_LIGHT 5
#define RENDER_PROGRESS_TEXT 6
#define RENDER_PROGRESS_DONE 7

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

    _white_remover_texture_loc(0),
    _white_remover_vflip_texture_loc(0),
    _bevel_texture_loc(0),
    _invb_texture_loc(0),
    
    _preparation_done(false),
    _initialized(false),

    _level(nullptr),

    _shaders_initialized(false),
    _layers_initialized(false),
    _dc_layers_initialized(false),
    _ga_layers_initialized(false),
    _gb_layers_initialized(false),
    _lightmap_initialized(false),
    _final_initialized(false),

    _layers_cleaned(false),
    _dc_layers_cleaned(false),
    _ga_layers_cleaned(false),
    _gb_layers_cleaned(false),
    _lightmap_cleaned(false),
    _final_cleaned(false),

    _tiles_to_render1({}),
    _tiles_to_render2({}),
    _tiles_to_render3({}),

    _layers_init_progress(0),
    _layers_clean_progress(0),
    _layers_compose_progress(29),

    _render_progress(0),

    _tile_layer_progress(0),
    _rand(0)
{}

Renderer::~Renderer() {
    if (_initialized) {
        UnloadShader(_white_remover);
        UnloadShader(_bevel);
        UnloadShader(_invb);

        for (size_t l = 0; l < 30; l++) {
            UnloadRenderTexture(_layers[l]);
            UnloadRenderTexture(_dc_layers[l]);
            UnloadRenderTexture(_ga_layers[l]);
            UnloadRenderTexture(_gb_layers[l]);
        }

        UnloadRenderTexture(_final);
        UnloadRenderTexture(_composed_layers);
    
        if (_preparation_thread.joinable()) _preparation_thread.join();
    }
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
            _layers[l]    = LoadRenderTexture(work_width, work_height);
            _dc_layers[l] = LoadRenderTexture(work_width, work_height);
            _ga_layers[l] = LoadRenderTexture(work_width, work_height);
            _ga_layers[l] = LoadRenderTexture(work_width, work_height);
            _gb_layers[l] = LoadRenderTexture(work_width, work_height);
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

        _white_remover = LoadShader(nullptr, (shaders_dir / "white_remover.frag").string().c_str());
        _white_remover_vflip = LoadShader(nullptr, (shaders_dir / "white_remover_vflip.frag").string().c_str());
        _composer = LoadShader(nullptr, (shaders_dir / "white_remover_apply_white_tint_vflip.frag").string().c_str());
        _bevel = LoadShader(nullptr, (shaders_dir / "bevel.frag").string().c_str());
        _invb = LoadShader((shaders_dir / "invb.vert").string().c_str(), (shaders_dir / "invb.frag").string().c_str());
        
        _white_remover_texture_loc = GetShaderLocation(_white_remover, "texture0");
        _white_remover_vflip_texture_loc = GetShaderLocation(_white_remover_vflip, "texture0");
        _composer_texture_loc = GetShaderLocation(_composer, "texture0");
        _composer_tint_loc = GetShaderLocation(_composer, "tint");
        _bevel_texture_loc = GetShaderLocation(_bevel, "texture0");
        _invb_texture_loc = GetShaderLocation(_invb, "texture0");

        _shaders_initialized = true;
        return false;
    }

    if (!_layers_initialized) {
        int progress = 0;

        while (progress < threshold && (_layers_init_progress) < 30) {
            _layers[_layers_init_progress] = LoadRenderTexture(work_width, work_height);
            
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
            _dc_layers[_layers_init_progress] = LoadRenderTexture(work_width, work_height);
            
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
            _ga_layers[_layers_init_progress] = LoadRenderTexture(work_width, work_height);

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
            _gb_layers[_layers_init_progress] = LoadRenderTexture(work_width, work_height);
        
            _layers_init_progress++;
            progress++;
        }

        _gb_layers_initialized = _layers_init_progress > 29;
        if (_gb_layers_initialized) _layers_init_progress = 0;
        return false;
    }

    if (!_lightmap_initialized) {
        _final_lightmap = LoadRenderTexture(final_width, final_height);

        _lightmap_initialized = true;
    }

    if (!_final_initialized) {
        _final = LoadRenderTexture(final_width, final_height);
        _composed_layers = LoadRenderTexture(final_width, final_height);
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
        ClearBackground(WHITE);
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
    while (progress < threshold && (_layers_compose_progress) >= 0) {
        const auto &l = _layers[_layers_compose_progress];
        if (_layers_compose_progress == 29) ClearBackground(WHITE);
        
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

void Renderer::load(const Level *level) {
    _logger->info("[Renderer] loading level");

    if (level == nullptr) throw std::invalid_argument("level is nullptr");

    _level = level;
}

void Renderer::_prepare() {
    if (_level == nullptr) return;

    _rand = RandomGen(_level->seed);

    const auto &mtx = _level->get_const_tile_matrix();

    _tiles_to_render1.clear();
    _tiles_to_render2.clear();
    _tiles_to_render3.clear();

    for (matrix_t x = 0; x < mtx.get_width(); x++) {
        for (matrix_t y = 0; y < mtx.get_height(); y++) {
            const auto *cell1 = mtx.get_const_ptr(x, y, 0);
            const auto *cell2 = mtx.get_const_ptr(x, y, 1);
            const auto *cell3 = mtx.get_const_ptr(x, y, 2);

            if (cell1 != nullptr && cell1->type == TileType::head && cell1->tile_def != nullptr) {
                _tiles_to_render1.push_back(Render_TileCell{ _rand.next(100000), x, y, 0, cell1 });
            }

            if (cell2 != nullptr && cell2->type == TileType::head && cell2->tile_def != nullptr) {
                _tiles_to_render2.push_back(Render_TileCell{ _rand.next(100000), x, y, 1, cell2 });
            }

            if (cell3 != nullptr && cell3->type == TileType::head && cell3->tile_def != nullptr) {
                _tiles_to_render3.push_back(Render_TileCell{ _rand.next(100000), x, y, 2, cell3 });
            }
        }
    }

    const auto sort_alg = [](const Render_TileCell &c1, const Render_TileCell &c2) { return c1.rnd < c2.rnd; };

    std::sort(
        _tiles_to_render1.begin(), 
        _tiles_to_render1.end(), 
        sort_alg
    );

    std::sort(
        _tiles_to_render2.begin(), 
        _tiles_to_render2.end(), 
        sort_alg
    );

    std::sort(
        _tiles_to_render3.begin(), 
        _tiles_to_render3.end(), 
        sort_alg
    );

    _preparation_done = true;
}

void Renderer::prepare() {
    if (_preparation_thread.joinable()) _preparation_thread.join();

    _prepare();
    _preparation_done = true;
    // _preparation_thread = std::thread([this]() {this->_prepare();});
}

void Renderer::_set_render_progress(int step) {
    if (step == _render_progress) return;

    _render_progress = step;

    if (_logger == nullptr) return;

    switch (_render_progress) {
        case 1: _logger->info("[Renderer] rendering tiles"); break;
        case 2: _logger->info("[Renderer] rendering materials"); break;
        case 3: _logger->info("[Renderer] rendering props"); break;
        case 4: _logger->info("[Renderer] rendering effects"); break;
        case 5: _logger->info("[Renderer] rendering light"); break;
        case 6: _logger->info("[Renderer] generating text"); break;
        case 7: _logger->info("[Renderer] done"); break;
    }

}

bool Renderer::frame_render() {
    if (!_preparation_done) return false;

    if (_render_progress == 0) _set_render_progress(RENDER_PROGRESS_TILES);

    if (_render_progress == RENDER_PROGRESS_TILES) {
        _draw_tiles_layer(_tile_layer_progress);
        _tile_layer_progress++;

        if (_tile_layer_progress >= 3) _set_render_progress(RENDER_PROGRESS_MATERIALS);
        return false;
    }

    if (_render_progress == RENDER_PROGRESS_MATERIALS) {

        // _set_render_progress(RENDER_PROGRESS_PROPS);
        return false;
    }

    if (_render_progress == RENDER_PROGRESS_PROPS) {

        // _set_render_progress(RENDER_PROGRESS_EFFECTS);
        return false;
    }

    if (_render_progress == RENDER_PROGRESS_EFFECTS) {

        // _set_render_progress(RENDER_PROGRESS_LIGHT);
        return false;
    }

    if (_render_progress == RENDER_PROGRESS_LIGHT) {

        // _set_render_progress(RENDER_PROGRESS_DONE);
        return false;
    }

    return _render_progress == RENDER_PROGRESS_DONE;
}

};