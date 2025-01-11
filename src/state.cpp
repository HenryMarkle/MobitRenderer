#include <cstdint>
#include <filesystem>
#include <memory>
#include <stdexcept>
#include <vector>

#include <spdlog/spdlog.h>

#include <raylib.h>

#include <MobitRenderer/draw.h>
#include <MobitRenderer/level.h>
#include <MobitRenderer/managed.h>
#include <MobitRenderer/state.h>
#include <MobitRenderer/utils.h>

namespace mr {

const std::filesystem::path &dirs::get_executable() const { return executable; }

const std::filesystem::path &dirs::get_assets() const { return assets; }
const std::filesystem::path &dirs::get_projects() const { return projects; }
const std::filesystem::path &dirs::get_levels() const { return levels; }
const std::filesystem::path &dirs::get_data() const { return data; }
const std::filesystem::path &dirs::get_logs() const { return logs; }

const std::filesystem::path &dirs::get_shaders() const { return shaders; }
const std::filesystem::path &dirs::get_fonts() const { return fonts; }
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
  fonts = assets / "Fonts";
  materials = data / "Materials";
  tiles = data / "Graphics";
  props = data / "Props";
  cast = data / "Cast";

  if (!exists(shaders))
    throw std::invalid_argument("Assets/Shaders does not exist");
  if (!exists(fonts))
    std::filesystem::create_directory(fonts);
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
  fonts = assets / "Fonts";
  materials = data / "Materials";
  tiles = data / "Graphics";
  props = data / "Props";
  cast = data / "Cast";

