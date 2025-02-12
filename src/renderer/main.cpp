#include <vector>
#include <thread>
#include <atomic>
#include <string>
#include <stdexcept>

#include <raylib.h>

#include <spdlog/spdlog.h>

#include <MobitRenderer/renderer.h>
#include <MobitRenderer/dirs.h>
#include <MobitRenderer/level.h>
#include <MobitRenderer/state.h>
#include <MobitRenderer/matrix.h>
#include <MobitRenderer/definitions.h>

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
    
    _preparation_done(false),
    _initialized(false),

    _level(nullptr),

    _shaders_initialized(false),
    _layers_initialized(false),
    _dc_layers_initialized(false),
    _ga_layers_initialized(false),
    _gb_layers_initialized(false),
    _final_initialized(false),

    _layers_cleaned(false),
    _dc_layers_cleaned(false),
    _ga_layers_cleaned(false),
    _gb_layers_cleaned(false),
    _final_cleaned(false),

    _layers_init_progress(0),
    _layers_clean_progress(0),
    _layers_compose_progress(29)
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
        _bevel = LoadShader(nullptr, (shaders_dir / "bevel.frag").string().c_str());
        _invb = LoadShader((shaders_dir / "invb.vert").string().c_str(), (shaders_dir / "invb.frag").string().c_str());

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

    _layers_clean_progress = 0;

    if (!_final_cleaned) {
        BeginTextureMode(_final);
        ClearBackground(WHITE);
        EndTextureMode();
        _final_cleaned = true;
        return false;
    }

    _layers_clean_progress = 0;

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
        
        BeginShaderMode(_white_remover);
        SetShaderValueTexture(_white_remover, GetShaderLocation(_white_remover, "texture0"), l.texture);
        DrawTexture(l.texture, 5 - _layers_compose_progress, 5 - _layers_compose_progress, WHITE);
        DrawRectangle(0, 0, 1400, 800, Color{255, 255, 255, static_cast<unsigned char>(2 * _layers_compose_progress)});
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

void Renderer::prepare() {
    if (_preparation_thread.joinable()) _preparation_thread.join();

    _preparation_done = false;
    _preparation_thread = std::thread([this](){
        _preparation_done = true;
    });
}

};