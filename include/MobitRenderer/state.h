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

namespace mr {

class dirs {
private:
  std::filesystem::path executable;
  std::filesystem::path assets, projects, levels, data, logs;
  std::filesystem::path shaders, fonts, materials, tiles, props, cast;

  bool 
    ok, 
    executable_found,
    
    assets_found,
    shaders_found,
    fonts_found,
    
    data_found,
    tiles_found,
    materials_found,
    props_found,
    cast_found,
    
    projects_found,
    levels_found,
    logs_found;
public:
  const std::filesystem::path &get_executable() const;

  const std::filesystem::path &get_assets() const;
  const std::filesystem::path &get_projects() const;
  const std::filesystem::path &get_levels() const;
  const std::filesystem::path &get_data() const;
  const std::filesystem::path &get_logs() const;

  const std::filesystem::path &get_shaders() const;
  const std::filesystem::path &get_fonts() const;
  const std::filesystem::path &get_materials() const;
  const std::filesystem::path &get_tiles() const;
  const std::filesystem::path &get_props() const;
  const std::filesystem::path &get_cast() const;

  inline bool is_ok() const noexcept { return ok; } 
  inline bool is_executable_found() const noexcept { return executable_found; }
  
  inline bool is_assets_found() const noexcept { return assets_found; }
  inline bool is_shaders_found() const noexcept { return shaders_found; }
  inline bool is_fonts_found() const noexcept { return fonts_found; }
  
  inline bool is_data_found() const noexcept { return data_found; }
  inline bool is_tiles_found() const noexcept { return tiles_found; }
  inline bool is_materials_found() const noexcept { return materials_found; }
  inline bool is_props_found() const noexcept { return props_found; }
  inline bool is_cast_found() const noexcept { return cast_found; }
  
  inline bool is_projects_found() const noexcept { return projects_found; }
  inline bool is_levels_found() const noexcept { return levels_found; }
  inline bool is_logs_found() const noexcept { return logs_found; }

  dirs();
  dirs(const std::filesystem::path &);
};

/// @note Requires shaders to be loaded.
class shaders {
private:
  std::filesystem::path _shaders_dir;

  Shader _vflip;
  Shader _white_remover;
  Shader _white_remover_apply_color;

public:
  /// @attention Requires OpenGL context.
  void unload_all();
  /// @attention Requires OpenGL context.
  void reload_all();

  const Shader &vflip() const noexcept;
  const Shader &white_remover() const noexcept;
  const Shader &white_remover_apply_color() const noexcept;

  shaders &operator=(shaders const&);
  shaders &operator=(shaders &&) noexcept;

  shaders(shaders const&);
  shaders(shaders &&) noexcept;

  shaders() = delete;
  shaders(std::filesystem::path shaders_dir);
  ~shaders();
};

class textures {
private:
  std::shared_ptr<dirs> directories;

public:
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

  texture file_icon, folder_icon, up_icon, home_icon;

  GE_Textures geometry_editor;

  void reload_all_textures();

  /// @brief Returns a read-only reference to the main level viewport.
  /// @warning Do not access this while modifying main_level_viewport.
  const RenderTexture2D &get_main_level_viewport() const noexcept;

  /// @brief Resizes all texture buffers that are associated with the current level.
  /// @param width Level width in pixels.
  /// @param height Level height in pixels.
  void resize_all_level_buffers(int width, int height);

  textures(std::shared_ptr<dirs>, bool preload_textures = false);
  ~textures();
};

class shortcuts {
private:
public:
  shortcuts();
  ~shortcuts();
};

enum class context_event_type { level_loaded, goto_page };

struct context_event {
  context_event_type type;
  std::any payload;
};

struct SpriteVisiblity {
  bool inherit, visible;
  uint8_t opacity;

  SpriteVisiblity(bool inherit = true, bool visible = true,
                  uint8_t opacity = 255);
};

struct SpritePrerender {
  bool tinted, preview, palette;

  SpritePrerender(bool tinted = false, bool preview = true, bool palette = false);
};

struct config {
  bool splashscreen, f3, crash_on_esc, blue_screen_of_death;

  uint16_t event_handle_per_frame, load_per_frame;
  bool list_wrap, strict_deserialization;

  SpriteVisiblity props_visibility, tiles_visibility, water_visibility,
      materials_visibility, grid;
  SpritePrerender tiles_prerender, props_prerender, materials_prerender;

  bool shadows;

  config();
};

class context {
private:
  Camera2D camera;

  std::vector<mr::Level *> levels;
  uint8_t selected_level;

  std::vector<Font> fonts;
  uint8_t selected_font;

  shaders _shaders;

  config _config;

public:
  std::shared_ptr<spdlog::logger> logger;
  std::shared_ptr<dirs> directories;
  textures *textures_;
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

  const Font *get_selected_font_const_ptr() const noexcept;
  Font get_selected_font() const noexcept;
  void select_font(uint8_t) noexcept;
  void add_font(Font) noexcept;

  shaders &get_shaders() noexcept;
  const shaders &get_shaders_const() const noexcept;

  const config &get_config_const() const noexcept;
  config &get_config() noexcept;
  const config *get_config_const_ptr() const noexcept;
  config *get_config_ptr() noexcept;
  void set_config(config) noexcept;

  context() = delete;
  context(std::shared_ptr<spdlog::logger>, std::shared_ptr<dirs>, textures*);
  ~context();
};

}; // namespace mr
