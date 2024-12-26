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

#ifdef __linux__
#include <limits.h>
#include <unistd.h>
#endif

#ifdef __APPLE__
#include <limits.h>
#include <mach-o/dyld.h>
#endif

#ifdef _WIN32
#include <windows.h>
#endif

#include <MobitRenderer/draw.h>
#include <MobitRenderer/level.h>
#include <MobitRenderer/managed.h>

namespace mr {
inline std::filesystem::path get_current_dir() {
  std::filesystem::path this_dir;

#ifdef __linux__
  {
    char result[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);

    if (count == -1)
      throw "could not retrieve executable's path";
    result[count] = '\0';
    this_dir = std::filesystem::absolute(result).parent_path();
  }
#endif

#ifdef __APPLE__
  throw mr::unsupported();
#endif

#ifdef _WIN32
  throw mr::unsupported();
#endif

  return this_dir;
}

class dirs {
private:
  std::filesystem::path executable;
  std::filesystem::path assets, projects, levels, data, logs;
  std::filesystem::path shaders, fonts, materials, tiles, props, cast;

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

  dirs();
  dirs(const std::filesystem::path &);
};

class shaders {
private:
  std::filesystem::path _shaders_dir;

  Shader _vflip;

public:
  /// @attention Requires OpenGL context.
  void reload_all();

  const Shader &vflip() const noexcept;

  shaders(std::filesystem::path shaders_dir);
  ~shaders();
};

class textures {
private:
  std::shared_ptr<dirs> directories;

public:
  rendertexture main_level_viewport;

  texture file_icon, folder_icon, up_icon, home_icon;

  void reload_all_textures();

  /// @brief Returns a read-only reference to the main level viewport.
  /// @warning Do not access this while modifying main_level_viewport.
  const RenderTexture2D &get_main_level_viewport() const noexcept;

  /// @brief Creates a new render texture and draws the old render texture over.
  void resize_main_level_viewport(int width, int height);

  textures(std::shared_ptr<dirs>, bool preload_textures = false);
  ~textures();
};

class shortcuts {
private:
public:
  shortcuts();
  ~shortcuts();
};

enum class context_event_type { level_loaded };

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
};

struct config {
  bool splashscreen, f3, crash_on_esc, blue_screen_of_death;

  uint16_t event_handle_per_frame, load_per_frame;
  bool list_wrap, strict_deserialization;

  SpriteVisiblity props_visibility, tiles_visibility, water_visibility,
      materials_visibility;
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

public:
  std::shared_ptr<spdlog::logger> logger;
  std::shared_ptr<dirs> directories;
  std::unique_ptr<textures> textures_;
  std::shared_ptr<debug::f3> f3_;
  std::queue<context_event> events;
  Camera2D &get_camera();
  void set_camera(Camera2D);
  bool f3_enabled, enable_global_shortcuts;

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

  context();
  context(std::shared_ptr<spdlog::logger>, std::shared_ptr<dirs>);
  context(std::shared_ptr<spdlog::logger>, std::shared_ptr<dirs>,
          std::unique_ptr<textures>);
  ~context();
};

}; // namespace mr