  if (!exists(shaders))
    throw std::invalid_argument("Assets/Shaders does not exist");
  if (!exists(fonts))
    std::filesystem::create_directory(fonts);
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

void shaders::unload_all() {
  mr::utils::unload_shader(_vflip);
  mr::utils::unload_shader(_white_remover);
  mr::utils::unload_shader(_white_remover_apply_color);
}

void shaders::reload_all() {
  unload_all();

  auto vflip_path = _shaders_dir / "vflip.frag";
  _vflip = LoadShader(nullptr, vflip_path.c_str());

  auto white_remover_path = _shaders_dir / "white_remover.frag";
  _white_remover = LoadShader(nullptr, white_remover_path.c_str());

  auto white_remover_apply_color_path = _shaders_dir / "white_remover_apply_color.frag";
  _white_remover_apply_color = LoadShader(nullptr, white_remover_apply_color_path.c_str());
}

const Shader &shaders::vflip() const noexcept { return _vflip; }
const Shader &shaders::white_remover() const noexcept { return _white_remover; }
const Shader &shaders::white_remover_apply_color() const noexcept { return _white_remover_apply_color; }

shaders &shaders::operator=(shaders const&other) {
  if (&other == this) return *this;

  _shaders_dir = other._shaders_dir;

  _vflip = Shader{.id=0};
  _white_remover = Shader{.id=0};
  _white_remover_apply_color = Shader{.id=0};

  reload_all();

  return *this;
}

shaders &shaders::operator=(shaders &&other) noexcept {
  if (&other == this) return *this;

  _shaders_dir = other._shaders_dir;

  _vflip = other._vflip;
  _white_remover = other._white_remover;
  _white_remover_apply_color = other._white_remover_apply_color;

  other._vflip = Shader{.id=0};
  other._white_remover = Shader{.id=0};
  other._white_remover_apply_color = Shader{.id=0};

  return *this;
}

shaders::shaders(shaders const&other) 
  : _shaders_dir(other._shaders_dir), _vflip(Shader{.id=0}), _white_remover(Shader{.id=0}), _white_remover_apply_color(Shader{.id=0}) {
    reload_all();
}

shaders::shaders(shaders &&other) noexcept : _shaders_dir(other._shaders_dir), _vflip(other._vflip), _white_remover(other._white_remover), _white_remover_apply_color(other._white_remover_apply_color) {
  other._vflip = Shader{.id = 0};
  other._white_remover = Shader{.id = 0};
  other._white_remover_apply_color = Shader{.id=0};
}

shaders::shaders(std::filesystem::path shaders_dir) : 
  _shaders_dir(shaders_dir), 
  _vflip(Shader{.id = 0}), 
  _white_remover(Shader{.id = 0}),
  _white_remover_apply_color(Shader{.id=0}) { }

shaders::~shaders() { 
  unload_all();
}

Camera2D &context::get_camera() { return camera; }
void context::set_camera(Camera2D _camera) { camera = _camera; }

uint8_t context::get_level_index() const noexcept { return selected_level; }
const std::vector<Level *> &context::get_levels() const noexcept {
  return levels;
}
void context::add_level(std::unique_ptr<Level> level) {
  auto *dropped = level.release();

  levels.push_back(dropped);
}
void context::add_level(mr::Level *level) { levels.push_back(level); }
void context::select_level(uint8_t index) {
  if (index >= levels.size())
    return;
  selected_level = index;
}

mr::Level *context::get_selected_level() { return levels[selected_level]; }

void context::lock_global_shortcuts() noexcept {
  enable_global_shortcuts = false;
}
void context::unlock_global_shortcuts() noexcept {
  enable_global_shortcuts = true;
}

const Font *context::get_selected_font_const_ptr() const noexcept {
  if (selected_font >= fonts.size())
    return nullptr;

  return &fonts[selected_font];
}
Font context::get_selected_font() const noexcept {
  if (selected_font >= fonts.size())
    return {.texture = {.id = 0}};

  return fonts[selected_font];
}
void context::select_font(uint8_t index) noexcept {
  if (index >= fonts.size())
    return;

  selected_font = index;
  f3_->set_font(fonts[index]);
}
void context::add_font(Font f) noexcept { fonts.push_back(f); }

const shaders &context::get_shaders_const() const noexcept { return _shaders; }
shaders &context::get_shaders() noexcept { return _shaders; }

context::context(std::shared_ptr<spdlog::logger> logger,
                 std::shared_ptr<dirs> dirs)
    : logger(logger), directories(dirs),
      textures_(std::make_unique<textures>(directories)),
      _shaders(dirs->get_shaders()),
      f3_(std::make_shared<debug::f3>(
          GetFontDefault(), 28, WHITE,
          Color{.r = GRAY.r, .g = GRAY.g, .b = GRAY.b, .a = 120})),
      camera(Camera2D{.target = Vector2{.x = 1, .y = -40}, .zoom = 0.5f}),
      enable_global_shortcuts(true) {}
context::context(std::shared_ptr<spdlog::logger> logger,
                 std::shared_ptr<dirs> dirs,
                 std::unique_ptr<textures> _textures)
    : logger(logger), directories(dirs), textures_(std::move(_textures)),
      _shaders(dirs->get_shaders()),
      f3_(std::make_shared<debug::f3>(
          GetFontDefault(), 28, WHITE,
          Color{.r = GRAY.r, .g = GRAY.g, .b = GRAY.b, .a = 120})),
      camera(Camera2D{.target = Vector2{.x = 1, .y = -40}, .zoom = 0.5f}),
      enable_global_shortcuts(true) {}
context::~context() {
  if (!levels.empty()) {
    for (auto *level : levels)
      delete level;
  }
}

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

void textures::resize_all_level_buffers(int width, int height) {
  if (width < 0 || height < 0) return;

  auto new_viewport = rendertexture(width, height);

  if (main_level_viewport.is_loaded()) {
    BeginTextureMode(new_viewport.get());
    ClearBackground(BLACK);

    DrawTexture(main_level_viewport.get().texture, 0, 0, WHITE);
    EndTextureMode();
  }

  main_level_viewport = std::move(new_viewport);

  // geo buffers

  auto new_geo_layer1 = rendertexture(width, height);
  auto new_geo_layer2 = rendertexture(width, height);
  auto new_geo_layer3 = rendertexture(width, height);

  if (geo_layer1.is_loaded()) {
    BeginTextureMode(new_geo_layer1.get());
    ClearBackground(WHITE);
    DrawTexture(geo_layer1.get().texture, 0, 0, WHITE);
    EndTextureMode();
  }

  if (geo_layer2.is_loaded()) {
    BeginTextureMode(new_geo_layer2.get());
    ClearBackground(WHITE);
    DrawTexture(geo_layer2.get().texture, 0, 0, WHITE);
    EndTextureMode();
  }

  if (geo_layer3.is_loaded()) {
    BeginTextureMode(new_geo_layer3.get());
    ClearBackground(WHITE);
    DrawTexture(geo_layer3.get().texture, 0, 0, WHITE);
    EndTextureMode();
  }

  geo_layer1 = std::move(new_geo_layer1);
  geo_layer2 = std::move(new_geo_layer2);
  geo_layer3 = std::move(new_geo_layer3);
}

textures::textures(std::shared_ptr<dirs> directories, bool preload_textures)
    : directories(directories) {
  if (preload_textures) {
    reload_all_textures();
  }
}

textures::~textures() {}

SpriteVisiblity::SpriteVisiblity(bool inherit, bool visible, uint8_t opacity) :
  inherit(inherit), visible(visible), opacity(opacity) {}

SpritePrerender::SpritePrerender(bool tinted, bool preview, bool palette) : tinted(tinted), preview(preview), palette(palette) {}

config::config() : 
  splashscreen(true), 
  f3(false), 
  crash_on_esc(false),
  blue_screen_of_death(true),
  event_handle_per_frame(30),
  load_per_frame(100),
  list_wrap(true),
  strict_deserialization(true),

  props_visibility(SpriteVisiblity()),
  tiles_visibility(SpriteVisiblity()),
  water_visibility(SpriteVisiblity()),
  materials_visibility(SpriteVisiblity()),
  
  tiles_prerender(SpritePrerender()),
  props_prerender(SpritePrerender()),
  materials_prerender(SpritePrerender()),
  shadows(false) {}
}; // namespace mr
