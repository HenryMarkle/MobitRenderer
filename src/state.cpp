#if defined(_WIN32) || defined(_WIN64)
  #define WIN32_LEAN_AND_MEAN
#endif

#include <cstdint>
#include <filesystem>
#include <memory>
#include <stdexcept>
#include <vector>

#include <spdlog/spdlog.h>

#include <raylib.h>

#include <MobitRenderer/atlas.h>
#include <MobitRenderer/draw.h>
#include <MobitRenderer/level.h>
#include <MobitRenderer/managed.h>
#include <MobitRenderer/state.h>
#include <MobitRenderer/utils.h>
#include <MobitRenderer/io.h>

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

void dirs::set_data(std::filesystem::path dir, bool include_cast) {
  if (!std::filesystem::is_directory(dir))
      throw std::invalid_argument(dir.string()+" is not a directory");
  
  data = dir;
  props = data / "Props";
  tiles = data / "Graphics";
  materials = data / "Materials";
  if (include_cast) cast = data / "Cast";

  data_found = std::filesystem::exists(data);
  props_found = std::filesystem::exists(props);
  tiles_found = std::filesystem::exists(tiles);
  materials_found = std::filesystem::exists(materials);
  if (include_cast) cast_found = std::filesystem::exists(cast);
}
void dirs::set_levels(std::filesystem::path dir) {
  if (!std::filesystem::is_directory(dir))
      throw std::invalid_argument(dir.string()+" is not a directory");
  
  levels = dir;
  levels_found = std::filesystem::exists(levels);
}

dirs::dirs() {

  executable = mr::get_executable_dir();

  #ifdef IS_DEBUG_BUILD
  data = executable / ".." / ".." / "Data";
  assets = executable / ".." / ".." / "assets";

  // VS Code's annoying configuration
  if (!exists(data) && !exists(assets)) {
    data = executable / ".." / ".." / ".." / "Data";
    assets = executable / ".." / ".." / ".." / "assets";
  }
  #else
  data = executable / "Data";
  assets = executable / "Assets";
  #endif


  projects = executable / "Projects";
  levels = executable / "Levels";
  logs = executable / "Logs";

  if (!exists(projects)) {
    std::filesystem::create_directory(projects);
  }

  if (!exists(levels)) {
    std::filesystem::create_directory(levels);
  }

  if (!exists(logs)) {
    std::filesystem::create_directory(logs);
  }

  shaders = assets / "Shaders";
  fonts = assets / "Fonts";
  materials = data / "Materials";
  tiles = data / "Graphics";
  props = data / "Props";
  cast = data / "Cast";

  executable_found = exists(executable);

  assets_found = exists(assets);
  shaders_found = exists(shaders);
  fonts_found = exists(fonts);

  data_found = exists(data);
  tiles_found = exists(tiles);
  materials_found = exists(materials);
  props_found = exists(props);
  cast_found = exists(cast);

  ok = executable_found && 
    assets_found &&
    shaders_found &&
    data_found &&
    tiles_found &&
    materials_found &&
    props_found &&
    cast_found;
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
  mr::utils::unload_shader(_white_remover_rgb_recolor);
  mr::utils::unload_shader(_voxel_struct_tinted);
}

void shaders::reload_all() {
  unload_all();

  auto vflip_path = _shaders_dir / "vflip.frag";
  _vflip = LoadShader(nullptr, vflip_path.string().c_str());

  auto white_remover_path = _shaders_dir / "white_remover.frag";
  _white_remover = LoadShader(nullptr, white_remover_path.string().c_str());

  auto white_remover_apply_color_path = _shaders_dir / "white_remover_apply_color.frag";
  _white_remover_apply_color = LoadShader(nullptr, white_remover_apply_color_path.string().c_str());
  
  auto white_remover_apply_alpha_path = _shaders_dir / "white_remover_apply_alpha.frag";
  _white_remover_apply_alpha = LoadShader(nullptr, white_remover_apply_alpha_path.string().c_str());

  auto white_remover_rgb_recolor_path = _shaders_dir / "white_remover_rgb_recolor.frag";
  _white_remover_rgb_recolor = LoadShader(nullptr, white_remover_rgb_recolor_path.string().c_str());

  auto voxel_struct_tinted_path = _shaders_dir / "voxel_struct_tinted.frag";
  _voxel_struct_tinted = LoadShader(nullptr, voxel_struct_tinted_path.string().c_str());
}

