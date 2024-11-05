#pragma once

#include <string>
#include <cstdint>
#include <vector>

#include <raylib.h>

namespace mr {

    enum TileDefType : uint8_t { 
        box, 
        voxel_struct, 
        voxel_struct_random_displace_horizontal,
        voxel_struct_random_displace_vertical,
        voxel_struct_rock_type,
        voxel_struct_sand_type
    };

    // 184 bytes
    class TileDef {
        private:
        std::string name;
        TileDefType type;
        uint8_t width, height, buffer;
        int8_t rnd;
        std::vector<int8_t> specs, specs2, specs3;
        std::vector<uint8_t> repeat;
        std::vector<std::string> tags;

        Texture2D texture;
        
        public:
        const std::string& get_name() const;
        TileDefType get_type() const;
        
        int get_width() const;
        int get_height() const;
        int get_buffer() const;
        
        const std::vector<int8_t>& get_specs() const;
        const std::vector<int8_t>& get_specs2() const;
        const std::vector<int8_t>& get_specs3() const;

        const std::vector<uint8_t>& get_repeat() const;

        const std::vector<std::string>& get_tags() const;
        int get_rnd() const;

        const Texture& get_texture() const;
        void set_texture(Texture);

        TileDef& operator=(const TileDef&) = delete;
        TileDef& operator=(TileDef&&);

        TileDef(const TileDef&) = delete;
        TileDef(TileDef&&);

        TileDef(
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
        );

        ~TileDef();
    };

    class MaterialDef {
        private:

        std::string name;


        public:

        const std::string& get_name() const;
    };
};
