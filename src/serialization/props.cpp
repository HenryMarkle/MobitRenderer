#include <string>
#include <vector>
#include <memory>
#include <unordered_set>
#include <cstdint>

#include <MobitRenderer/exceptions.h>
#include <MobitRenderer/serialization.h>

mr::Standard        *deser_standard_prop        (const mp::Props*);
mr::VariedStandard  *deser_varied_standard_prop (const mp::Props*);
mr::Soft            *deser_soft_prop            (const mp::Props*);
mr::VariedSoft      *deser_varied_soft_prop     (const mp::Props*);
mr::ColoredSoft     *deser_colored_soft_prop    (const mp::Props*);
mr::SoftEffect      *deser_soft_effect_prop     (const mp::Props*);
mr::Decal           *deser_decal_prop           (const mp::Props*);
mr::VariedDecal     *deser_varied_decal_prop    (const mp::Props*);
mr::Antimatter      *deser_antimatter_prop      (const mp::Props*);
mr::Long            *deser_long_prop            (const mp::Props*);
mr::Rope            *deser_rope_prop            (const mp::Props*);

namespace mr::serde {

PropDefCategory deser_propdef_category(const mp::Node *node) {
    const mp::List *list = dynamic_cast<const mp::List*>(node);
  
    if (list == nullptr) 
        throw deserialization_failure("category is not a linear list");

    mp::Node *name_node = list->elements[0].get();
    mp::String *name_string_node = dynamic_cast<mp::String*>(name_node);
    if (name_string_node == nullptr)
        throw deserialization_failure("category name is not a string");

    mp::Node *color_node = list->elements[1].get();
    Color color;

    try {
        color = deser_color(color_node);
    } catch (deserialization_failure &e) {
        std::string msg("invalid category color: ");
        msg += e.what();

        throw deserialization_failure(msg);
    }

    return PropDefCategory{name_string_node->str, color};
}

PropDef *deser_propdef(const mp::Node *node) {
    const mp::Props *props = dynamic_cast<const mp::Props*>(node);
    if (props == nullptr) throw deserialization_failure("node is not a Property List");

    const auto type_iter = props->map.find("tp");
    if (type_iter == props->map.end()) 
        throw deserialization_failure("missing required property #tp");

    std::string type_str;

    try {
        type_str = deser_string(type_iter->second.get());
    } catch (deserialization_failure &de) {
        throw deserialization_failure(std::string("failed to deserialize #tp property: ")+de.what());
    }

    PropType type;

    if      (type_str == "standard")       type = PropType::standard;
    else if (type_str == "variedStandard") type = PropType::varied_standard;
    else if (type_str == "soft")           type = PropType::soft;
    else if (type_str == "variedSoft")     type = PropType::varied_soft;
    else if (type_str == "coloredSoft")    type = PropType::colored_soft;
    else if (type_str == "softEffect")     type = PropType::soft_effect;
    else if (type_str == "simpleDecal")    type = PropType::decal; 
    else if (type_str == "variedDecal")    type = PropType::varied_decal; 
    else if (type_str == "antimatter")     type = PropType::antimatter;
    else if (type_str == "long")           type = PropType::_long;
    else if (type_str == "rope")           type = PropType::rope;
    // else if (type_str == "customLong")     throw deserialization_failure("customLong type is not supported yet"); 
    // else                                   throw deserialization_failure(std::string("unknown type: \"") + type_str + "\"");

    PropDef *def = nullptr;

    switch (type) {
    case PropType::standard:        def = deser_standard_prop(props);        break;
    case PropType::varied_standard: def = deser_varied_standard_prop(props); break;
    case PropType::soft:            def = deser_soft_prop(props);            break;
    case PropType::varied_soft:     def = deser_varied_soft_prop(props);     break;
    case PropType::colored_soft:    def = deser_colored_soft_prop(props);    break;
    case PropType::soft_effect:     def = deser_soft_effect_prop(props);     break;
    case PropType::decal:           def = deser_decal_prop(props);           break;
    case PropType::varied_decal:    def = deser_varied_decal_prop(props);    break;
    case PropType::antimatter:      def = deser_antimatter_prop(props);      break;
    // case PropType::_long:           def = deser_long_prop(props);            break;
    // case PropType::rope:            def = deser_rope_prop(props);            break;
    }

    return def;
}

};

