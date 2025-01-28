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
    else if (type_str == "customLong")     throw deserialization_failure("customLong type is not supported yet"); 
    else                                   throw deserialization_failure(std::string("unknown type: \"") + type_str + "\"");

    PropDef *def = nullptr;

    try {
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
        case PropType::_long:           def = deser_long_prop(props);            break;
        case PropType::rope:            def = deser_rope_prop(props);            break;
        }
    } catch (deserialization_failure &de) {
        throw deserialization_failure(
            std::string("failed to deserialize prop of type \""+type_str+"\": ")+de.what()
        );
    }

    return def;
}

};

mr::Standard        *deser_standard_prop        (const mp::Props*) {
    return nullptr;
}
mr::VariedStandard  *deser_varied_standard_prop (const mp::Props*) {
    return nullptr;
}
mr::Soft            *deser_soft_prop            (const mp::Props*) {
    return nullptr;
}
mr::VariedSoft      *deser_varied_soft_prop     (const mp::Props*) {
    return nullptr;
}
mr::ColoredSoft     *deser_colored_soft_prop    (const mp::Props*) {
    return nullptr;
}
mr::SoftEffect      *deser_soft_effect_prop     (const mp::Props*) {
    return nullptr;
}
mr::Decal           *deser_decal_prop           (const mp::Props*) {
    return nullptr;
}
mr::VariedDecal     *deser_varied_decal_prop    (const mp::Props*) {
    return nullptr;
}
mr::Antimatter      *deser_antimatter_prop      (const mp::Props*) {
    return nullptr;
}
mr::Long            *deser_long_prop            (const mp::Props*) {
    return nullptr;
}
mr::Rope            *deser_rope_prop            (const mp::Props*) {
    return nullptr;
}