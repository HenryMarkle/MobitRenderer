#include <MobitRenderer/matrix.h>

namespace mr {
    TileCell create_default_tile() {
        return TileCell {
            _default,
            "",

            0,
            0,
            0,
        
            nullptr,
            nullptr
        };
    }
    
    TileCell create_head_tile(std::shared_ptr<TileDef> tile) {
        return TileCell {
            head,
            "",

            0,
            0,
            0,
        
            tile,
            nullptr
        };
    }

    TileCell create_body_tile(uint16_t x, uint16_t y, uint16_t z, std::shared_ptr<TileDef> tile) {
        return TileCell {
            body,
            "",

            x,
            y,
            z,
        
            tile,
            nullptr
        };
    }

    TileCell create_material_tile(std::shared_ptr<MaterialDef> _material) {
        return TileCell {
            material,
            "",

            0,
            0,
            0,
        
            nullptr,
            _material
        };
    }

    TileCell create_undefined_head_tile(std::string name) {
        return TileCell {
            head,
            name,

            0,
            0,
            0,
        
            nullptr,
            nullptr
        };
    }

    TileCell create_undefined_material_tile(std::string name) {
        return TileCell {
            material,
            name,

            0,
            0,
            0,
        
            nullptr,
            nullptr
        };
    }
}