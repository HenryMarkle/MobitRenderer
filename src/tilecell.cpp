#include <MobitRenderer/matrix.h>

namespace mr {

// TileCell &TileCell::operator=(TileCell const &other) {
//     if (this == &other) return *this;

//     type = other.type;
//     und_name = other.und_name;
//     head_pos_x = other.head_pos_x;
//     head_pos_y = other.head_pos_y;
//     head_pos_z = other.head_pos_z;
//     tile_def = other.tile_def;
//     material_def = other.material_def;

//     return *this;
// }
// TileCell &TileCell::operator=(TileCell &&other) noexcept {
//     if (this == &other) return *this;

//     und_name = std::move(other.und_name);
    
//     type = other.type;
//     head_pos_x = other.head_pos_x;
//     head_pos_y = other.head_pos_y;
//     head_pos_z = other.head_pos_z;
//     tile_def = other.tile_def;
//     material_def = other.material_def;

//     other.type = TileType::_default;
//     other.head_pos_x = 0;
//     other.head_pos_y = 0;
//     other.head_pos_z = 0;
//     other.tile_def = nullptr;
//     other.material_def = nullptr;

//     return *this;
// }

// TileCell::TileCell(TileCell const &other) {
//     type = other.type;
//     und_name = other.und_name;
//     head_pos_x = other.head_pos_x;
//     head_pos_y = other.head_pos_y;
//     head_pos_z = other.head_pos_z;
//     tile_def = other.tile_def;
//     material_def = other.material_def;
// }
// TileCell::TileCell(TileCell &&other) noexcept {
//     und_name = std::move(other.und_name);
    
//     type = other.type;
//     head_pos_x = other.head_pos_x;
//     head_pos_y = other.head_pos_y;
//     head_pos_z = other.head_pos_z;
//     tile_def = other.tile_def;
//     material_def = other.material_def;

//     other.type = TileType::_default;
//     other.head_pos_x = 0;
//     other.head_pos_y = 0;
//     other.head_pos_z = 0;
//     other.tile_def = nullptr;
//     other.material_def = nullptr;
// }

TileCell::TileCell() : 
    type(TileType::_default), 
    und_name(std::string("")),
    head_pos_x(0),
    head_pos_y(0),
    head_pos_z(0),
    tile_def(nullptr),
    material_def(nullptr) {}

TileCell::TileCell(TileDef *tile) : 
    type(TileType::head), 
    und_name(std::string("")),
    head_pos_x(0),
    head_pos_y(0),
    head_pos_z(0),
    tile_def(tile),
    material_def(nullptr) { }

TileCell::TileCell(uint16_t x, uint16_t y, uint16_t z, TileDef *tile) : 
    type(TileType::body), 
    und_name(std::string("")),
    head_pos_x(x),
    head_pos_y(y),
    head_pos_z(z),
    tile_def(tile),
    material_def(nullptr) { }

TileCell::TileCell(MaterialDef *_material) : 
    type(TileType::material), 
    und_name(std::string("")),
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