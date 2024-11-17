#pragma once

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <memory>
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
  std::filesystem::path shaders, materials, tiles, props, cast;

public:
  const std::filesystem::path &get_executable() const;

  const std::filesystem::path &get_assets() const;
  const std::filesystem::path &get_projects() const;
  const std::filesystem::path &get_levels() const;
  const std::filesystem::path &get_data() const;
  const std::filesystem::path &get_logs() const;

  const std::filesystem::path &get_shaders() const;
  const std::filesystem::path &get_materials() const;
  const std::filesystem::path &get_tiles() const;
  const std::filesystem::path &get_props() const;
  const std::filesystem::path &get_cast() const;

  dirs();
  dirs(const std::filesystem::path &);
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

class context {
private:
  Camera2D camera;

  std::vector<mr::Level> levels;
  uint8_t selected_level;

public:
  std::shared_ptr<spdlog::logger> logger;
  std::shared_ptr<dirs> directories;
  std::shared_ptr<textures> textures_;
  std::shared_ptr<debug::f3> f3_;
  Camera2D &get_camera();
  void set_camera(Camera2D);
  bool f3_enabled;

  const std::vector<mr::Level> &get_levels() const noexcept;
  mr::Level &get_selected_level();
  void add_level(mr::Level &&);
  void add_level(mr::Level const &);
  void remove_level(size_t);

  context();
  context(std::shared_ptr<spdlog::logger>, std::shared_ptr<dirs>);
  context(std::shared_ptr<spdlog::logger>, std::shared_ptr<dirs>,
          std::shared_ptr<textures>);
  ~context();
};
}; // namespace mr
