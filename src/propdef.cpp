#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <filesystem>

#include <raylib.h>

#include <MobitRenderer/definitions.h>
#include <MobitRenderer/quad.h>

namespace mr {

void PropDef::load_texture() {
    if (loaded) return;

    #ifdef IS_DEBUG_BUILD
    if (!std::filesystem::exists(texture_path)) {
        std::cout << "Warning: prop texture path not found: " << texture_path << std::endl;
        return;
    }
    #endif

    auto img = LoadImage(texture_path.string().c_str());
    ImageCrop(&img, Rectangle{0, 1, (float)img.width, (float)img.height - 1});

    texture = LoadTextureFromImage(img);
    UnloadImage(img);

    loaded = true;
}
void PropDef::unload_texture() {
    if (!loaded) return;
    UnloadTexture(texture);
    loaded = false;
}

uint32_t PropDef::get_pixel_width() const noexcept { return texture.width; }
uint32_t PropDef::get_pixel_height() const noexcept { return texture.height; }

PropDef::PropDef(uint8_t depth, std::string &&name, PropType type) :
    depth(depth), name(std::move(name)), type(type), tags({}), loaded(false), color({255, 0, 0, 255})
{}
PropDef::PropDef(uint8_t depth, std::string &&name, PropType type, std::unordered_set<std::string> &&tags) :
    depth(depth), name(std::move(name)), type(type), tags(std::move(tags)), loaded(false), color({255, 0, 0, 255})
{}

PropDef::~PropDef() {
    unload_texture();
}


Standard::Standard(
    uint8_t depth, 
    std::string &&name, 
    uint16_t width, 
    uint16_t height, 
    std::vector<uint8_t> &&repeat,
    PropColorTreatment color_treatment,
    int bevel
) :
    PropDef(depth, std::move(name), PropType::standard),
    width(width), height(height),
    repeat(std::move(repeat)),
    color_treatment(color_treatment), bevel(bevel)
{}
Standard::Standard(
    uint8_t depth, 
    std::string &&name, 
    std::unordered_set<std::string> &&tags,
    uint16_t width, 
    uint16_t height, 
    std::vector<uint8_t> &&repeat,
    PropColorTreatment color_treatment,
    int bevel
) :
    PropDef(depth, std::move(name), PropType::standard, std::move(tags)),
    width(width), height(height),
    repeat(std::move(repeat)),
    color_treatment(color_treatment), bevel(bevel)
{}


VariedStandard::VariedStandard(
    uint8_t depth, 
    std::string &&name, 
    uint16_t width, 
    uint16_t height, 
    std::vector<uint8_t> &&repeat,
    uint8_t variations,
    bool random,
    PropColorTreatment color_treatment,
    bool colorize,
    int bevel
) : PropDef(depth, std::move(name), PropType::varied_standard), width(width), height(height),
    repeat(std::move(repeat)), variations(variations), random(random), color_treatment(color_treatment),
    colorize(colorize), bevel(bevel)
{}
VariedStandard::VariedStandard(
    uint8_t depth, 
    std::string &&name, 
    std::unordered_set<std::string> &&tags,
    uint16_t width, 
    uint16_t height, 
    std::vector<uint8_t> &&repeat,
    uint8_t variations,
    bool random,
    PropColorTreatment color_treatment,
    bool colorize,
    int bevel
) : PropDef(depth, std::move(name), PropType::varied_standard, std::move(tags)), width(width), height(height),
    repeat(std::move(repeat)), variations(variations), random(random), color_treatment(color_treatment),
    colorize(colorize), bevel(bevel)
{}

Decal::Decal(uint8_t depth, std::string &&name) :
    PropDef(depth, std::move(name), PropType::decal)
{}
Decal::Decal(uint8_t depth, std::string &&name, std::unordered_set<std::string> &&tags) :
    PropDef(depth, std::move(name), PropType::decal, std::move(tags))
{}

VariedDecal::VariedDecal(
    uint8_t depth, 
    std::string &&name, 
    uint32_t pixel_width,
    uint32_t pixel_height,
    uint8_t variations,
    bool random
) : PropDef(depth, std::move(name), PropType::varied_decal), 
    pixel_width(pixel_width), pixel_height(pixel_height), variations(variations),
    random(random)
{}
VariedDecal::VariedDecal(
    uint8_t depth, 
    std::string &&name, 
    std::unordered_set<std::string> &&tags,
    uint32_t pixel_width,
    uint32_t pixel_height,
    uint8_t variations,
    bool random
) : PropDef(depth, std::move(name), PropType::varied_decal, std::move(tags)), 
    pixel_width(pixel_width), pixel_height(pixel_height), variations(variations),
    random(random)
{}

Soft::Soft(
    uint8_t depth, 
    std::string &&name,
    uint32_t smooth_shading,
    float contour_exp, 
    float highlight_border, 
    float depth_affect_hilites, 
    float shadow_border,
    bool round, 
    bool self_shade
) :
    PropDef(depth, std::move(name), PropType::soft),
    smooth_shading(smooth_shading),
    contour_exp(contour_exp), highlight_border(highlight_border), depth_affect_hilites(depth_affect_hilites),
    shadow_border(shadow_border), round(round), self_shade(self_shade)
{}
Soft::Soft(
    uint8_t depth, 
    std::string &&name, 
    std::unordered_set<std::string> &&tags, 
    uint32_t smooth_shading,
    float contour_exp, 
    float highlight_border, 
    float depth_affect_hilites, 
    float shadow_border,
    bool round, 
    bool self_shade
) :
    PropDef(depth, std::move(name), PropType::soft, std::move(tags)),
    smooth_shading(smooth_shading),
    contour_exp(contour_exp), highlight_border(highlight_border), depth_affect_hilites(depth_affect_hilites),
    shadow_border(shadow_border), round(round), self_shade(self_shade)
{}

VariedSoft::VariedSoft(
    uint8_t depth, 
    std::string &&name, 
    uint32_t pixel_width,
    uint32_t pixel_height,
    uint8_t variations,
    bool random,
    bool colorize,
    uint32_t smooth_shading,
    float contour_exp, 
    float highlight_border, 
    float depth_affect_hilites, 
    float shadow_border,
    bool round, 
    bool self_shade
) : PropDef(depth, std::move(name), PropType::varied_soft),
    pixel_width(pixel_width), pixel_height(pixel_height), 
    variations(variations), random(random), colorize(colorize), smooth_shading(smooth_shading),
    contour_exp(contour_exp), highlight_border(highlight_border), depth_affect_hilites(depth_affect_hilites),
    shadow_border(shadow_border), round(round), self_shade(self_shade)
{}
VariedSoft::VariedSoft(
    uint8_t depth, 
    std::string &&name, 
    std::unordered_set<std::string> &&tags,
    uint32_t pixel_width,
    uint32_t pixel_height,
    uint8_t variations,
    bool random,
    bool colorize,
    uint32_t smooth_shading,
    float contour_exp, 
    float highlight_border, 
    float depth_affect_hilites, 
    float shadow_border,
    bool round, 
    bool self_shade
) : PropDef(depth, std::move(name), PropType::varied_soft, std::move(tags)),
    pixel_width(pixel_width), pixel_height(pixel_height), 
    variations(variations), random(random), smooth_shading(smooth_shading),
    contour_exp(contour_exp), highlight_border(highlight_border), depth_affect_hilites(depth_affect_hilites),
    shadow_border(shadow_border), round(round), self_shade(self_shade), colorize(colorize)
{}

ColoredSoft::ColoredSoft(
    uint8_t depth, 
    std::string &&name, 
    uint32_t pixel_width,
    uint32_t pixel_height,
    bool colorize,
    uint32_t smooth_shading,
    float contour_exp, 
    float highlight_border, 
    float depth_affect_hilites, 
    float shadow_border,
    bool round, 
    bool self_shade
) : PropDef(depth, std::move(name), PropType::colored_soft),
    pixel_width(pixel_width), pixel_height(pixel_height), colorize(colorize), smooth_shading(smooth_shading),
    contour_exp(contour_exp), highlight_border(highlight_border), depth_affect_hilites(depth_affect_hilites),
    shadow_border(shadow_border), round(round), self_shade(self_shade)
{}
ColoredSoft::ColoredSoft(
    uint8_t depth, 
    std::string &&name, 
    std::unordered_set<std::string> &&tags,
    uint32_t pixel_width,
    uint32_t pixel_height,
    bool colorize,
    uint32_t smooth_shading,
    float contour_exp, 
    float highlight_border, 
    float depth_affect_hilites, 
    float shadow_border,
    bool round, 
    bool self_shade
) : PropDef(depth, std::move(name), PropType::colored_soft, std::move(tags)),
    pixel_width(pixel_width), pixel_height(pixel_height), colorize(colorize), smooth_shading(smooth_shading),
    contour_exp(contour_exp), highlight_border(highlight_border), depth_affect_hilites(depth_affect_hilites),
    shadow_border(shadow_border), round(round), self_shade(self_shade)
{}

SoftEffect::SoftEffect(uint8_t depth, std::string &&name) :
    PropDef(depth, std::move(name), PropType::soft_effect)
{}
SoftEffect::SoftEffect(uint8_t depth, std::string &&name, std::unordered_set<std::string> &&tags) :
    PropDef(depth, std::move(name), PropType::soft_effect, std::move(tags))
{}

Long::Long(uint8_t depth, std::string &&name) :
    PropDef(depth, std::move(name), PropType::_long)
{}
Long::Long(uint8_t depth, std::string &&name, std::unordered_set<std::string> &&tags) :
    PropDef(depth, std::move(name), PropType::_long, std::move(tags))
{}

Rope::Rope(uint8_t depth, std::string &&name,
    int segment_length, int collision_depth, float segment_radius, float gravity, float friction, float air_friction,
    bool stiff, float edge_direction, float rigid, float self_push, float source_push
) : PropDef(depth, std::move(name), PropType::rope),
    segment_length(segment_length), collision_depth(collision_depth), segment_radius(segment_radius),
    gravity(gravity), friction(friction), air_friction(air_friction), stiff(stiff), edge_direction(edge_direction),
    rigid(rigid), self_push(self_push), source_push(source_push)
{}
Rope::Rope(uint8_t depth, std::string &&name, std::unordered_set<std::string> &&tags,
    int segment_length, int collision_depth, float segment_radius, float gravity, float friction, float air_friction,
    bool stiff, float edge_direction, float rigid, float self_push, float source_push
) : PropDef(depth, std::move(name), PropType::rope, std::move(tags)),
    segment_length(segment_length), collision_depth(collision_depth), segment_radius(segment_radius),
    gravity(gravity), friction(friction), air_friction(air_friction), stiff(stiff), edge_direction(edge_direction),
    rigid(rigid), self_push(self_push), source_push(source_push)
{}

Antimatter::Antimatter(uint8_t depth, std::string &&name, float contour_exp) :
    PropDef(depth, std::move(name), PropType::antimatter), contour_exp(contour_exp)
{}
Antimatter::Antimatter(uint8_t depth, std::string &&name, std::unordered_set<std::string> &&tags, float contour_exp) :
    PropDef(depth, std::move(name), PropType::antimatter, std::move(tags)), contour_exp(contour_exp)
{}

PropSettings::PropSettings() :
    render_order(0),
    seed(0),
    render_time(0),

