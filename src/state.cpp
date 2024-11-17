#include <filesystem>
#include <memory>
#include <vector>

#include <spdlog/spdlog.h>

#include <raylib.h>

#include <MobitRenderer/draw.h>
#include <MobitRenderer/level.h>
#include <MobitRenderer/managed.h>
#include <MobitRenderer/state.h>

namespace mr {

const std::filesystem::path &dirs::get_executable() const { return executable; }

const std::filesystem::path &dirs::get_assets() const { return assets; }
const std::filesystem::path &dirs::get_projects() const { return projects; }
const std::filesystem::path &dirs::get_levels() const { return levels; }
const std::filesystem::path &dirs::get_data() const { return data; }
const std::filesystem::path &dirs::get_logs() const { return logs; }

const std::filesystem::path &dirs::get_shaders() const { return shaders; }
const std::filesystem::path &dirs::get_materials() const { return materials; }
const std::filesystem::path &dirs::get_tiles() const { return tiles; }
const std::filesystem::path &dirs::get_props() const { return props; }
const std::filesystem::path &dirs::get_cast() const { return cast; }

dirs::dirs() {
#ifdef __linux__
  {
    char result[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);

    if (count == -1)
      throw "could not retrieve executable's path";
    result[count] = '\0';
    executable = std::filesystem::absolute(result).parent_path();
  }
#endif

  assets = executable / "Assets";
  projects = executable / "Projects";
  levels = executable / "Levels";
  data = executable / "Data";
  logs = executable / "Logs";

  if (!exists(assets))
    throw std::invalid_argument("assets does not exist");
  if (!exists(data))
    throw std::invalid_argument("data does not exist");

  if (!exists(projects)) {
    std::filesystem::create_directory(projects);
  }

  if (!exists(levels)) {
    std::filesystem::create_directory(levels);
  }

  if (!exists(levels)) {
    std::filesystem::create_directory(logs);
  }

  shaders = assets / "Shaders";
  materials = data / "Materials";
  tiles = data / "Graphics";
  props = data / "Props";
  cast = data / "Cast";

  if (!exists(shaders))
    throw std::invalid_argument("Assets/Shaders does not exist");
  if (!exists(materials))
    throw std::invalid_argument("Data/Materials does not exist");
  if (!exists(tiles))
    throw std::invalid_argument("Data/Graphics does not exist");
  if (!exists(props))
    throw std::invalid_argument("Data/Props does not exist");
  if (!exists(cast))
    throw std::invalid_argument("Data/Cast does not exist");
}

dirs::dirs(const std::filesystem::path &executable_directory) {
  if (!std::filesystem::exists(executable_directory))
    throw std::invalid_argument("invalid executable directory");

  assets = executable_directory / "Assets";
  projects = executable_directory / "Projects";
  levels = executable_directory / "Levels";
  data = executable_directory / "Data";
  logs = executable_directory / "Logs";

  if (!exists(assets))
    throw std::invalid_argument("assets does not exist");
  if (!exists(data))
    throw std::invalid_argument("data does not exist");

  if (!exists(projects)) {
    std::filesystem::create_directory(projects);
  }

  if (!exists(levels)) {
    std::filesystem::create_directory(levels);
  }

  if (!exists(levels)) {
    std::filesystem::create_directory(logs);
  }

  shaders = assets / "Shaders";
  materials = data / "Materials";
  tiles = data / "Graphics";
  props = data / "Props";
  cast = data / "Cast";

  if (!exists(shaders))
    throw std::invalid_argument("Assets/Shaders does not exist");
  if (!exists(materials))
    throw std::invalid_argument("Data/Materials does not exist");
  if (!exists(tiles))
    throw std::invalid_argument("Data/Graphics does not exist");
  if (!exists(props))
    throw std::invalid_argument("Data/Props does not exist");
  if (!exists(cast))
    throw std::invalid_argument("Data/Cast does not exist");

  executable = executable_directory;
}

Camera2D &context::get_camera() { return camera; }
void context::set_camera(Camera2D _camera) { camera = _camera; }

const std::vector<Level> &context::get_levels() const noexcept {
  return levels;
}

context::context()
    : logger(nullptr), directories(nullptr),
      textures_(std::make_shared<textures>(directories)),
      f3_(std::make_shared<debug::f3>(
          GetFontDefault(), 20, WHITE,
          Color{.r = GRAY.r, .g = GRAY.g, .b = GRAY.b, .a = 120})),
      camera(Camera2D{.target = Vector2{.x = 1, .y = 1}, .zoom = 1.5f}) {}
context::context(std::shared_ptr<spdlog::logger> logger,
                 std::shared_ptr<dirs> dirs)
    : logger(logger), directories(dirs),
      textures_(std::make_shared<textures>(directories)),
      f3_(std::make_shared<debug::f3>(
          GetFontDefault(), 20, WHITE,
          Color{.r = GRAY.r, .g = GRAY.g, .b = GRAY.b, .a = 120})),
      camera(Camera2D{.target = Vector2{.x = 1, .y = 1}, .zoom = 1.5f}) {}
context::context(std::shared_ptr<spdlog::logger> logger,
                 std::shared_ptr<dirs> dirs,
                 std::shared_ptr<textures> _textures)
    : logger(logger), directories(dirs), textures_(_textures),
      f3_(std::make_shared<debug::f3>(
          GetFontDefault(), 20, WHITE,
          Color{.r = GRAY.r, .g = GRAY.g, .b = GRAY.b, .a = 120})),
      camera(Camera2D{.target = Vector2{.x = 1, .y = 1}, .zoom = 1.5f}) {}
context::~context() {}

void textures::reload_all_textures() {
  file_icon = texture(directories->get_assets() / "Icons" / "file icon.png");
  folder_icon =
      texture(directories->get_assets() / "Icons" / "folder icon.png");
  up_icon = texture(directories->get_assets() / "Icons" / "up icon.png");
  home_icon = texture(directories->get_assets() / "Icons" / "home icon.png");
}

const RenderTexture2D &textures::get_main_level_viewport() const noexcept {
  return main_level_viewport.get();
}

void textures::resize_main_level_viewport(int width, int height) {
  if (width < 0 || height < 0)
    return;

  auto new_viewport = rendertexture(width, height);

  BeginTextureMode(new_viewport.get());
  ClearBackground(BLACK);

  DrawTexture(main_level_viewport.get().texture, 0, 0, WHITE);
  EndTextureMode();

  main_level_viewport = std::move(new_viewport);
}

textures::textures(std::shared_ptr<dirs> directories, bool preload_textures)
    : directories(directories) {
  if (preload_textures) {
    reload_all_textures();
  }
}

textures::~textures() {}
}; // namespace mr