shaders &shaders::operator=(shaders &&other) noexcept {
  if (&other == this) return *this;

  _shaders_dir = other._shaders_dir;

  _vflip = other._vflip;
  _white_remover = other._white_remover;
  _white_remover_apply_color = other._white_remover_apply_color;
  _white_remover_apply_alpha = other._white_remover_apply_alpha;
  _white_remover_rgb_recolor = other._white_remover_rgb_recolor;
  _voxel_struct_tinted = other._voxel_struct_tinted;

  other._vflip = Shader{0};
  other._white_remover = Shader{0};
  other._white_remover_apply_color = Shader{0};
  other._white_remover_apply_alpha = Shader{0};
  other._white_remover_rgb_recolor = Shader{0};
  other._voxel_struct_tinted = Shader{0};

  return *this;
}

shaders::shaders(shaders &&other) noexcept : 
  _shaders_dir(other._shaders_dir), 
  _vflip(other._vflip), 
  _white_remover(other._white_remover), 
  _white_remover_apply_color(other._white_remover_apply_color),
  _white_remover_apply_alpha(other._white_remover_apply_alpha),
  _white_remover_rgb_recolor(other._white_remover_rgb_recolor),
  _voxel_struct_tinted(other._voxel_struct_tinted)
{
  other._vflip = Shader{0};
  other._white_remover = Shader{0};
  other._white_remover_apply_color = Shader{0};
  other._white_remover_apply_alpha = Shader{0};
  other._white_remover_rgb_recolor = Shader{0};
  other._voxel_struct_tinted = Shader{0};
}

shaders::shaders(std::filesystem::path shaders_dir) : 
  _shaders_dir(shaders_dir), 
  _vflip(Shader{0}), 
  _white_remover(Shader{0}),
  _white_remover_apply_color(Shader{0}),
  _white_remover_apply_alpha(Shader{0}),
  _white_remover_rgb_recolor(Shader{0}),
  _voxel_struct_tinted(Shader{0}) { }

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

mr::Level *context::get_selected_level() { 
  if (levels.size() == 0) return nullptr;
  
  return levels[selected_level]; 
}

void context::lock_global_shortcuts() noexcept {
  enable_global_shortcuts = false;
}
void context::unlock_global_shortcuts() noexcept {
  enable_global_shortcuts = true;
}

const config &context::get_config_const() const noexcept {
  return _config;
}
config &context::get_config() noexcept {
  return _config;
}
const config *context::get_config_const_ptr() const noexcept {
  return &_config;
}
config *context::get_config_ptr() noexcept {
  return &_config;
}
void context::set_config(config c) noexcept {
  _config = c;
}

context::context(std::shared_ptr<spdlog::logger> logger,
                 std::shared_ptr<dirs> dirs)
    : logger(logger), 
      directories(dirs),
      _textures(nullptr),
      _tiledex(nullptr),
      _shaders(nullptr),
      _fonts(nullptr),
      f3_(std::make_shared<debug::f3>(GetFontDefault(), 22, WHITE, Color{GRAY.r, GRAY.g, GRAY.b, 120})),
      camera(Camera2D{Vector2{1, 40}, Vector2{0, 0}, 0, 0.5f}),
      enable_global_shortcuts(true),
      level_layer_(0) {}


context::~context() {
  if (!levels.empty()) {
    for (auto *level : levels)
      delete level;
  }
}

