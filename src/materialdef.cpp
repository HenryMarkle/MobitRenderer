#include <string>

#include <MobitRenderer/definitions.h>

// To be used in unordered maps and sets
namespace std {
    template<>
    struct hash<mr::MaterialDef> {
        std::size_t operator()(const mr::MaterialDef& t) const {
            return std::hash<std::string>{}(t.get_name());
        }
    };
};

namespace mr {
    const std::string& MaterialDef::get_name() const { return name; }

};