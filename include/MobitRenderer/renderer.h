#pragma once

#include <vector>
#include <thread>
#include <atomic>
#include <string>

#include <raylib.h>
#include <raymath.h>

#include <spdlog/spdlog.h>

#include <MobitRenderer/dirs.h>
#include <MobitRenderer/level.h>
#include <MobitRenderer/state.h>
#include <MobitRenderer/matrix.h>
#include <MobitRenderer/definitions.h>

namespace mr::renderer {

class render_error : public std::exception {
private:
    std::string msg_;

public:
    explicit render_error(const std::string &);
    explicit render_error(const std::string &, const std::exception &);
    const char *what() const noexcept override;
};

struct RenderConfig {

    /// @brief Render level without light.
    bool no_light;
    
    /// @brief Render level without props.
    bool no_props;

    /// @brief Render level without effects.
    bool no_effects;

    /// @brief Render level without tiles.
    bool no_tiles;

    /// @brief Skips any undefined material without 
    /// throwing an exception.
    bool skip_undefined_materials;

    /// @brief Skips any undefined effect 
    /// without throwing an exception.
    bool skip_undefined_effects;

    /// @brief Skips any undefined prop 
    /// without throwing an exception.
    bool skip_undefined_props;
    
    /// @brief Skips any undefined tile 
    /// without throwing an exception.
    bool skip_undefined_tiles;
    
    /// @brief Skips any undefined reference 
    /// (tile, prop, etc..) without throwing an exception.
    bool skip_undefined;

    /// @brief An array of selected cameras (indices) to render.
    std::vector<size_t> cameras;
};

/// @brief Responsible for rendering the final level image.
/// @attention Requires OpenGL context.
class Renderer {

protected:

    /// @brief The width of the working level in matrix units.
    static const int columns = 100;

    /// @brief The height of the working level in matrix units.
    static const int rows = 60;

    /// @brief The width of the working level layer texture.
    static const int work_width = 2000;

    /// @brief The height of the working level layer texture.
    static const int work_height = 1200;

    /// @brief The width of the final level image.
    static const int final_width = 1400;

    /// @brief The height of the final level image.
    static const int final_height = 800;

    std::shared_ptr<Dirs> _dirs;
    std::shared_ptr<spdlog::logger> _logger;

    /// @brief Removes the white background.
    Shader _white_remover;

    /// @brief Adds highlight/shadow at the edges of an rgb texture and 
    /// removes the white background.
    Shader _bevel;

    /// @brief Draws a texture using Inverse-Bilinear Interpolation algorithm
    /// and removes the white background.
    Shader _invb;

    TileDex     *_tiles;
    PropDex     *_props;
    MaterialDex *_materials;
    CastLibs    *_castlibs;

    const Level *_level;

    // Must be terminated if not done in destructor.
    std::thread _preparation_thread;
    std::atomic<bool> _preparation_done;

    bool _initialized, _cleaned_up;

    bool 
        _shaders_initialized,
        _layers_initialized,
        _dc_layers_initialized,
        _ga_layers_initialized,
        _gb_layers_initialized,
        _final_initialized;

    bool
        _layers_cleaned,
        _dc_layers_cleaned,
        _ga_layers_cleaned,
        _gb_layers_cleaned,
        _final_cleaned;

    size_t _layers_init_progress, _layers_clean_progress;
    int _layers_compose_progress;

    /// A lot of draw calls here
    /// TODO: continue here.
    ///

public:

    /// @brief The main working layers.
    RenderTexture2D _layers[30];

    RenderTexture2D _dc_layers[30];
    RenderTexture2D _ga_layers[30];
    RenderTexture2D _gb_layers[30];

    RenderTexture2D _composed_layers;

    /// @brief The final texture of the level.
    RenderTexture2D _final;

    /// @brief Initializes render textures and data; happens independently
    /// from the state of the level.
    /// @attention Requires OpenGL context.
    void initialize();

    inline bool is_initialized() const noexcept { return _initialized; }
    inline bool is_cleaned() const noexcept { return _cleaned_up; }
    inline void reset_cleaned() noexcept { _cleaned_up = false; }

    /// @brief Initializes render textures and data partially each frame; happens independently
    /// from the state of the level.
    /// @attention Requires OpenGL context.
    bool frame_initialize(int threshold = 10);

    /// @brief Cleans up render textures partially each frame.
    /// @attention Requires OpenGL context.
    bool frame_cleanup(int threshold = 10);

    void frame_compose(int threshold = 10);

    /// @brief Loads a level to be rendered,
    /// @throw std::invalid_argument if the level pointer is nullptr. 
    void load(const Level*);

    /// @brief Loads data dependant on the level state on a background thread.
    /// When it's done, preparation done is set to true.
    void prepare();
    inline bool is_preparation_done() const noexcept { return _preparation_done; }

    /// @brief Renders a portion of the level at a time.
    /// @return Returns true if the level is not completely done.
    bool render_frame();

    Renderer &operator=(Renderer const&) = delete;

    Renderer(
        std::shared_ptr<Dirs>,
        std::shared_ptr<spdlog::logger>,
        TileDex*,
        PropDex*,
        MaterialDex*,
        CastLibs*
    );
    Renderer(Renderer const&) = delete;
    virtual ~Renderer();
};

};