void textures::reload_all_textures() {
  file_icon = texture((directories->get_assets() / "Icons" / "file icon.png").string().c_str());
  folder_icon = texture((directories->get_assets() / "Icons" / "folder icon.png").string().c_str());
  up_icon = texture((directories->get_assets() / "Icons" / "up icon.png").string().c_str());
  home_icon = texture((directories->get_assets() / "Icons" / "home icon.png").string().c_str());

  geometry_editor.reload();
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
  {
    auto new_geo_layer1 = rendertexture(width, height);
    auto new_geo_layer2 = rendertexture(width, height);
    auto new_geo_layer3 = rendertexture(width, height);

    BeginTextureMode(new_geo_layer1.get());
    ClearBackground(WHITE);
    if (geo_layer1.is_loaded()) {
      DrawTexture(geo_layer1.get().texture, 0, 0, WHITE);
    }
    EndTextureMode();

    BeginTextureMode(new_geo_layer2.get());
    ClearBackground(WHITE);
    if (geo_layer2.is_loaded()) {
      DrawTexture(geo_layer2.get().texture, 0, 0, WHITE);
    }
    EndTextureMode();

    BeginTextureMode(new_geo_layer3.get());
    ClearBackground(WHITE);
    if (geo_layer3.is_loaded()) {
      DrawTexture(geo_layer3.get().texture, 0, 0, WHITE);
    }
    EndTextureMode();

    geo_layer1 = std::move(new_geo_layer1);
    geo_layer2 = std::move(new_geo_layer2);
    geo_layer3 = std::move(new_geo_layer3);
  }
  // feature buffers
  {

    auto new_feature_layer1 = rendertexture(width, height);
    auto new_feature_layer2 = rendertexture(width, height);
    auto new_feature_layer3 = rendertexture(width, height);

    BeginTextureMode(feature_layer1.get());
    ClearBackground(WHITE);
    if (feature_layer1.is_loaded()) {
      DrawTexture(feature_layer1.get().texture, 0, 0, WHITE);
    }
    EndTextureMode();

    BeginTextureMode(feature_layer2.get());
    ClearBackground(WHITE);
    if (feature_layer2.is_loaded()) {
      DrawTexture(feature_layer2.get().texture, 0, 0, WHITE);
    }
    EndTextureMode();

    BeginTextureMode(new_feature_layer3.get());
    ClearBackground(WHITE);
    if (feature_layer3.is_loaded()) {
      DrawTexture(feature_layer3.get().texture, 0, 0, WHITE);
    }
    EndTextureMode();

    feature_layer1 = std::move(new_feature_layer1);
    feature_layer2 = std::move(new_feature_layer2);
    feature_layer3 = std::move(new_feature_layer3);
  }
  // Tile buffers
  {
    auto new_tile_layer1 = rendertexture(width, height);
    auto new_tile_layer2 = rendertexture(width, height);
    auto new_tile_layer3 = rendertexture(width, height);

    BeginTextureMode(new_tile_layer1.get());
    ClearBackground(WHITE);
    if (tile_layer1.is_loaded()) {
      DrawTexture(tile_layer1.get().texture, 0, 0, WHITE);
    }
    EndTextureMode();

    BeginTextureMode(new_tile_layer2.get());
    ClearBackground(WHITE);
    if (tile_layer2.is_loaded()) {
      DrawTexture(tile_layer2.get().texture, 0, 0, WHITE);
    }
    EndTextureMode();

    BeginTextureMode(new_tile_layer3.get());
    ClearBackground(WHITE);
    if (tile_layer3.is_loaded()) {
      DrawTexture(tile_layer3.get().texture, 0, 0, WHITE);
    }
    EndTextureMode();

    tile_layer1 = std::move(new_tile_layer1);
    tile_layer2 = std::move(new_tile_layer2);
    tile_layer3 = std::move(new_tile_layer3);
  }
}

textures::textures(std::shared_ptr<dirs> directories, bool preload_textures)
    : directories(directories), 
    geometry_editor(directories.get()->get_assets()) {
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
  grid(SpriteVisiblity()),
  
  tiles_prerender(SpritePrerender()),
  props_prerender(SpritePrerender()),
  materials_prerender(SpritePrerender()),
  shadows(false) {}

void fonts::load_all() {
  if (loaded) return;
  
  small_default_font = LoadFontEx((fonts_dir / "Oswald-Regular.ttf").string().c_str(), 22, nullptr, 0);
  large_default_font = LoadFontEx((fonts_dir / "Oswald-Regular.ttf").string().c_str(), 30, nullptr, 0);

  SetTextureFilter(small_default_font.texture, TEXTURE_FILTER_POINT);
  SetTextureFilter(large_default_font.texture, TEXTURE_FILTER_POINT);

  loaded = true;
}

fonts::fonts(const std::filesystem::path &fonts_dir)
  : fonts_dir(fonts_dir), loaded(false) {}

fonts::~fonts() {
  unload_all();
}

}; // namespace mr
