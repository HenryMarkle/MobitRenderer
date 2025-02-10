#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <exception>

#include <raylib.h>

#include <toml++/toml.hpp>
#include <toml++/impl/forward_declarations.hpp>
#include <toml++/impl/parse_error.hpp>
#include <toml++/impl/parser.hpp>

#include <MobitParser/nodes.h>
#include <MobitParser/tokens.h>

#include <MobitRenderer/exceptions.h>
#include <MobitRenderer/level.h>
#include <MobitRenderer/matrix.h>
#include <MobitRenderer/serialization.h>
#include <MobitRenderer/state.h>
#include <MobitRenderer/dex.h>

namespace mr::serde {


TileDefCategory deser_tiledef_category(const mp::Node *node) {
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

  return TileDefCategory{name_string_node->str, color};
}

TileDef *deser_tiledef(const mp::Node *node) {
  const mp::Props *props = dynamic_cast<const mp::Props*>(node);
  
  if (props == nullptr) throw deserialization_failure("node is not a property list");
  
  std::string name, type;
  int width, height, buffer;
  int rnd;
  std::vector<int> specs1, specs2, specs3;
  std::vector<int> repeat;
  std::unordered_set<std::string> tags;
  TileDefType tile_type;

  const auto &dict = props->map;

  // Required

  // nm
  try {
    const auto &name_node = dict.at("nm");
    name = deser_string(name_node.get());
  } catch (std::out_of_range &e) {
    throw deserialization_failure("missing required propery: 'nm'");
  } catch (deserialization_failure &de) {
    std::string msg("failed to deserialize property 'nm': ");
    msg += de.what();
    throw deserialization_failure(msg);
  }

  // sz
  try {
    const auto &size_node = dict.at("sz");
    int x, y;
    deser_point(size_node.get(), x, y);
    width = (uint8_t)x;
    height = (uint8_t)y;
  } catch (std::out_of_range &e) {
    throw deserialization_failure("missing required propery: 'sz'");
  } catch (deserialization_failure &de) {
    std::string msg("failed to deserialize property 'sz': ");
    msg += de.what();
    throw deserialization_failure(msg);
  }

  // tp
  try {
    const auto &type_node = dict.at("tp");
    type = deser_string(type_node.get());
  } catch (std::out_of_range &e) {
    throw deserialization_failure("missing required propery: 'tp'");
  } catch (deserialization_failure &de) {
    std::string msg("failed to deserialize property 'tp': ");
    msg += de.what();
    throw deserialization_failure(msg);
  }

  if (type == "box") {
    tile_type = TileDefType::box;
  } 
  else if (type == "voxelStruct") {
    tile_type = TileDefType::voxel_struct;
  } 
  else if (type == "voxelStructRockType") {
    tile_type = TileDefType::voxel_struct_rock_type;
  }
  else if (type == "voxelStructSandType") {
    tile_type = TileDefType::voxel_struct_sand_type;
  }
  else if (type == "voxelStructRandomDisplaceVertical") {
    tile_type = TileDefType::voxel_struct_random_displace_vertical;
  }
  else if (type == "voxelStructRandomDisplaceHorizontal") {
    tile_type = TileDefType::voxel_struct_random_displace_horizontal;
  } else {
    std::string msg("unknown tile type '");
    msg += type;
    msg += "'";
    throw deserialization_failure(msg);
  }

  // bfTiles
  try {
    const auto &bftl_node = dict.at("bftiles");
    buffer = deser_uint8(bftl_node.get());
  } catch (std::out_of_range &e) {
    throw deserialization_failure("missing required propery: 'bfTiles'");
  } catch (deserialization_failure &de) {
    std::string msg("failed to deserialize property 'bfTiles': ");
    msg += de.what();
    throw deserialization_failure(msg);
  }

  // specs
  try {
    const auto &spc1_node = dict.at("specs");
    specs1 = deser_int_vec(spc1_node.get());
  } catch (std::out_of_range &e) {
    throw deserialization_failure("missing required propery: 'specs'");
  } catch (deserialization_failure &de) {
    std::string msg("failed to deserialize property 'specs': ");
    msg += de.what();
    throw deserialization_failure(msg);
  }

  // tags
  try {
    const auto &tags_node = dict.at("tags");
    tags = deser_string_set(tags_node.get());
  } catch (std::out_of_range &e) {
    throw deserialization_failure("missing required propery: 'tags'");
  } catch (deserialization_failure &de) {
    std::string msg("failed to deserialize property 'tags': ");
    msg += de.what();
    throw deserialization_failure(msg);
  }

  // Optional

  auto spc2_node = dict.find("specs2");
  if (spc2_node != dict.end()) {
    try {
      specs2 = deser_int_vec(spc2_node->second.get());
    } catch (deserialization_failure &e) {
      const mp::List *list = dynamic_cast<const mp::List*>(spc2_node->second.get());
      if (list != nullptr) {
        std::string msg("failed to deserialize property 'specs2': ");
        msg += e.what();
        throw deserialization_failure(msg);
      }
    }
  }

  auto spc3_node = dict.find("specs3");
  if (spc3_node != dict.end()) {
    try {
      specs3 = deser_int_vec(spc3_node->second.get());
    } catch (deserialization_failure &e) {
      const mp::List *list = dynamic_cast<const mp::List*>(spc3_node->second.get());
      if (list != nullptr) {
        std::string msg("failed to deserialize property 'specs3': ");
        msg += e.what();
        throw deserialization_failure(msg);
      }
    }
  }

  auto rand_node = dict.find("rnd");
  if (rand_node != dict.end()) {
    try {
      rnd = deser_int8(rand_node->second.get());
    } catch (deserialization_failure &e) {
      std::string msg("failed to deserialize property 'rnd': ");
      msg += e.what();
      throw deserialization_failure(msg);
    }
  }

  auto rept_node = dict.find("repeatl");
  if (rept_node != dict.end()) {
    try {
      repeat = deser_int_vec(rept_node->second.get());
    } catch (deserialization_failure &e) {
      std::string msg("failed to deserialize property 'repeatL': ");
      msg += e.what();
      throw deserialization_failure(msg);
    }
  }

  if (repeat.size() == 0) repeat.push_back(1);

  return new TileDef(
    std::move(name), 
    tile_type,
    width, height, buffer, rnd,
    std::move(tags),
    std::move(specs1),
    std::move(specs2),
    std::move(specs3),
    std::move(repeat)
  );
}

void deser_tile_matrix    (const mp::Node *node, Matrix<TileCell> &matrix) {
  const mp::List *columns = dynamic_cast<const mp::List*>(node);

  if (columns == nullptr) throw deserialization_failure("top level node (columns) is not a linear list");

  if (columns->elements.size() != matrix.get_width()) {
    std::stringstream sb;

    sb 
      << "incorrect matrix width; expected (" 
      << matrix.get_width()
      << "), but got (" 
      << columns->elements.size() 
      << ')';

    throw deserialization_failure(sb.str());
  }

  for (auto x = 0; x < matrix.get_width(); x++) {
    auto *rows = dynamic_cast<mp::List *>(columns->elements[x].get());

    if (rows == nullptr) {
      std::stringstream sb;

      sb << "malformed geometry (expected a list of rows at column " << x
         << ")";

      throw malformed_geometry(sb.str());
    }

    if (rows->elements.size() != matrix.get_height()) {
      std::stringstream sb;

      sb << "incorrect geometry height; expected (" << matrix.get_height()
         << ") but got (" << rows->elements.size() << ')';

      throw malformed_geometry(sb.str());
    }

    for (auto y = 0; y < matrix.get_height(); y++) {
      auto *depth = dynamic_cast<mp::List *>(rows->elements[y].get());

      if (depth == nullptr)
        throw deserialization_failure(
            "malformed tiles (expected a list of cell layers)");

      if (depth->elements.size() != 3) {
        std::stringstream sb;

        sb << "incorrect tiles depth; expected (3) but got ("
           << depth->elements.size() << ')';

        throw deserialization_failure(sb.str());
      }

      const auto *l1 = depth->elements[0].get();
      const auto *l2 = depth->elements[1].get();
      const auto *l3 = depth->elements[2].get();

      TileCell new_cell1, new_cell2, new_cell3;

      try {
        deser_tilecell(l1, matrix.get(x, y, 0));
      } catch (deserialization_failure &de1) {
        std::stringstream sb;
        sb 
          << "failed to deserialize cell at ("
          << x << ", " << y << ", 0): "
          << de1.what();

        throw deserialization_failure(sb.str());
      }

      try {
        deser_tilecell(l2, matrix.get(x, y, 1));
      } catch (deserialization_failure &de2) {
        std::stringstream sb;
        sb 
          << "failed to deserialize cell at ("
          << x << ", " << y << ", 1): "
          << de2.what();

        throw deserialization_failure(sb.str());
      }

      try {
        deser_tilecell(l3, matrix.get(x, y, 2));
      } catch (deserialization_failure &de3) {
        std::stringstream sb;
        sb 
          << "failed to deserialize cell at ("
          << x << ", " << y << ", 2): "
          << de3.what();

        throw deserialization_failure(sb.str());
      }
    }
  }

}

void deser_tilecell(const mp::Node *node, TileCell &cell) {
  const mp::Props *props = dynamic_cast<const mp::Props*>(node);

  if (props == nullptr) throw deserialization_failure("node is not a property list");

  auto tp_iter = props->map.find("tp");

  if (tp_iter == props->map.end())
    throw deserialization_failure("missing required cell property #tp");

  auto data_iter = props->map.find("data");

  if (data_iter == props->map.end())
    throw deserialization_failure("missing required cell property #Data");

  mp::String *tp_node = dynamic_cast<mp::String*>(tp_iter->second.get());
  if (tp_node == nullptr)
    throw deserialization_failure("cell property #tp is not a String");
  
  
  TileType type = TileType::_default;

  if (tp_node->str == "tileHead") {
    type = TileType::head;
  } else if (tp_node->str == "tileBody") {
    type = TileType::body;
  } else if (tp_node->str == "material") {
    type = TileType::material;
  } else if (tp_node->str == "default") {
    type = TileType::_default;
  } else throw deserialization_failure("unknown cell type '"+tp_node->str+"'");
  
  switch (type) {
    case TileType::head:
    {
      const mp::List *data_node = dynamic_cast<const mp::List*>(data_iter->second.get());
      if (data_node == nullptr)
        throw deserialization_failure("cell property #Data is not a Linear list (requierd for cell type 'tileHead')");

      if (data_node->elements.size() < 2)
        throw deserialization_failure("cell data has insufficient data (expected at least 2 elements)");
    
      std::string und_name;

      try {
        und_name = deser_string(data_node->elements[1].get());
      } catch (deserialization_failure &sde) {
        throw deserialization_failure(
          std::string("failed tp deserialize cell #Data node's head tile name (second element): ") + sde.what()
        );
      }

      cell = TileCell(und_name, false);
    }
    break;

    case TileType::body:
    {
      const mp::List *data_node = dynamic_cast<const mp::List*>(data_iter->second.get());
      if (data_node == nullptr)
        throw deserialization_failure("cell property #Data is not a Linear list (requierd for cell type 'tileBody')");

      if (data_node->elements.size() < 2)
        throw deserialization_failure("cell data has insufficient data (expected at least 2 elements)");
    
      int x, y, z;

      try {
        deser_point(data_node->elements[0].get(), x, y);
      } catch (deserialization_failure &pde) {
        throw deserialization_failure(
          std::string("failed to deserialize cell's #Data node's first element: ")+pde.what()
        );
      }

      try {
        z = deser_int(data_node->elements[1].get());
      } catch (deserialization_failure &pde) {
        throw deserialization_failure(
          std::string("failed to deserialize cell's #Data node's second element: ")+pde.what()
        );
      }

      cell = TileCell(x - 1, y - 1, z - 1);
    }
    break;

    case TileType::material:
    {
      const mp::String *data_node = dynamic_cast<const mp::String*>(data_iter->second.get());
      if (data_node == nullptr)
        throw deserialization_failure("cell property #Data is not a String (requierd for cell type 'material')");

      std::string und_name;

      try {
        und_name = deser_string(data_node);
      } catch (deserialization_failure &sde) {
        throw deserialization_failure(
          std::string("failed to deserialize cell #Data propery as String: ")+sde.what()
        );
      }

      auto new_cell = TileCell(und_name, true);

      cell = new_cell;
    }
    break;

    default:
    if (cell.type != TileType::_default) cell = TileCell();
    break;
  }
}
void define_tile_matrix(
  Matrix<TileCell> &mtx, 
  const TileDex *tiledex,
  const MaterialDex *materialdex
) {
  if (tiledex == nullptr)
    throw std::invalid_argument("tile dex was null");
  
  if (materialdex == nullptr)
    throw std::invalid_argument("material dex was null");

  for (uint16_t z = 0; z < 3; z++) {
    for (uint16_t x = 0; x < mtx.get_width(); x++) {
      for (uint16_t y = 0; y < mtx.get_height(); y++) {
        auto &cell = mtx.get(x, y, z);

        switch (cell.type) {
          case TileType::head:
          {
            cell.tile_def = tiledex->tile(cell.und_name);
          }
          break;

          case TileType::body:
          {
            if (mtx.is_in_bounds(cell.head_pos_x, cell.head_pos_y, cell.head_pos_z)) {
              auto &supposed_head = mtx.get(cell.head_pos_x, cell.head_pos_y, cell.head_pos_z);

              if (supposed_head.type == TileType::head) {
                if (supposed_head.tile_def == nullptr) {
                  auto *def = tiledex->tile(supposed_head.und_name);

                  cell.tile_def = def;
                  supposed_head.tile_def = def;
                }
                else
                {
                  cell.tile_def = supposed_head.tile_def;
                }
              }
            }
          }
          break;

          case TileType::material:
          {
            auto *def = materialdex->material(cell.und_name);
            cell.material_def = def;
          }
          break;

          default: continue;
        }
      }
    }
  }
}

std::string deser_default_material(const mp::Node *line_node) {
  const mp::Props *props = dynamic_cast<const mp::Props*>(line_node);
  if (props == nullptr) throw deserialization_failure("node is not a property list");

  auto iter = props->map.find("defaultmaterial");
  if (iter == props->map.end()) throw deserialization_failure("#defaultMaterial not found");

  try {
    auto name = deser_string(iter->second.get());
    return name;
  } catch (deserialization_failure &de) {
    throw deserialization_failure(std::string("failed to deserialize property #defaultMaterial: ")+de.what());
  }
}

};