inline void deser_nm_or_throw(const mp::Props *props, std::string &name) {
    const auto nm_iter = props->map.find("nm");
    if (nm_iter == props->map.end()) throw mr::deserialization_failure("missing required property #nm");

    try {
        name = mr::serde::deser_string(nm_iter->second.get());
    } catch (mr::deserialization_failure &de) {
        throw mr::deserialization_failure(
            std::string("failed to deserialize property #nm: ")+de.what()
        );
    }
}

inline void deser_depth_or_throw            (const mp::Node *node, uint8_t &depth) {
    try {
        depth = mr::serde::deser_uint8(node);
    } catch (mr::deserialization_failure &de) {
        throw mr::deserialization_failure(
            std::string("failed to deserialize property #depth: ")+de.what()
        );
    }
}
inline void deser_tags_or_throw             (const mp::Node *node, std::unordered_set<std::string> &tags) {
    try {
        tags = mr::serde::deser_string_set(node);
    } catch (mr::deserialization_failure &de) {
        throw mr::deserialization_failure(
            std::string("failed to deserialize property #tags: ")+de.what()
        );
    }
}
inline void deser_repeat_or_throw           (const mp::Node *node, std::vector<uint8_t> &repeat) {
    try {
        repeat = mr::serde::deser_uint8_vec(node);
    } catch (mr::deserialization_failure &de) {
        throw mr::deserialization_failure(
            std::string("failed to deserialize property #repeatL: ")+de.what()
        );
    }
}
inline void deser_color_treatment_or_throw  (const mp::Node *node, mr::PropColorTreatment &color_treatment) {
    std::string tr;
    
    try {
        tr = mr::serde::deser_string(node);
    } catch (mr::deserialization_failure &de) {
        throw mr::deserialization_failure(
            std::string("failed to deserialize property #colorTreatment: ")+de.what()
        );
    }

    if (tr == "standard") color_treatment = mr::PropColorTreatment::standard;
    else if (tr == "bevel") color_treatment = mr::PropColorTreatment::bevel;
    else throw mr::deserialization_failure(std::string("unknown #colorTreatment property value \"")+tr+"\"");
}
inline void deser_bevel_or_throw            (const mp::Node *node, int &bevel) {
    try {
        bevel = mr::serde::deser_int(node);
    } catch (mr::deserialization_failure &de) {
        throw mr::deserialization_failure(
            std::string("failed to deserialize property #bevel: ")+de.what()
        );
    }
}
inline void deser_colorize_or_throw         (const mp::Node *node, bool &colorize) {
    try {
        colorize = mr::serde::deser_bool(node);
    } catch (mr::deserialization_failure &de) {
        throw mr::deserialization_failure(
            std::string("failed to deserialize property #colorize: ")+de.what()
        );
    } 
}
inline void deser_self_shade_or_throw       (const mp::Node *node, bool &self_shade) {
    try {
        self_shade = mr::serde::deser_bool(node);
    } catch (mr::deserialization_failure &de) {
        throw mr::deserialization_failure(std::string("failed to deserialize property #selfShade: ")+de.what());
    }
}
inline void deser_random_or_throw           (const mp::Node *node, bool &random) {
    try {
        random = mr::serde::deser_bool(node);
    } catch (mr::deserialization_failure &de) {
        throw mr::deserialization_failure(
            std::string("failed to deserialize property #random: ")+de.what()
        );
    } 
}
inline void deser_round_or_throw            (const mp::Node *node, bool &round) {
    try {
        round = mr::serde::deser_bool(node);
    } catch (mr::deserialization_failure &de) {
        throw mr::deserialization_failure(std::string("failed to deserialize property #round: ")+de.what());
    }
}
inline void deser_variations_or_throw       (const mp::Node *node, uint8_t &variations) {
    try {
        variations = mr::serde::deser_uint8(node);
    } catch (mr::deserialization_failure &de) {
        throw mr::deserialization_failure(
            std::string("failed to deserialize property #vars: ")+de.what()
        );
    }
}
inline void deser_smooth_shading_or_throw   (const mp::Node *node, uint32_t &smooth_shading) {
    try {
        smooth_shading = (uint32_t)mr::serde::deser_int(node);
    } catch (mr::deserialization_failure &de) {
        throw mr::deserialization_failure(std::string("failed to deserialize property #smoothShading: ")+de.what());
    }
}
inline void deser_contour_exp_or_throw      (const mp::Node *node, float &contour_exp) {
    try {
        contour_exp = mr::serde::deser_float(node);
    } catch (mr::deserialization_failure &de) {
        throw mr::deserialization_failure(std::string("failed to deserialize property #contourExp: ")+de.what());
    }
}
inline void deser_highlight_border_or_throw (const mp::Node *node, float &highlight_border) {
    try {
        highlight_border = (uint32_t)mr::serde::deser_float(node);
    } catch (mr::deserialization_failure &de) {
        throw mr::deserialization_failure(std::string("failed to deserialize property #highLightBorder: ")+de.what());
    }
}
inline void deser_depth_aff_hil_or_throw    (const mp::Node *node, float &depth_aff) {
    try {
        depth_aff = (uint32_t)mr::serde::deser_float(node);
    } catch (mr::deserialization_failure &de) {
        throw mr::deserialization_failure(std::string("failed to deserialize property #depthAffectHilites: ")+de.what());
    }
}
inline void deser_shadow_border_or_throw    (const mp::Node *node, float &shadow_border) {
    try {
        shadow_border = (uint32_t)mr::serde::deser_float(node);
    } catch (mr::deserialization_failure &de) {
        throw mr::deserialization_failure(std::string("failed to deserialize property #shadowBorder: ")+de.what());
    }
}
inline void deser_size_or_throw             (const mp::Node *node, uint16_t &width, uint16_t &height) {
    try {
        int w, h;
        mr::serde::deser_point(node, w, h);

        width = (uint16_t)w;
        height = (uint16_t)h;
    } catch (mr::deserialization_failure &de) {
        throw mr::deserialization_failure(
            std::string("failed to deserialize property #sz: ")+de.what()
        );
    }
}
inline void deser_pixel_size_or_throw       (const mp::Node *node, uint32_t &width, uint32_t &height) {
    try {
        int w, h;
        mr::serde::deser_point(node, w, h);

        width = (uint32_t)w;
        height = (uint32_t)h;
    } catch (mr::deserialization_failure &de) {
        throw mr::deserialization_failure(std::string("failed to deserialize property #pxlSize: ")+de.what());
    }
}

