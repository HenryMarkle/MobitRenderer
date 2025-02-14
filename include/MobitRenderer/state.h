#pragma once

#include <any>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <queue>
#include <vector>

#include <spdlog/spdlog.h>

#include <raylib.h>

#include <MobitRenderer/draw.h>
#include <MobitRenderer/level.h>
#include <MobitRenderer/managed.h>
#include <MobitRenderer/exceptions.h>
#include <MobitRenderer/atlas.h>
#include <MobitRenderer/dex.h>
#include <MobitRenderer/castlibs.h>
#include <MobitRenderer/config.h>
#include <MobitRenderer/dirs.h>

namespace mr {

class fonts {

private:

const std::filesystem::path fonts_dir;
Font small_default_font;
Font large_default_font;
bool loaded;

public:

inline const Font &get_small_default_font() const noexcept { return small_default_font; }
inline const Font &get_large_default_font() const noexcept { return large_default_font; }

inline void unload_all() {
  if (!loaded) return;

  UnloadFont(small_default_font);
  UnloadFont(large_default_font);

  loaded = false;
}

void load_all();

inline void reload_all() {
  unload_all();
  load_all();
}

fonts &operator=(fonts&&) noexcept = delete;
fonts &operator=(fonts const&) = delete;

fonts(const std::filesystem::path &fonts_dir);
fonts(fonts&&) noexcept = delete;
fonts(fonts const&) = delete;
~fonts();

};

/// @note Requires shaders to be loaded.
class shaders {
private:
  std::filesystem::path _shaders_dir;

  Shader _ink;
  Shader _vflip;
  Shader _apply_alpha;
  Shader _apply_alpha_vflip;
  Shader _white_remover;
  Shader _white_remover_apply_color_vflip;
  Shader _white_remover_apply_color;
  Shader _white_remover_apply_alpha;
  Shader _white_remover_rgb_recolor;
  Shader _voxel_struct;
  Shader _default_prop;
  Shader _soft;
  Shader _varied_soft;
  Shader _voxel_struct_tinted;
  Shader _varied_voxel_struct;

  bool loaded;

public:
  /// @attention Requires OpenGL context.
  void unload_all();
  /// @attention Requires OpenGL context.
  void load_all();
  /// @attention Requires OpenGL context.
  void reload_all();

  inline bool is_loaded() const noexcept { return loaded; }

  inline const Shader &ink() const noexcept { return _ink; }
  inline const Shader &vflip() const noexcept { return _vflip; }
  inline const Shader &apply_alpha() const noexcept { return _apply_alpha; }
  inline const Shader &apply_alpha_vflip() const noexcept { return _apply_alpha_vflip; }
  inline const Shader &white_remover() const noexcept { return _white_remover; }
  inline const Shader &white_remover_apply_color_vflip() const noexcept { return _white_remover_apply_color_vflip; }
  inline const Shader &white_remover_apply_color() const noexcept { return _white_remover_apply_color; }
  inline const Shader &white_remover_apply_alpha() const noexcept { return _white_remover_apply_alpha; }
  inline const Shader &white_remover_rgb_recolor() const noexcept { return _white_remover_rgb_recolor; }
  inline const Shader &voxel_struct() const noexcept { return _voxel_struct; }
  inline const Shader &default_prop() const noexcept { return _default_prop; }
  inline const Shader &soft() const noexcept { return _soft; }
  inline const Shader &varied_soft() const noexcept { return _varied_soft; }
  inline const Shader &voxel_struct_tinted() const noexcept { return _voxel_struct_tinted; }
  inline const Shader &varied_voxel_struct() const noexcept { return _varied_voxel_struct; }

  shaders &operator=(shaders const&) = delete;
  shaders &operator=(shaders &&) noexcept;

  shaders(shaders const&) = delete;
  shaders(shaders &&) noexcept;