    variation(0),
    custom_depth(0),
    apply_color(false),

    release(RopeRelease::none),
    thickness(1.0f),
    segments({}) 
{}
PropSettings::PropSettings(int render_order, int seed, int render_time) :
    render_order(render_order),
    seed(seed),
    render_time(render_time),

    variation(0),
    custom_depth(0),
    apply_color(false),

    release(RopeRelease::none),
    thickness(1.0f),
    segments({})
{}

PropSettings PropSettings::standard(int render_order, int seed, int render_time) { 
    return PropSettings(render_order, seed, render_time); 
}
PropSettings PropSettings::varied_standard(int render_order, int seed, int render_time, int variation) {
    auto settings = PropSettings(render_order, seed, render_time);
    settings.variation = variation;
    return settings;
}
PropSettings PropSettings::long_(int render_order, int seed, int render_time) { 
    return PropSettings(render_order, seed, render_time); 
}
PropSettings PropSettings::soft(
    int render_order, 
    int seed, 
    int render_time, 
    int custom_depth
) {
    auto settings = PropSettings(render_order, seed, render_time);
    settings.custom_depth = custom_depth;
    return settings;
}
PropSettings PropSettings::varied_soft(
    int render_order, 
    int seed, 
    int render_time, 
    int variation, 
    int custom_depth, 
    bool apply_color
) {
    auto settings = PropSettings(render_order, seed, render_time);
    settings.variation = variation;
    settings.custom_depth = custom_depth;
    settings.apply_color = apply_color;
    return settings;
}
PropSettings PropSettings::colored_soft(int render_order, int seed, int render_time) { 
    return PropSettings(render_order, seed, render_time); 
}
PropSettings PropSettings::soft_effect(
    int render_order, 
    int seed, 
    int render_time, 
    int custom_depth
) {
    auto settings = PropSettings(render_order, seed, render_time);
    settings.custom_depth = custom_depth;
    return settings;
}
PropSettings PropSettings::decal(
    int render_order, 
    int seed, 
    int render_time, 
    int custom_depth
) {
    auto settings = PropSettings(render_order, seed, render_time);
    settings.custom_depth = custom_depth;
    return settings;
}
PropSettings PropSettings::varied_decal(
    int render_order, 
    int seed, 
    int render_time, 
    int variation, 
    int custom_depth
) {
    auto settings = PropSettings(render_order, seed, render_time);
    settings.variation = variation;
    settings.custom_depth = custom_depth;
    return settings;
}
PropSettings PropSettings::rope(
    int render_order, 
    int seed, 
    int render_time, 
    RopeRelease release,
    std::vector<Vector2> const &segments,
    float thickness,
    bool apply_color
) {
    auto settings = PropSettings(render_order, seed, render_time);
    settings.segments = segments;
    settings.thickness = thickness;
    settings.apply_color = apply_color;
    return settings;
}
PropSettings PropSettings::rope(
    int render_order, 
    int seed, 
    int render_time, 
    RopeRelease release,
    std::vector<Vector2> &&segments,
    float thickness,
    bool apply_color
) {
    auto settings = PropSettings(render_order, seed, render_time);
    settings.segments = std::move(segments);
    settings.thickness = thickness;
    settings.apply_color = apply_color;
    return settings;
}
PropSettings PropSettings::antimatter(
    int render_order, 
    int seed, 
    int render_time, 
    int custom_depth
) {
    auto settings = PropSettings(render_order, seed, render_time);
    settings.custom_depth = custom_depth;
    return settings;
}

Prop::Prop(std::shared_ptr<std::string> name, Quad const &quad) : 
    und_name(name), 
    prop_def(nullptr), 
    tile_def(nullptr), 
    quad(quad),
    settings(PropSettings())
{}

Prop::Prop(std::shared_ptr<std::string> name, PropDef *def, Quad const &quad) :
    und_name(name), 
    prop_def(def), 
    tile_def(nullptr), 
    quad(quad),
    settings(PropSettings())
{}

Prop::Prop(std::shared_ptr<std::string> name, TileDef *def, Quad const &quad) :
    und_name(name), 
    prop_def(nullptr), 
    tile_def(def), 
    quad(quad),
    settings(PropSettings())
{}

};