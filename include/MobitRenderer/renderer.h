#pragma once

#include <queue>
#include <vector>
#include <thread>
#include <atomic>
#include <string>
#include <random>
#include <cstdint>

#include <raylib.h>
#include <raymath.h>

#include <spdlog/spdlog.h>

#include <MobitRenderer/vec.h>
#include <MobitRenderer/quad.h>
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

    RenderConfig() :
        no_light(false),
        no_props(false),
        no_effects(false),
        no_tiles(false),
        skip_undefined(true),
        skip_undefined_effects(true),
        skip_undefined_materials(true),
        skip_undefined_tiles(true),
        skip_undefined_props(true),
        cameras({}) 
    {}
};

struct Render_TileCell {
    int rnd;
    matrix_t x, y, z;
    const TileCell *cell;

    Render_TileCell() : rnd(0), x(0), y(0), z(0), cell(nullptr) {}
    Render_TileCell(int rnd, matrix_t x, matrix_t y, matrix_t z, const TileCell *cell) :
        rnd(rnd),
        x(x), y(y), z(z),
        cell(cell)
    {}
};

struct Render_Material {
    const MaterialDef *def;
    Matrix<bool> matrix;

    Render_Material(const MaterialDef *def, matrix_t width, matrix_t height) :
        def(def),
        matrix(Matrix<bool>(width, height, 1)) 
    {}
};

class RandomGen {

private:

    uint32_t _seed, _init;

    
    inline static int _random_derive(uint32_t param) noexcept {
        auto var1 = static_cast<int>((param << 0xd ^ param) - (static_cast<int>(param) >> 0x15));
        auto var2 = static_cast<uint32_t>(((var1 * var1 * 0x3d73 + 0xc0ae5) * var1 + 0xd208dd0d & 0x7fffffff) + var1);
        return static_cast<int>((var2 * 0x2000 ^ var2) - (static_cast<int>(var2) >> 0x15));
    }
    
    public:
    
    inline void init_rng() noexcept {
        _seed = 1;
        _init = 0xA3000000;
    }

    // inline int next(uint max) noexcept {
    //     if (_seed == 0) init_rng();

    //     if ((_seed & 1) == 0) _seed = static_cast<uint>(_seed >> 1);
    //     else _seed = static_cast<uint>(_seed >> 1 ^ _init);

    //     auto var1 = _random_derive(static_cast<uint>(_seed * 0x47));
    //     if (max > 1) var1 = static_cast<int>((long)static_cast<ulong>(var1 & 0x7FFFFFFF) % static_cast<long>(max));
        
    //     return var1;
    // }

    inline int next(int max) noexcept {
        if (_seed == 0) init_rng();

        if ((_seed & 1) == 0) _seed = static_cast<uint32_t>(_seed >> 1);
        else _seed = static_cast<uint32_t>(_seed >> 1 ^ _init);

        auto var1 = _random_derive(static_cast<uint32_t>(_seed * 0x47));
        if (max > 1) var1 = static_cast<int>(static_cast<long>(static_cast<uint64_t>(var1 & 0x7FFFFFFF)) % static_cast<long>(max));
        
        return var1;
    }

    RandomGen(uint32_t seed) : _seed(seed), _init(0xA3000000) {}
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

    RenderConfig _config;

    Shader _vflip;
    int _vflip_texture_loc;

    /// @brief Removes the white background.
    Shader _white_remover;
    int _white_remover_texture_loc;

    Shader _white_remover_vflip;
    int _white_remover_vflip_texture_loc;

    Shader _composer;
    int _composer_texture_loc, _composer_tint_loc;

    /// @brief Adds highlight/shadow at the edges of an rgb texture and 
    /// removes the white background.
    Shader _bevel;
    int _bevel_texture_loc;
    int _bevel_tex_size_loc;
    int _bevel_thick_loc;
    int _bevel_highlight_loc;
    int _bevel_shadow_loc;
    int _bevel_vflip_loc;

    /// @brief Draws a texture using Inverse-Bilinear Interpolation algorithm
    /// and removes the white background.
    Shader _invb;
    int _invb_texture_loc;
    int _invb_vertices_loc;
    int _invb_tex_coord_loc;

    Shader _red_encoder;
    int _red_encoder_texture_loc;
    int _red_encoder_lightmap_loc;
    int _red_encoder_depth_loc;
    int _red_encoder_flipv_loc;

    Shader _rgb_apply_palette;
    int _rgb_apply_palette_texture_loc;
    int _rgb_apply_palette_palette_loc;
    int _rgb_apply_palette_lightmap_loc;
    int _rgb_apply_palette_depth_loc;
    int _rgb_apply_palette_vflip_loc;

    Shader _palette_sky;
    int _palette_sky_texture_loc;

    Shader _sill;
    int _sill_texture_loc;
    int _sill_invert_loc;
    int _sill_vflip_loc;

    Shader _cross_sill;
    int _cross_sill_texture_loc;
    int _cross_sill_sill_loc;
    int _cross_sill_invert_loc;
    int _cross_sill_vflip_loc;

