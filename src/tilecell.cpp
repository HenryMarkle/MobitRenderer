#include <MobitRenderer/matrix.h>

namespace mr {
    TileCell create_default_tile() {
        return TileCell {
            .type = _default,
            .und_name = "",

            .head_pos_x = 0,
            .head_pos_y = 0,
            .head_pos_z = 0,
        
            .tile_def = nullptr,
            .material_def = nullptr
        };
    }
    
    TileCell create_head_tile(std::shared_ptr<TileDef> tile) {
        return TileCell {
            .type = head,
            .und_name = "",

            .head_pos_x = 0,
            .head_pos_y = 0,
            .head_pos_z = 0,
        
            .tile_def = tile,
            .material_def = nullptr
        };
    }

    TileCell create_body_tile(uint16_t x, uint16_t y, uint16_t z, std::shared_ptr<TileDef> tile) {
        return TileCell {
            .type = body,
            .und_name = "",

            .head_pos_x = x,
            .head_pos_y = y,
            .head_pos_z = z,
        
            .tile_def = tile,
            .material_def = nullptr
        };
    }

    TileCell create_material_tile(std::shared_ptr<MaterialDef> _material) {
        return TileCell {
            .type = material,
            .und_name = "",

            .head_pos_x = 0,
            .head_pos_y = 0,
            .head_pos_z = 0,
        
            .tile_def = nullptr,
            .material_def = _material
        };
    }

    TileCell create_undefined_head_tile(std::string name) {
        return TileCell {
            .type = head,
            .und_name = name,

            .head_pos_x = 0,
            .head_pos_y = 0,
            .head_pos_z = 0,
        
            .tile_def = nullptr,
            .material_def = nullptr
        };
    }

    TileCell create_undefined_material_tile(std::string name) {
        return TileCell {
            .type = material,
            .und_name = name,

            .head_pos_x = 0,
            .head_pos_y = 0,
            .head_pos_z = 0,
        
            .tile_def = nullptr,
            .material_def = nullptr
        };
    }
}