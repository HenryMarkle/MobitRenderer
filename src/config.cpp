#include <cstdint>
#include <filesystem>
#include <memory>

#include <raylib.h>

#include <toml++/toml.hpp>
#include <toml++/impl/forward_declarations.hpp>
#include <toml++/impl/parse_error.hpp>
#include <toml++/impl/parser.hpp>

#include <MobitRenderer/serialization.h>
#include <MobitRenderer/config.h>

bool is_inherit(const toml::v3::node_view<toml::v3::node>&) noexcept;
void deser_sprite_visibility(const toml::v3::node_view<toml::v3::node>&, mr::SpriteVisiblity&) noexcept;
void deser_generic_page_config(const toml::v3::node_view<toml::v3::node>&, mr::GenericPageConfig&, const mr::GenericPageConfig&) noexcept;

namespace mr {

SpriteVisiblity::SpriteVisiblity(bool inherit, bool visible, uint8_t opacity) :
  inherit(inherit), visible(visible), opacity(opacity) {}

SpritePrerender::SpritePrerender(bool tinted, bool preview, bool palette) : tinted(tinted), preview(preview), palette(palette) {}

GeoPageConfig::GeoPageConfig() : 
    props(SpriteVisiblity()), 
    tiles(SpriteVisiblity()), 
    water(SpriteVisiblity()), 
    grid(SpriteVisiblity()), 
    materials(SpriteVisiblity()),
    layer1(BLACK),
    layer2(Color {0, 255, 0, 70}),
    layer3(Color {255, 0, 0, 70}),
    water_color(Color {0, 0, 255, 90}),
    basic_view(true)
{}

GenericPageConfig::GenericPageConfig() : 
    props(SpriteVisiblity()), 
    tiles(SpriteVisiblity()), 
    water(SpriteVisiblity()), 
    grid(SpriteVisiblity()), 
    materials(SpriteVisiblity())
{}

std::shared_ptr<Config> Config::from_file(const std::filesystem::path &p) {
    if (!std::filesystem::exists(p) || !std::filesystem::is_regular_file(p)) return std::make_shared<Config>();

    Config config;
    toml::table result;

    try {
        result = toml::parse_file(p.string());
    } catch (toml::parse_error &pe) {
        throw std::runtime_error(std::string("failed to parse config file: ")+pe.what());
    }

    const auto &misc = result["misc"];

    config.splashscreen         = misc["splashscreen"].value_or(false);
    config.f3                   = misc["f3"].value_or(false);
    config.crash_on_esc         = misc["crash_on_esc"].value_or(false);
    config.blue_screen_of_death = misc["blue_screen_of_death"].value_or(false);

    const auto &general = result["general"];

    config.event_handle_per_frame = general["event_handle_per_frame"].value_or(30);
    config.load_per_frame         = general["load_per_frame"].value_or(20);
    config.list_wrap              = general["list_wrap"].value_or(true);
    config.strict_deserialization = general["strict_deserialization"].value_or(false);

    auto general_config = GenericPageConfig();

    general_config.props.visible = general["props"]["visible"].value_or(true);
    general_config.props.opacity = general["props"]["opacity"].value_or(255);

    general_config.tiles.visible = general["tiles"]["visible"].value_or(true);
    general_config.tiles.opacity = general["tiles"]["opacity"].value_or(255);

    general_config.materials.visible = general["materials"]["visible"].value_or(true);
    general_config.materials.opacity = general["materials"]["opacity"].value_or(255);

    general_config.water.visible = general["water"]["visible"].value_or(true);
    general_config.water.opacity = general["water"]["opacity"].value_or(70);
    
    general_config.grid.visible = general["grid"]["visible"].value_or(true);
    general_config.grid.opacity = general["grid"]["opacity"].value_or(90);

    config.default_sprites = general_config;
    
    const auto &general_prerendering = general["prerendering"];

    auto general_pr_tiles = SpritePrerender();

    general_pr_tiles.tinted = general_prerendering["tiles"]["tinted"].value_or(false);
    general_pr_tiles.preview = general_prerendering["tiles"]["preview"].value_or(true);
    general_pr_tiles.palette = general_prerendering["tiles"]["palette"].value_or(false);

    auto general_pr_props = SpritePrerender();

    general_pr_props.tinted = general_prerendering["props"]["tinted"].value_or(false);
    general_pr_props.preview = general_prerendering["props"]["preview"].value_or(true);
    general_pr_props.palette = general_prerendering["props"]["palette"].value_or(false);

    auto general_pr_materials = SpritePrerender();

    general_pr_materials.palette = general_prerendering["materials"]["palette"].value_or(false);

    config.shadows = general_prerendering["shadows"].value_or(false);
    
    const auto &page = result["page"];

    { // Geometry sprite settings

        const auto &geometry = page["geometry"];

        config.geometry.basic_view= geometry["basic_view"].value_or(true);

        if (geometry["props"]["inherit"].value_or(true)) {
            config.geometry.props = general_config.props;
        } else {
            config.geometry.props.visible = geometry["props"]["visible"].value_or(false);
            config.geometry.props.opacity = geometry["props"]["opacity"].value_or(255);
        }

        if (geometry["tiles"]["inherit"].value_or(true)) {
            config.geometry.tiles = general_config.tiles;
        } else {
            config.geometry.tiles.visible = geometry["tiles"]["visible"].value_or(false);
            config.geometry.tiles.opacity = geometry["tiles"]["opacity"].value_or(255);
        }

        if (geometry["materials"]["inherit"].value_or(true)) {
            config.geometry.materials = general_config.materials;
        } else {
            config.geometry.materials.visible = geometry["materials"]["visible"].value_or(false);
            config.geometry.materials.opacity = geometry["materials"]["opacity"].value_or(255);
        }

        if (geometry["water"]["inherit"].value_or(true)) {
            config.geometry.water = general_config.water;
        } else {
            config.geometry.water.visible = geometry["water"]["visible"].value_or(false);
            config.geometry.water.opacity = geometry["water"]["opacity"].value_or(70);
        }

        if (geometry["grid"]["inherit"].value_or(true)) {
            config.geometry.grid = general_config.grid;
        } else {
            config.geometry.grid.visible = geometry["grid"]["visible"].value_or(false);
            config.geometry.grid.opacity = geometry["grid"]["opacity"].value_or(90);
        }

        const auto &basic_view_colors = geometry["basic_view_colors"];

        const auto &layer1 = geometry["layer1"];
        const auto &layer2 = geometry["layer2"];
        const auto &layer3 = geometry["layer3"];
        const auto &water_color = geometry["water_color"];

        config.geometry.layer1.r = layer1["r"].value_or(255);
        config.geometry.layer1.g = layer1["g"].value_or(255);
        config.geometry.layer1.b = layer1["b"].value_or(255);
        config.geometry.layer1.a = layer1["a"].value_or(255);

        config.geometry.layer2.r = layer2["r"].value_or(0);
        config.geometry.layer2.g = layer2["g"].value_or(255);
        config.geometry.layer2.b = layer2["b"].value_or(0);
        config.geometry.layer2.a = layer2["a"].value_or(255);

        config.geometry.layer3.r = layer3["r"].value_or(255);
        config.geometry.layer3.g = layer3["g"].value_or(0);
        config.geometry.layer3.b = layer3["b"].value_or(0);
        config.geometry.layer3.a = layer3["a"].value_or(255);

        config.geometry.water_color.r = water_color["r"].value_or(0);
        config.geometry.water_color.g = water_color["g"].value_or(0);
        config.geometry.water_color.b = water_color["b"].value_or(255);
        config.geometry.water_color.a = water_color["a"].value_or(70);

    }
    
    deser_generic_page_config(page["tiles"], config.tiles, general_config);
    deser_generic_page_config(page["cameras"], config.cameras, general_config);
    deser_generic_page_config(page["light"], config.light, general_config);
    deser_generic_page_config(page["effects"], config.effects, general_config);
    deser_generic_page_config(page["props"], config.props, general_config);

    const auto &paths = result["paths"];

    config.data_path = std::filesystem::path(paths["data"]["path"].value_or(""));
    config.cast_path = std::filesystem::path(paths["cast"]["path"].value_or(""));
    config.assets_path = std::filesystem::path(paths["assets"]["path"].value_or(""));

    config.default_data_path = paths["data"]["default"].value_or(true);
    config.inherit_cast_path = paths["cast"]["inherit"].value_or(true);
    config.default_assets_path = paths["assets"]["default"].value_or(true);
    
    const auto &materials = paths["inits"]["materials"];
    const auto &tiles = paths["inits"]["tiles"];
    const auto &props = paths["inits"]["props"];

    materials.as_array()->for_each([&](toml::value<std::string> &path) {
        config.material_inits.push_back(std::filesystem::path(path.value_or("")));
    });
    tiles.as_array()->for_each([&](toml::value<std::string> &path) {
        config.tile_inits.push_back(std::filesystem::path(path.value_or("")));
    });
    props.as_array()->for_each([&](toml::value<std::string> &path) {
        config.prop_inits.push_back(std::filesystem::path(path.value_or("")));
    });

    return std::make_shared<Config>(std::move(config));
}

Config::Config() : 
  splashscreen(true), 
  f3(false), 
  crash_on_esc(false),
  blue_screen_of_death(true),
  event_handle_per_frame(30),
  load_per_frame(100),
  list_wrap(true),
  strict_deserialization(true),