  shaders() = delete;
  shaders(std::filesystem::path shaders_dir);
  ~shaders();
};

/// @attention Must be destroyed (or unloaded) before CloseWindow().
class Palette {

private:
  
  std::string name;
  Texture2D texture;
  bool loaded;

public:
  
  std::filesystem::path texture_path;

  inline const std::string &get_name() const noexcept { return name; }
  inline bool is_loaded() const noexcept { return loaded; }
  void load();
  void unload();
  inline void reload() { unload(); load(); }
  inline const Texture &get_texture() const noexcept { return texture; }
  inline const Texture &get_loaded_texture() { load(); return texture; }
  
  Palette &operator=(Palette const&) = delete;
  Palette &operator=(Palette&&) noexcept;

  Palette(const std::string&, const std::filesystem::path&);
  Palette(Palette const&) = delete;
  Palette(Palette&&) noexcept;
  ~Palette();

};

class textures {
private:
  std::shared_ptr<Dirs> directories;
  std::vector<Palette> palettes;

public:

  // Shouldn't these be stored in the Level class?

  rendertexture main_level_viewport;

  rendertexture 
    geo_layer1, 
    geo_layer2, 
    geo_layer3;

  rendertexture 
    feature_layer1, 
    feature_layer2, 
    feature_layer3;

  rendertexture
    tile_layer1,
    tile_layer2,
    tile_layer3;

  /// @note This buffer's size is wider by 400px and higher by 400px
  /// than the level, to show out-of-bounds props.
  rendertexture props;

  texture file_icon, folder_icon, up_icon, home_icon;

  GE_Textures geometry_editor;
  LE_Textures light_editor;
  CE_Textures cameras_editor;

  void reload_all_textures();

  /// @brief Returns a read-only reference to the main level viewport.
  /// @warning Do not access this while modifying main_level_viewport.
  const RenderTexture2D &get_main_level_viewport() const noexcept;

  /// @brief Resizes all texture buffers that are associated with the current level.
  /// @param width Level width in pixels.
  /// @param height Level height in pixels.
  void resize_all_level_buffers(int width, int height);

  inline const std::vector<Palette>& get_palettes() const noexcept { return palettes; }
  void reload_palettes();

  textures(std::shared_ptr<Dirs>, bool preload_textures = false);
  ~textures();
};

enum class context_event_type { level_loaded, level_selected, goto_page };

struct context_event {
  context_event_type type;
  std::any payload;
};

class context {

private:

  Camera2D camera;

  std::vector<mr::Level *> levels;
  uint8_t selected_level;

  std::shared_ptr<Config> _config;

public:

  std::shared_ptr<spdlog::logger> logger;
  std::shared_ptr<Dirs> directories;

  // DO NOT DELETE THESE

  shaders *_shaders;
  textures *_textures;

  TileDex *_tiledex;
  PropDex *_propdex;
  MaterialDex *_materialdex;

  CastLibs *_castlibs;

  fonts *_fonts;

  //
  
  std::shared_ptr<mr::debug::f3> f3_;
  std::queue<context_event> events;
  Camera2D &get_camera();
  void set_camera(Camera2D);
  bool f3_enabled, enable_global_shortcuts;
  uint8_t level_layer_; // 0, 1, 2

  const std::vector<mr::Level *> &get_levels() const noexcept;
  mr::Level *get_selected_level();
  uint8_t get_level_index() const noexcept;
  void select_level(uint8_t);
  void add_level(std::unique_ptr<mr::Level>);
  void add_level(mr::Level *);
  void remove_level(size_t);
  void lock_global_shortcuts() noexcept;
  void unlock_global_shortcuts() noexcept;

  inline std::shared_ptr<Config> get_config() const noexcept { return _config; }
  void set_config(Config const&) noexcept;

  context() = delete;
  context(std::shared_ptr<spdlog::logger>, std::shared_ptr<Dirs>);
  ~context();
};

}; // namespace mr
