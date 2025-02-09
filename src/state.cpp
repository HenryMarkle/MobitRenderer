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

#include <toml++/toml.hpp>
#include <toml++/impl/forward_declarations.hpp>
#include <toml++/impl/parse_error.hpp>
#include <toml++/impl/parser.hpp>

#include <MobitRenderer/atlas.h>
#include <MobitRenderer/draw.h>
#include <MobitRenderer/level.h>
#include <MobitRenderer/managed.h>
#include <MobitRenderer/state.h>
#include <MobitRenderer/utils.h>
#include <MobitRenderer/io.h>

namespace mr {

void shaders::unload_all() {
  if (!loaded) return;
  mr::utils::unload_shader(_vflip);
  mr::utils::unload_shader(_apply_alpha);
  mr::utils::unload_shader(_apply_alpha_vflip);
  mr::utils::unload_shader(_white_remover);
  mr::utils::unload_shader(_white_remover_apply_color_vflip);
  mr::utils::unload_shader(_white_remover_apply_color);
  mr::utils::unload_shader(_white_remover_rgb_recolor);
  mr::utils::unload_shader(_voxel_struct);
  mr::utils::unload_shader(_default_prop);
  mr::utils::unload_shader(_soft);
  mr::utils::unload_shader(_varied_soft);
  mr::utils::unload_shader(_voxel_struct_tinted);
  mr::utils::unload_shader(_varied_voxel_struct);
  loaded = false;
}

void shaders::load_all() {
  if (loaded) return;

  auto vflip_path = _shaders_dir / "vflip.frag";
  _vflip = LoadShader(nullptr, vflip_path.string().c_str());

  auto apply_alpha_path = _shaders_dir / "apply_alpha.frag";
  _apply_alpha = LoadShader(nullptr, apply_alpha_path.string().c_str());

  auto vflip_apply_alpha_path = _shaders_dir / "apply_alpha_vflip.frag";
  _apply_alpha_vflip = LoadShader(nullptr, vflip_apply_alpha_path.string().c_str());

  auto white_remover_path = _shaders_dir / "white_remover.frag";
  _white_remover = LoadShader(nullptr, white_remover_path.string().c_str());

  auto white_remover_apply_color_vflip_path = _shaders_dir / "white_remover_apply_color_vflip.frag";
  _white_remover_apply_color_vflip = LoadShader(nullptr, white_remover_apply_color_vflip_path.string().c_str());

  auto white_remover_apply_color_path = _shaders_dir / "white_remover_apply_color.frag";
  _white_remover_apply_color = LoadShader(nullptr, white_remover_apply_color_path.string().c_str());
  
  auto white_remover_apply_alpha_path = _shaders_dir / "white_remover_apply_alpha.frag";
  _white_remover_apply_alpha = LoadShader(nullptr, white_remover_apply_alpha_path.string().c_str());

  auto white_remover_rgb_recolor_path = _shaders_dir / "white_remover_rgb_recolor.frag";
  _white_remover_rgb_recolor = LoadShader(nullptr, white_remover_rgb_recolor_path.string().c_str());

  auto voxel_struct_path = _shaders_dir / "voxel_struct.frag";
  _voxel_struct = LoadShader(nullptr, voxel_struct_path.string().c_str());

  auto default_prop_path = _shaders_dir / "default_prop.frag";
  _default_prop = LoadShader(nullptr, default_prop_path.string().c_str());

  auto soft_path = _shaders_dir / "soft.frag";
  _soft = LoadShader(nullptr, soft_path.string().c_str());

  auto varied_soft_path = _shaders_dir / "varied_soft.frag";
  _varied_soft = LoadShader(nullptr, varied_soft_path.string().c_str());

  auto voxel_struct_tinted_path = _shaders_dir / "voxel_struct_tinted.frag";
  _voxel_struct_tinted = LoadShader(nullptr, voxel_struct_tinted_path.string().c_str());

  auto variedvoxel_struct_path = _shaders_dir / "varied_voxel_struct.frag";
  _varied_voxel_struct = LoadShader(nullptr, variedvoxel_struct_path.string().c_str());

  loaded = true;
}

void shaders::reload_all() {
  unload_all();
  load_all();
}

shaders &shaders::operator=(shaders &&other) noexcept {
  if (&other == this) return *this;

  loaded = other.loaded;

  _shaders_dir = other._shaders_dir;

  _vflip = other._vflip;
  _apply_alpha = other._apply_alpha;
  _apply_alpha_vflip = other._apply_alpha_vflip;
  _white_remover = other._white_remover;
  _white_remover_apply_color_vflip = other._white_remover_apply_color_vflip;
  _white_remover_apply_color = other._white_remover_apply_color;
  _white_remover_apply_alpha = other._white_remover_apply_alpha;
  _white_remover_rgb_recolor = other._white_remover_rgb_recolor;
  _voxel_struct = other._voxel_struct;
  _default_prop = other._default_prop;
  _soft = other._soft;
  _varied_soft = other._varied_soft;
  _voxel_struct_tinted = other._voxel_struct_tinted;
  _varied_voxel_struct = other._varied_voxel_struct;

  other._vflip = Shader{0};
  other._apply_alpha = Shader{0};
  other._apply_alpha_vflip = Shader{0};
  other._white_remover = Shader{0};
  other._white_remover_apply_color_vflip = Shader{0};
  other._white_remover_apply_color = Shader{0};
  other._white_remover_apply_alpha = Shader{0};
  other._white_remover_rgb_recolor = Shader{0};
  other._voxel_struct = Shader{0};
  other._default_prop = Shader{0};
  other._soft = Shader{0};
  other._varied_soft = Shader{0};
  other._voxel_struct_tinted = Shader{0};
  other._varied_voxel_struct = Shader{0};
  other.loaded = false;

  return *this;
}

shaders::shaders(shaders &&other) noexcept : 
  _shaders_dir(other._shaders_dir), 
  _vflip(other._vflip), 
  _apply_alpha_vflip(other._apply_alpha_vflip), 
  _apply_alpha(other._apply_alpha), 
  _white_remover(other._white_remover), 
  _white_remover_apply_color_vflip(other._white_remover_apply_color_vflip),
  _white_remover_apply_color(other._white_remover_apply_color),
  _white_remover_apply_alpha(other._white_remover_apply_alpha),
  _white_remover_rgb_recolor(other._white_remover_rgb_recolor),
  _voxel_struct(other._voxel_struct),
  _default_prop(other._default_prop),
  _soft(other._soft),
  _varied_soft(other._varied_soft),
  _voxel_struct_tinted(other._voxel_struct_tinted),
  _varied_voxel_struct(other._varied_voxel_struct),
  loaded(other.loaded)
{
  other.loaded = false;
  other._vflip = Shader{0};
  other._apply_alpha = Shader{0};
  other._apply_alpha_vflip = Shader{0};
  other._white_remover = Shader{0};
  other._white_remover_apply_color_vflip = Shader{0};
  other._white_remover_apply_color = Shader{0};
  other._white_remover_apply_alpha = Shader{0};
  other._white_remover_rgb_recolor = Shader{0};
  other._voxel_struct = Shader{0};
  other._default_prop = Shader{0};
  other._soft = Shader{0};
  other._varied_soft = Shader{0};
  other._voxel_struct_tinted = Shader{0};
  other._varied_voxel_struct = Shader{0};
}

shaders::shaders(std::filesystem::path shaders_dir) : 
  loaded(false),
  _shaders_dir(shaders_dir), 
  _vflip(Shader{0}), 
  _apply_alpha(Shader{0}), 
  _apply_alpha_vflip(Shader{0}), 
  _white_remover(Shader{0}),
  _white_remover_apply_color_vflip(Shader{0}),
  _white_remover_apply_color(Shader{0}),
  _white_remover_apply_alpha(Shader{0}),
  _white_remover_rgb_recolor(Shader{0}),
  _voxel_struct(Shader{0}),
  _default_prop(Shader{0}),
  _soft(Shader{0}),
  _varied_soft(Shader{0}),
  _voxel_struct_tinted(Shader{0}),
  _varied_voxel_struct(Shader{0}) { }

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

void context::set_config(Config const &c) noexcept { *_config = c; }

context::context(std::shared_ptr<spdlog::logger> logger,
                 std::shared_ptr<Dirs> dirs)
    : logger(logger),
      _config(Config::from_file(dirs->get_executable() / "config.toml")),
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

void Palette::load() {
  if (loaded) return;

  if (!std::filesystem::exists(texture_path)) {
    #ifdef IS_DEBUG_BUILD
    std::cout << "Warning: palette texture not found: " << texture_path << std::endl;
    #endif
    return;
  }

  texture = LoadTexture(texture_path.string().c_str());
  loaded = true;
}

void Palette::unload() {
  if (!loaded) return;
  mr::utils::unload_texture(texture);
  loaded = false;
}

Palette &Palette::operator=(Palette &&other) noexcept {
  if (this == &other) return *this;

  texture = other.texture;
  loaded = other.loaded;

  name = std::move(other.name);
  texture_path = std::move(other.texture_path);

  other.texture = Texture2D{0};
  other.loaded = false;

  return *this;
}

Palette::Palette(const std::string &name, const std::filesystem::path &p) :
  texture(Texture2D{0}),
  name(name),
  texture_path(p),
  loaded(false)
{}

Palette::Palette(Palette &&other) noexcept :
  loaded(other.loaded),
  texture_path(std::move(other.texture_path)),
  name(std::move(other.name)),
  texture(other.texture)
{
  memset(&other.texture, 0, sizeof(Texture2D));
  other.loaded = false;
}

Palette::~Palette() { unload(); }

void textures::reload_all_textures() {
  file_icon = texture((directories->get_assets() / "Icons" / "file icon.png").string().c_str());
  folder_icon = texture((directories->get_assets() / "Icons" / "folder icon.png").string().c_str());
  up_icon = texture((directories->get_assets() / "Icons" / "up icon.png").string().c_str());
  home_icon = texture((directories->get_assets() / "Icons" / "home icon.png").string().c_str());

  geometry_editor.reload();
  light_editor.reload();

  #ifdef FEATURE_PALETTES
  for (auto &palette : palettes) palette.reload();
  #endif
}

void textures::reload_palettes() {
  for (auto &palette : palettes) palette.reload();
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
  // Prop buffer
  {
    auto new_buffer = rendertexture(width, height);

    BeginTextureMode(new_buffer.get());
    ClearBackground(WHITE);

    if (props.is_loaded()) {
      DrawTexture(props.get().texture, 0, 0, WHITE);
    }
    EndTextureMode();

    props = std::move(new_buffer);
  }
}

textures::textures(std::shared_ptr<Dirs> directories, bool preload_textures)
    : directories(directories), 
    geometry_editor(directories.get()->get_assets()),
    light_editor(directories.get()->get_assets())
{

  if (preload_textures) {
    reload_all_textures();
  }

  #ifdef FEATURE_PALETTES
  if (directories->is_palettes_found()) {
    for (const auto &entry : std::filesystem::directory_iterator(directories->get_palettes())) {
      if (entry.path().extension() != ".png") continue;

      palettes.push_back(Palette(entry.path().stem().string(), entry.path()));
    }
  }
  #endif
}

textures::~textures() {}



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
