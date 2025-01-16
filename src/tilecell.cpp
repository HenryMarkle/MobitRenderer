#include <MobitRenderer/matrix.h>

namespace mr {

TileCell::TileCell() : 
    type(TileType::_default), 
    und_name(""),
    head_pos_x(0),
    head_pos_y(0),
    head_pos_z(0),
    tile_def(nullptr),
    material_def(nullptr) {}

TileCell::TileCell(TileDef *tile) : 
    type(TileType::head), 
    und_name(""),
    head_pos_x(0),
    head_pos_y(0),
    head_pos_z(0),
    tile_def(tile),
    material_def(nullptr) { }

TileCell::TileCell(uint16_t x, uint16_t y, uint16_t z, TileDef *tile) : 
    type(TileType::body), 
    und_name(""),
    head_pos_x(x),
    head_pos_y(y),
    head_pos_z(z),
    tile_def(tile),
    material_def(nullptr) { }

TileCell::TileCell(MaterialDef *_material) : 
    type(TileType::material), 
    und_name(""),
    head_pos_x(0),
    head_pos_y(0),
    head_pos_z(0),
    tile_def(nullptr),
    material_def(_material) { }

TileCell::TileCell(std::string name, bool material) : 
    type(material ? TileType::material : TileType::head), 
    und_name(name),
    head_pos_x(0),
    head_pos_y(0),
    head_pos_z(0),
    tile_def(nullptr),
    material_def(nullptr) { }

}