    TileDex     *_tiles;
    PropDex     *_props;
    MaterialDex *_materials;
    CastLibs    *_castlibs;

    const Level *_level;

    // Must be terminated if not done in destructor.
    std::thread _preparation_thread;
    std::atomic<bool> _preparation_done;

    RandomGen _rand;

    bool _initialized, _cleaned_up;

    bool 
        _shaders_initialized,
        _layers_initialized,
        _dc_layers_initialized,
        _ga_layers_initialized,
        _gb_layers_initialized,
        _quadified_initialized,
        _lightmap_initialized,
        _final_initialized;

    bool
        _layers_cleaned,
        _dc_layers_cleaned,
        _ga_layers_cleaned,
        _gb_layers_cleaned,
        _quadified_cleaned,
        _lightmap_cleaned,
        _final_cleaned;

    size_t _layers_init_progress, 
        _layers_clean_progress, 
        _light_render_progress, 
        _quadify_progress;
    int _layers_compose_progress;

    size_t 
        _material_progress,
        _material_layer_progress,
        _material_progress_x,
        _material_progress_y;

    size_t _camera_index;
    LevelCamera const*_camera;

    /// 1 - tiles
    /// 2 - materials
    /// 3 - props
    /// 4 - effects
    /// 5 - light
    /// 6 - generate text
    /// 7 - done
    int _render_progress;

    void _set_render_progress(int);

    void _prepare();

    uint8_t _tile_layer_progress;

    std::vector<std::vector<Render_TileCell>> 
        _tiles_to_render1,
        _tiles_to_render2,
        _tiles_to_render3;

    // For each selected camera, an array of render type array of material cells.
    // accessed as the following:
    // array[selected_camera][material_rendertype][cell]
    // note: selected_camera is the index of _config.cameras.
    std::vector<std::vector<std::queue<Render_TileCell>>> 
        _materials_to_render1,
        _materials_to_render2,
        _materials_to_render3;

    
    bool _is_material(int x, int y, int z);
    bool _is_material(int x, int y, int z, const MaterialDef *def);

    /// A lot of draw calls here
    /// TODO: continue here.
    ///

    void _draw_tile_origin_mtx(TileDef *def, matrix_t x, matrix_t y, uint8_t layer) noexcept;
    void _draw_material_origin_mtx(MaterialDef *def, matrix_t x, matrix_t y, uint8_t layer) noexcept;
    void _draw_prop(Prop *prop) noexcept;

    void _draw_tiles_layer(uint8_t layer) noexcept;
    void _draw_materials_layer(uint8_t layer) noexcept;

    bool _frame_render_materials_layer(uint8_t layer, int threshold = 10);

    bool _frame_draw_materials_layer_unified(
        std::queue<Render_TileCell> *cells,
        uint8_t layer,
        int threshold = 10
    );

    bool _frame_render_bricks_layer(uint8_t layer, int threshold = 300);
    void _render_bricks_layer(uint8_t layer);
    void _render_standard_layer(uint8_t layer);
    void _render_chaotic_stone_layer(uint8_t layer);
    void _render_small_pipes_layer(uint8_t layer);


public:

    /// @brief The main working layers.
    RenderTexture2D _layers[30];

    RenderTexture2D _dc_layers[30];
    RenderTexture2D _ga_layers[30];
    RenderTexture2D _gb_layers[30];
    RenderTexture2D _quadified_layers[30];

    RenderTexture2D _material_canvas;

    RenderTexture2D _composed_layers;

    RenderTexture2D _composed_lightmap;
    RenderTexture2D _final_lightmap;

    /// @brief The final texture of the level.
    RenderTexture2D _final;

    inline void configure(const RenderConfig &c) noexcept { _config = c; }

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
    void frame_compose(
        int min_layer,
        int max_layer,
        float offsetx,
        float offsety,
        bool fog,
        int threshold = 10
    );

    #ifdef FEATURE_PALETTES
    void frame_compose_palette(
        int min_layer,
        int max_layer,
        float offsetx,
        float offsety,
        bool fog,
        const Palette &palette,
        int threshold = 10
    );
    #endif

    bool frame_quadify_layers(int threshold = 10);

    bool frame_render_light(int threshold = 10);

    void frame_render_final(int threshold = 10);

    /// @brief Loads a level to be rendered,
    /// @throw std::invalid_argument if the level pointer is nullptr. 
    void load(const Level*);

    /// @brief Loads data dependant on the level state on a background thread.
    /// When it's done, preparation done is set to true.
    void prepare();
    inline bool is_preparation_done() const noexcept { return _preparation_done; }

    inline int get_render_progress() const noexcept { return _render_progress; }
    inline bool is_light_render_done() const noexcept { return _light_render_progress >= 29; }
    inline bool is_quadification_done() const noexcept { return _quadify_progress >= 29; }

    /// @brief Renders a portion of the level at a time.
    /// @return Returns true if the level is not completely done.
    bool frame_render();

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
