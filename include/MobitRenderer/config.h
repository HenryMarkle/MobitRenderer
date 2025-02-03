#include <filesystem>
#include <cstdint>
#include <memory>

#include <raylib.h>

namespace mr {

struct SpriteVisiblity {
  int opacity;
  bool inherit, visible;

  SpriteVisiblity(bool inherit = true, bool visible = true,
                  uint8_t opacity = 255);
};

struct SpritePrerender {
  bool tinted, preview, palette;

  SpritePrerender(bool tinted = false, bool preview = true, bool palette = false);
};

struct GeoPageConfig {
  SpriteVisiblity props, tiles, materials, water, grid;
  Color layer1, layer2, layer3, water_color;
  bool basic_view;

  GeoPageConfig();
};

struct GenericPageConfig {
  SpriteVisiblity props, tiles, materials, water, grid;
  GenericPageConfig();
};

struct Config {
  bool 
    splashscreen, 
    f3, 
    crash_on_esc, 
    blue_screen_of_death;

  int event_handle_per_frame, load_per_frame;
  bool list_wrap, strict_deserialization;

  GenericPageConfig default_sprites;

  GeoPageConfig geometry;
  GenericPageConfig 
    tiles,
    cameras,
    light,
    effects,
    props;

  SpritePrerender 
    tiles_prerender, 
    props_prerender, 
    materials_prerender;
  
  bool shadows;

  std::filesystem::path data_path;
  std::filesystem::path cast_path;
  std::filesystem::path assets_path;

  bool default_data_path;
  bool inherit_cast_path;
  bool default_assets_path;

  std::vector<std::filesystem::path> tile_inits, prop_inits, material_inits;

  static std::shared_ptr<Config> from_file(const std::filesystem::path&);

  Config();
};

};