  tiles_prerender(SpritePrerender()),
  props_prerender(SpritePrerender()),
  materials_prerender(SpritePrerender()),
  shadows(false),
  
  geometry(GeoPageConfig()),
  tiles(GenericPageConfig()),
  props(GenericPageConfig()),
  cameras(GenericPageConfig()),
  light(GenericPageConfig()),
  effects(GenericPageConfig()),
  default_sprites(GenericPageConfig())
{}

};

bool is_inherit(const toml::v3::node_view<toml::v3::node> &node) noexcept { return node["inherit"].value_or(false); }

void deser_sprite_visibility(const toml::v3::node_view<toml::v3::node> &node, mr::SpriteVisiblity &data) noexcept {
    data.inherit = node["inherit"].value_or(false);
    data.visible = node["visible"].value_or(true);
    data.opacity = node["opacity"].value_or(255);
}

void deser_generic_page_config(
    const toml::v3::node_view<toml::v3::node> &node, 
    mr::GenericPageConfig &data, 
    const mr::GenericPageConfig &default_data) noexcept 
{
    if (node["props"]["inherit"].value_or(true)) {
        data.props = default_data.props;
    } else {
        data.props.visible = node["props"]["visible"].value_or(true);
        data.props.opacity = node["props"]["opacity"].value_or(255);
    }

    if (node["tiles"]["inherit"].value_or(true)) {
        data.tiles = default_data.tiles;
    } else {
        data.tiles.visible = node["tiles"]["visible"].value_or(true);
        data.tiles.opacity = node["tiles"]["opacity"].value_or(255);
    }

    if (node["materials"]["inherit"].value_or(true)) {
        data.materials = default_data.materials;
    } else {
        data.materials.visible = node["materials"]["visible"].value_or(true);
        data.materials.opacity = node["materials"]["opacity"].value_or(255);
    }

    if (node["water"]["inherit"].value_or(true)) {
        data.water = default_data.water;
    } else {
        data.water.visible = node["water"]["visible"].value_or(true);
        data.water.opacity = node["water"]["opacity"].value_or(70);
    }

    if (node["grid"]["inherit"].value_or(true)) {
        data.grid = default_data.grid;
    } else {
        data.grid.visible = node["grid"]["visible"].value_or(true);
        data.grid.opacity = node["grid"]["opacity"].value_or(90);
    }
}
