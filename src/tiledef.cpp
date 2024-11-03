#include <string>

#include "definitions.h"

#include "raylib.h"

// To be used in unordered maps and sets
namespace std {
    template<>
    struct hash<mr::TileDef> {
        std::size_t operator()(const mr::TileDef& t) const {
            return std::hash<std::string>{}(t.get_name());
        }
    };
};

namespace mr {
    const std::string& TileDef::get_name() const { return name; }

    TileDefType TileDef::get_type() const { return type; }
    
    int TileDef::get_width() const { return width; }
    int TileDef::get_height() const { return height; }
    int TileDef::get_buffer() const { return buffer; }
    
    const std::vector<int8_t>& TileDef::get_specs() const { return specs; }
    const std::vector<int8_t>& TileDef::get_specs2() const { return specs2; }
    const std::vector<int8_t>& TileDef::get_specs3() const { return specs3; }

    const std::vector<uint8_t>& TileDef::get_repeat() const { return repeat; }

    const std::vector<std::string>& TileDef::get_tags() const { return tags; }
    int TileDef::get_rnd() const { return rnd; }

    const Texture& TileDef::get_texture() const { return texture; }

    void TileDef::set_texture(Texture t)
    {
        if (texture.id != 0) UnloadTexture(texture);

        texture = t;
    }

    TileDef::TileDef(TileDef&& other) {
        if (texture.id != 0) UnloadTexture(texture);

        name = std::move(other.name);
        type = other.type;

        width = other.width;
        height = other.height;
        buffer = other.buffer;
        rnd = other.rnd;

        specs = std::move(other.specs);
        specs2 = std::move(other.specs2);
        specs3 = std::move(other.specs3);
        repeat = std::move(other.repeat);
        tags = std::move(other.tags);

        texture = other.texture;

        other.texture = Texture2D{};
        other.width = 0;
        other.height = 0;
        other.buffer = 0;
        other.rnd = 0;
    }

    TileDef::TileDef(
        std::string name, 
        TileDefType type, 
        uint8_t width, 
        uint8_t height, 
        uint8_t buffer, 
        int8_t rnd, 
        std::vector<std::string> tags,
        std::vector<int8_t> specs,
        std::vector<int8_t> specs2,
        std::vector<int8_t> specs3,
        std::vector<uint8_t> repeat
    ) 
        : name(name), type(type), width(width), height(height), 
          buffer(buffer), rnd(rnd), tags(tags), specs(specs),
          specs2(specs2), specs3(specs3), repeat(repeat),
          texture(Texture2D{})
    { }

    TileDef::~TileDef()
    {
        if (texture.id != 0) UnloadTexture(texture);
        texture = Texture2D{};
    }
};