mr::Standard        *deser_standard_prop        (const mp::Props *node) {
    std::string name;
    uint8_t depth = 1;
    std::unordered_set<std::string> tags;
    std::vector<uint8_t> repeat = { 1 };
    mr::PropColorTreatment color_treatment = mr::PropColorTreatment::standard;
    int bevel = 0;
    uint16_t width = 1, height = 1;

    const auto &map = node->map;
    const auto notfound = map.end();

    // Required
    deser_nm_or_throw(node, name);
    
    // Optional
    const auto depth_iter           = map.find("depth");
    const auto tags_iter            = map.find("tags"); 
    const auto repeat_iter          = map.find("repeatl");
    const auto size_iter            = map.find("sz");
    const auto color_treatment_iter = map.find("colortreatment");
    const auto bevel_iter           = map.find("bevel");

    if (depth_iter != notfound)           deser_depth_or_throw(depth_iter->second.get(), depth);
    if (tags_iter != notfound)            deser_tags_or_throw(tags_iter->second.get(), tags);
    if (repeat_iter != notfound)          deser_repeat_or_throw(repeat_iter->second.get(), repeat);
    if (size_iter != notfound)            deser_size_or_throw(size_iter->second.get(), width, height);
    if (color_treatment_iter != notfound) deser_color_treatment_or_throw(color_treatment_iter->second.get(), color_treatment);
    if (bevel_iter != notfound)           deser_bevel_or_throw(bevel_iter->second.get(), bevel);
    
    return new mr::Standard(
        depth, 
        std::move(name), 
        std::move(tags), 
        width, height, 
        std::move(repeat), 
        color_treatment, 
        bevel
    );
}
mr::VariedStandard  *deser_varied_standard_prop (const mp::Props *node) {
    std::string name;
    uint8_t depth = 1;
    std::unordered_set<std::string> tags;
    std::vector<uint8_t> repeat = { 1 };
    mr::PropColorTreatment color_treatment = mr::PropColorTreatment::standard;
    int bevel = 0;
    uint16_t width = 1, height = 1;
    uint8_t variations = 1;
    bool random = false;
    bool colorize = false;

    const auto &map = node->map;
    const auto notfound = map.end();

    // Required
    deser_nm_or_throw(node, name);
    
    // Optional
    const auto depth_iter           = map.find("depth");
    const auto tags_iter            = map.find("tags"); 
    const auto repeat_iter          = map.find("repeatl");
    const auto size_iter            = map.find("sz");
    const auto color_treatment_iter = map.find("colortreatment");
    const auto bevel_iter           = map.find("bevel");
    const auto colorize_iter        = map.find("colorize");
    const auto variations_iter      = map.find("vars");
    const auto random_iter          = map.find("random");

    if (depth_iter != notfound)           deser_depth_or_throw(depth_iter->second.get(), depth);
    if (tags_iter != notfound)            deser_tags_or_throw(tags_iter->second.get(), tags);
    if (repeat_iter != notfound)          deser_repeat_or_throw(repeat_iter->second.get(), repeat);
    if (size_iter != notfound)            deser_size_or_throw(size_iter->second.get(), width, height);
    if (color_treatment_iter != notfound) deser_color_treatment_or_throw(color_treatment_iter->second.get(), color_treatment);
    if (bevel_iter != notfound)           deser_bevel_or_throw(bevel_iter->second.get(), bevel);
    if (colorize_iter != notfound)        deser_colorize_or_throw(colorize_iter->second.get(), colorize);
    if (random_iter != notfound)          deser_random_or_throw(random_iter->second.get(), random);
    if (variations_iter != notfound)      deser_variations_or_throw(variations_iter->second.get(), variations);

    return new mr::VariedStandard(
        depth, 
        std::move(name), 
        std::move(tags), 
        width, height, 
        std::move(repeat),
        variations,
        random,
        color_treatment, 
        colorize,
        bevel
    );
}
mr::Soft            *deser_soft_prop            (const mp::Props *node) {
    std::string name;
    uint8_t depth = 1;
    std::unordered_set<std::string> tags;
    
    bool round, self_shade;
    uint32_t smooth_shading;
    float contour_exp, contour_shading, highlight_border, depth_affect_hilites, shadow_border;

    const auto &map = node->map;
    const auto notfound = map.end();

    // Required
    deser_nm_or_throw(node, name);
    
    // Optional
    const auto depth_iter            = map.find("depth");
    const auto tags_iter             = map.find("tags");
    const auto round_iter            = map.find("round");
    const auto self_shade_iter       = map.find("selfshade");
    const auto smooth_shading_iter   = map.find("smoothshading");
    const auto contour_exp_iter      = map.find("contourexp");
    const auto highlight_border_iter = map.find("highlightborder");
    const auto depth_aff_iter        = map.find("depthaffecthilites");
    const auto shadow_border_iter    = map.find("shadowborder");

    if (depth_iter != notfound)            deser_depth_or_throw(depth_iter->second.get(), depth);
    if (tags_iter != notfound)             deser_tags_or_throw(tags_iter->second.get(), tags);
    if (round_iter != notfound)            deser_round_or_throw(round_iter->second.get(), round);
    if (self_shade_iter != notfound)       deser_self_shade_or_throw(self_shade_iter->second.get(), self_shade);
    if (smooth_shading_iter != notfound)   deser_smooth_shading_or_throw(smooth_shading_iter->second.get(), smooth_shading);
    if (contour_exp_iter != notfound)      deser_contour_exp_or_throw(contour_exp_iter->second.get(), contour_exp);
    if (highlight_border_iter != notfound) deser_highlight_border_or_throw(highlight_border_iter->second.get(), highlight_border);
    if (depth_aff_iter != notfound)        deser_depth_aff_hil_or_throw(depth_aff_iter->second.get(), depth_affect_hilites);
    if (shadow_border_iter != notfound)    deser_shadow_border_or_throw(shadow_border_iter->second.get(), shadow_border);

    return new mr::Soft(
        depth, std::move(name), std::move(tags),
        smooth_shading,
        contour_exp,
        highlight_border,
        depth_affect_hilites,
        shadow_border,
        round,
        self_shade
    );
}
mr::VariedSoft      *deser_varied_soft_prop     (const mp::Props *node) {
    std::string name;
    uint8_t depth = 1;
    std::unordered_set<std::string> tags;
    
    uint8_t variations = 1;
    bool round = false, self_shade = false, colorize = false, random = false;
    uint32_t smooth_shading = 1, pixel_width, pixel_height;
    float contour_exp, contour_shading, highlight_border, depth_affect_hilites, shadow_border;

    const auto &map = node->map;
    const auto notfound = map.end();

    // Required
    deser_nm_or_throw(node, name);

    const auto pixel_size_iter = map.find("pxlsize");
    if (pixel_size_iter == notfound) throw mr::deserialization_failure("missing required property #pxlSize");       
    deser_pixel_size_or_throw(pixel_size_iter->second.get(), pixel_width, pixel_height);
    
    // Optional
    const auto depth_iter            = map.find("depth");
    const auto tags_iter             = map.find("tags");
    const auto round_iter            = map.find("round");
    const auto self_shade_iter       = map.find("selfshade");
    const auto smooth_shading_iter   = map.find("smoothshading");
    const auto contour_exp_iter      = map.find("contourexp");
    const auto highlight_border_iter = map.find("highlightborder");
    const auto depth_aff_iter        = map.find("depthaffecthilites");
    const auto shadow_border_iter    = map.find("shadowborder");
    const auto random_iter           = map.find("random");
    const auto colorize_iter         = map.find("colorize");
    const auto variations_iter       = map.find("vars");

    if (depth_iter != notfound)            deser_depth_or_throw(depth_iter->second.get(), depth);
    if (tags_iter != notfound)             deser_tags_or_throw(tags_iter->second.get(), tags);
    if (round_iter != notfound)            deser_round_or_throw(round_iter->second.get(), round);
    if (self_shade_iter != notfound)       deser_self_shade_or_throw(self_shade_iter->second.get(), self_shade);
    if (smooth_shading_iter != notfound)   deser_smooth_shading_or_throw(smooth_shading_iter->second.get(), smooth_shading);
    if (contour_exp_iter != notfound)      deser_contour_exp_or_throw(contour_exp_iter->second.get(), contour_exp);
    if (highlight_border_iter != notfound) deser_highlight_border_or_throw(highlight_border_iter->second.get(), highlight_border);
    if (depth_aff_iter != notfound)        deser_depth_aff_hil_or_throw(depth_aff_iter->second.get(), depth_affect_hilites);
    if (shadow_border_iter != notfound)    deser_shadow_border_or_throw(shadow_border_iter->second.get(), shadow_border);
    if (random_iter != notfound)           deser_random_or_throw(random_iter->second.get(), random);
    if (colorize_iter != notfound)         deser_colorize_or_throw(colorize_iter->second.get(), colorize);
    if (variations_iter != notfound)       deser_variations_or_throw(variations_iter->second.get(), variations);

    return new mr::VariedSoft(
        depth, std::move(name), std::move(tags),
        pixel_width,
        pixel_height,
        variations,
        random,
        colorize,
        smooth_shading,
        contour_exp,
        highlight_border,
        depth_affect_hilites,
        shadow_border,
        round,
        self_shade
    );
}
mr::ColoredSoft     *deser_colored_soft_prop    (const mp::Props *node) {
    std::string name;
    uint8_t depth = 1;
    std::unordered_set<std::string> tags;
    
    bool round, self_shade, colorize;
    uint32_t smooth_shading, pixel_width, pixel_height;
    float contour_exp, contour_shading, highlight_border, depth_affect_hilites, shadow_border;

    const auto &map = node->map;
    const auto notfound = map.end();

    // Required
    deser_nm_or_throw(node, name);

    const auto pixel_size_iter = map.find("pxlsize");
    if (pixel_size_iter == notfound) throw mr::deserialization_failure("missing required property #pxlSize");       
    deser_pixel_size_or_throw(pixel_size_iter->second.get(), pixel_width, pixel_height);

    // Optional
    const auto depth_iter            = map.find("depth");
    const auto tags_iter             = map.find("tags");
    const auto round_iter            = map.find("round");
    const auto self_shade_iter       = map.find("selfshade");
    const auto smooth_shading_iter   = map.find("smoothshading");
    const auto contour_exp_iter      = map.find("contourexp");
    const auto highlight_border_iter = map.find("highlightborder");
    const auto depth_aff_iter        = map.find("depthaffecthilites");
    const auto shadow_border_iter    = map.find("shadowborder");
    const auto colorize_iter         = map.find("colorize");

    if (depth_iter != notfound)            deser_depth_or_throw(depth_iter->second.get(), depth);
    if (tags_iter != notfound)             deser_tags_or_throw(tags_iter->second.get(), tags);
    if (round_iter != notfound)            deser_round_or_throw(round_iter->second.get(), round);
    if (self_shade_iter != notfound)       deser_self_shade_or_throw(self_shade_iter->second.get(), self_shade);
    if (smooth_shading_iter != notfound)   deser_smooth_shading_or_throw(smooth_shading_iter->second.get(), smooth_shading);
    if (contour_exp_iter != notfound)      deser_contour_exp_or_throw(contour_exp_iter->second.get(), contour_exp);
    if (highlight_border_iter != notfound) deser_highlight_border_or_throw(highlight_border_iter->second.get(), highlight_border);
    if (depth_aff_iter != notfound)        deser_depth_aff_hil_or_throw(depth_aff_iter->second.get(), depth_affect_hilites);
    if (shadow_border_iter != notfound)    deser_shadow_border_or_throw(shadow_border_iter->second.get(), shadow_border);
    if (colorize_iter != notfound)         deser_colorize_or_throw(colorize_iter->second.get(), colorize);

    return new mr::ColoredSoft(
        depth, std::move(name), std::move(tags),
        pixel_width,
        pixel_height,
        colorize,
        smooth_shading,
        contour_exp,
        highlight_border,
        depth_affect_hilites,
        shadow_border,
        round,
        self_shade
    );
}
mr::SoftEffect      *deser_soft_effect_prop     (const mp::Props *node) {
    std::string name;
    uint8_t depth = 1;
    std::unordered_set<std::string> tags;

    const auto &map = node->map;
    const auto notfound = map.end();

    // Required
    deser_nm_or_throw(node, name);

    // Optional
    const auto depth_iter           = map.find("depth");
    const auto tags_iter            = map.find("tags"); 

    if (depth_iter != notfound)           deser_depth_or_throw(depth_iter->second.get(), depth);
    if (tags_iter != notfound)            deser_tags_or_throw(tags_iter->second.get(), tags);

    return new mr::SoftEffect(
        depth, std::move(name), std::move(tags)
    );
}
mr::Decal           *deser_decal_prop           (const mp::Props *node) {
    std::string name;
    uint8_t depth = 1;
    std::unordered_set<std::string> tags;

    const auto &map = node->map;
    const auto notfound = map.end();

    // Required
    deser_nm_or_throw(node, name);

    // Optional
    const auto depth_iter           = map.find("depth");
    const auto tags_iter            = map.find("tags"); 

    if (depth_iter != notfound)           deser_depth_or_throw(depth_iter->second.get(), depth);
    if (tags_iter != notfound)            deser_tags_or_throw(tags_iter->second.get(), tags);

    return new mr::Decal(
        depth, std::move(name), std::move(tags)
    );
}
mr::VariedDecal     *deser_varied_decal_prop    (const mp::Props *node) {
    std::string name;
    uint8_t depth = 1;
    std::unordered_set<std::string> tags;

    bool random = false;
    uint8_t variations = 1;
    uint32_t pixel_width, pixel_height;

    const auto &map = node->map;
    const auto notfound = map.end();

    // Required
    deser_nm_or_throw(node, name);

    const auto pixel_size_iter       = map.find("pxlsize");
    if (pixel_size_iter == notfound) throw mr::deserialization_failure("missing required property #pxlSize");
    deser_pixel_size_or_throw(pixel_size_iter->second.get(), pixel_width, pixel_height);


    // Optional
    const auto depth_iter            = map.find("depth");
    const auto tags_iter             = map.find("tags");
    const auto random_iter           = map.find("random");
    const auto variations_iter       = map.find("vars");

    if (depth_iter != notfound)      deser_depth_or_throw(depth_iter->second.get(), depth);
    if (tags_iter != notfound)       deser_tags_or_throw(tags_iter->second.get(), tags);
    if (random_iter != notfound)     deser_random_or_throw(random_iter->second.get(), random);
    if (variations_iter != notfound) deser_variations_or_throw(variations_iter->second.get(), variations);

    return new mr::VariedDecal(
        depth, std::move(name), std::move(tags),
        pixel_width, pixel_height,
        variations,
        random
    );

}
mr::Antimatter      *deser_antimatter_prop      (const mp::Props *node) {
    std::string name;
    uint8_t depth = 1;
    std::unordered_set<std::string> tags;

    float contour_exp;

    const auto &map = node->map;
    const auto notfound = map.end();

    // Required
    deser_nm_or_throw(node, name);

    // Optional
    const auto depth_iter           = map.find("depth");
    const auto tags_iter            = map.find("tags");
    const auto contour_exp_iter      = map.find("contourexp");

    if (depth_iter != notfound)           deser_depth_or_throw(depth_iter->second.get(), depth);
    if (tags_iter != notfound)            deser_tags_or_throw(tags_iter->second.get(), tags);
    if (contour_exp_iter != notfound)      deser_contour_exp_or_throw(contour_exp_iter->second.get(), contour_exp);

    return new mr::Antimatter(
        depth, std::move(name), std::move(tags), contour_exp
    );
}
mr::Long            *deser_long_prop            (const mp::Props*) {
    return nullptr;
}
mr::Rope            *deser_rope_prop            (const mp::Props*) {
    return nullptr;
}