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

    _level(nullptr)
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

    _logger->info("[Renderer] (initialization) reloading shaders");

    const auto &shaders_dir = _dirs->get_shaders();

    _white_remover = LoadShader(nullptr, (shaders_dir / "white_remover.frag").string().c_str());
    _bevel = LoadShader(nullptr, (shaders_dir / "bevel.frag").string().c_str());
    _invb = LoadShader((shaders_dir / "invb.vert").string().c_str(), (shaders_dir / "invb.frag").string().c_str());

    _initialized = true;
}

void Renderer::load(const Level *level) {
    _logger->info("[Renderer] loading level");

    if (!_initialized) throw std::runtime_error("renderer is uninitialized");
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