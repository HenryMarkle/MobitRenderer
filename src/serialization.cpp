#if defined(_WIN32) || defined(_WIN64)
  #define WIN32_LEAN_AND_MEAN
#endif

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
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

namespace mr {

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

TileDef* deser_tiledef(const mp::Node *node) {
  const mp::Props *props = dynamic_cast<const mp::Props*>(node);
  
  if (props == nullptr) throw deserialization_failure("node is not a property list");
  
  std::string name, type;
  uint8_t width, height, buffer;
  int8_t rnd;
  std::vector<int8_t> specs1, specs2, specs3;
  std::vector<uint8_t> repeat;
  std::vector<std::string> tags;
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
    specs1 = deser_int8_vec(spc1_node.get());
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
    tags = deser_string_vec(tags_node.get());
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
      specs2 = deser_int8_vec(spc2_node->second.get());
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
      specs3 = deser_int8_vec(spc3_node->second.get());
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
      repeat = deser_uint8_vec(rept_node->second.get());
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

std::unique_ptr<ProjectSaveFileLines> read_project(const std::filesystem::path &file_path) {
  std::fstream file(file_path);

  if (!file.is_open()) {
    std::stringstream ss;
    ss << "could not open project save file '" << file_path << "'";
    throw file_read_error();
  }

  auto lines = std::make_unique<ProjectSaveFileLines>();

  std::string line;
  int counter = 0;

  std::vector<mp::token> tokens;

  // TODO: This is limited by memory (Frame buffer cannot be created).
  while (std::getline(file, line)) {
    switch (counter) {
    case 0:
      lines->geometry = std::move(line);
      break;
    case 1:
      lines->tiles = std::move(line);
      break;
    case 2:
      lines->effects = std::move(line);
      break;
    case 3:
      lines->light_settings = std::move(line);
      break;
    case 4:
      lines->terrain_settings = std::move(line);
      break;
    case 5:
      lines->seed_and_sizes = std::move(line);
      break;
    case 6:
      lines->cameras = std::move(line);
      break;
    case 7:
      lines->water = std::move(line);
      break;
    case 8:
      lines->props = std::move(line);
      break;
    }

    if (++counter > 8)
      break;
    ;
  }

  file.close();

  return lines;
}

GeoType get_geo_type(int type) {
  switch (type) {
  case 0:
    return GeoType::air;
  case 1:
    return GeoType::solid;
  case 2:
    return GeoType::slope_ne;
  case 3:
    return GeoType::slope_nw;
  case 4:
    return GeoType::slope_es;
  case 5:
    return GeoType::slope_sw;
  case 6:
    return GeoType::platform;
  case 7:
    return GeoType::shortcut_entrance;
  case 9:
    return GeoType::glass;

  default:
    return GeoType::solid;
  }
}

GeoFeature get_geo_features(const mp::List *list) {
  auto features = GeoFeature::none;

  for (auto &feature_element : list->elements) {
    auto *feature_ptr = dynamic_cast<mp::Int *>(feature_element.get());

    if (feature_ptr == nullptr)
      continue;

    GeoFeature feature;

    switch (feature_ptr->number) {
    case 1:
      feature = GeoFeature::horizontal_pole;
      break;
    case 2:
      feature = GeoFeature::vertical_pole;
      break;

    case 3:
      feature = GeoFeature::bathive;
      break;
    case 4:
      feature = GeoFeature::shortcut_entrance;
      break;
    case 5:
      feature = GeoFeature::shortcut_path;
      break;
    case 6:
      feature = GeoFeature::room_entrance;
      break;
    case 7:
      feature = GeoFeature::dragon_den;
      break;
    case 9:
      feature = GeoFeature::place_rock;
      break;
    case 10:
      feature = GeoFeature::place_spear;
      break;
    case 11:
      feature = GeoFeature::cracked_terrain;
      break;
    case 12:
      feature = GeoFeature::forbid_fly_chains;
      break;
    case 13:
      feature = GeoFeature::garbage_worm_hole;
      break;
    case 18:
      feature = GeoFeature::waterfall;
      break;
    case 19:
      feature = GeoFeature::wack_a_mole_hole;
      break;
    case 20:
      feature = GeoFeature::worm_grass;
      break;
    case 21:
      feature = GeoFeature::scavenger_hole;
      break;

    default:
      feature = GeoFeature::none;
      break;
    }

    features = features | feature;
  }

  return features;
}
std::unique_ptr<ProjectSaveFileNodes> deser_project(const std::filesystem::path &file_path) {
  std::ifstream file(file_path);

  if (!file.is_open()) {
    std::stringstream ss;
    ss << "could not open project save file '" << file_path << "'";
    throw file_read_error();
  }

  auto nodes = std::make_unique<ProjectSaveFileNodes>();

  int counter = 0;

  std::vector<mp::token> tokens;

  while (mp::tokenize_line(file, tokens)) {
    switch (counter) {
    case 0:
      try {
        nodes->geometry = mp::parse(tokens, true);
      } catch (std::exception &e) {
        std::string msg = "failed to parse geometry: ";
        msg.append(e.what());
        throw parse_failure(msg);
      }
      break;
    case 1:
      try {
        nodes->tiles = mp::parse(tokens, true);
      } catch (std::exception &e) {
        std::string msg = "failed to parse tiles: ";
        msg.append(e.what());
        throw parse_failure(msg);
      }
      break;
    case 2:
      break;
    case 3:
      break;
    case 4:
      break;
    case 5:
      try {
        nodes->seed_and_sizes = mp::parse(tokens, true);
      } catch (std::exception &e) {
        std::string msg = "failed to parse seed and sizes: ";
        msg.append(e.what());
        throw parse_failure(msg);
      }
      break;
    case 6:
      break;
    case 7:
      break;
    case 8:
      break;
    }

    if (++counter > 8)
      break;
    ;
  }

  file.close();

  return nodes;
}
std::unique_ptr<ProjectSaveFileNodes> deser_project(const std::unique_ptr<ProjectSaveFileLines> &file_lines) {
  if (file_lines == nullptr)
    return nullptr;

  auto nodes = std::make_unique<ProjectSaveFileNodes>();

  nodes->geometry = mp::parse(file_lines->geometry);
  nodes->tiles = mp::parse(file_lines->tiles);
  nodes->seed_and_sizes = mp::parse(file_lines->seed_and_sizes);

  return nodes;
}

void deser_geometry_matrix(const mp::Node *node, Matrix<GeoCell> &matrix) {
  const mp::List *columns = dynamic_cast<const mp::List *>(node);

  if (columns == nullptr)
    throw malformed_geometry("malformed geometry (expected a list of columns)");

  if (columns->elements.size() != matrix.get_width()) {
    std::stringstream sb;

    sb << "incorrect matrix width; expected (" << matrix.get_width()
       << "), but got (" << columns->elements.size() << ')';

    throw malformed_geometry(sb.str());
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
        throw malformed_geometry(
            "malformed geometry (expected a list of cell layers)");

      if (depth->elements.size() != 3) {
        std::stringstream sb;

        sb << "incorrect geometry depth; expected (3) but got ("
           << depth->elements.size() << ')';

        throw malformed_geometry(sb.str());
      }

      auto *l1 = dynamic_cast<mp::List *>(depth->elements[0].get());
      auto *l2 = dynamic_cast<mp::List *>(depth->elements[1].get());
      auto *l3 = dynamic_cast<mp::List *>(depth->elements[2].get());

      if (l1 == nullptr) {
        std::stringstream sb;

        sb << "malformed geometry cell at (x: " << x << ", y: " << y
           << ", z: 0)";
        throw malformed_geometry(sb.str());
      }

      if (l2 == nullptr) {
        std::stringstream sb;

        sb << "malformed geometry cell at (x: " << x << ", y: " << y
           << ", z: 1)";
        throw malformed_geometry(sb.str());
      }

      if (l3 == nullptr) {
        std::stringstream sb;

        sb << "malformed geometry cell at (x: " << x << ", y: " << y
           << ", z: 2)";
        throw malformed_geometry(sb.str());
      }

      if (l1->elements.size() < 2) {
        std::stringstream sb;
        sb << "malformed geometry cell at (x: " << x << ", y: " << y
           << ", z: 0)";

        throw malformed_geometry(sb.str());
      }

      if (l2->elements.size() < 2) {
        std::stringstream sb;
        sb << "malformed geometry cell at (x: " << x << ", y: " << y
           << ", z: 1)";

        throw malformed_geometry(sb.str());
      }

      if (l3->elements.size() < 2) {
        std::stringstream sb;
        sb << "malformed geometry cell at (x: " << x << ", y: " << y
           << ", z: 2)";

        throw malformed_geometry(sb.str());
      }

      auto *geo1 = dynamic_cast<mp::Int *>(l1->elements[0].get());
      auto *geo2 = dynamic_cast<mp::Int *>(l2->elements[0].get());
      auto *geo3 = dynamic_cast<mp::Int *>(l3->elements[0].get());

      auto *features1 = dynamic_cast<mp::List *>(l1->elements[1].get());
      auto *features2 = dynamic_cast<mp::List *>(l2->elements[1].get());
      auto *features3 = dynamic_cast<mp::List *>(l3->elements[1].get());

      if (geo1 == nullptr) {
        std::stringstream sb;
        sb << "malformed geometry cell type at (x: " << x << ", y: " << y
           << ", z: 0)";

        throw malformed_geometry(sb.str());
      }

      if (geo2 == nullptr) {
        std::stringstream sb;
        sb << "malformed geometry cell type at (x: " << x << ", y: " << y
           << ", z: 1)";

        throw malformed_geometry(sb.str());
      }

      if (geo3 == nullptr) {
        std::stringstream sb;
        sb << "malformed geometry cell type at (x: " << x << ", y: " << y
           << ", z: 2)";

        throw malformed_geometry(sb.str());
      }

      if (features1 == nullptr) {
        std::stringstream sb;

        sb << "malformed geometry cell features at (x: " << x << ", y: " << y
           << ", z: 0";

        throw malformed_geometry(sb.str());
      }

      if (features2 == nullptr) {
        std::stringstream sb;

        sb << "malformed geometry cell features at (x: " << x << ", y: " << y
           << ", z: 1";

        throw malformed_geometry(sb.str());
      }

      if (features3 == nullptr) {
        std::stringstream sb;

        sb << "malformed geometry cell features at (x: " << x << ", y: " << y
           << ", z: 2";

        throw malformed_geometry(sb.str());
      }

      matrix.set_noexcept(x, y, 0,
                          GeoCell{get_geo_type(geo1->number),
                                  get_geo_features(features1)});
      matrix.set_noexcept(x, y, 1,
                          GeoCell{get_geo_type(geo2->number),
                                  get_geo_features(features2)});
      matrix.set_noexcept(x, y, 2,
                          GeoCell{get_geo_type(geo3->number),
                                  get_geo_features(features3)});
    }
  }
}

/// @brief deserializes the cameras with their quads.
/// @warning Incomplete implemetation.
/// @param node 
/// @param cameras 
void deser_cameras(const mp::Node *node, std::vector<LevelCamera> &cameras) {
  const mp::Props *prop_list = dynamic_cast<const mp::Props *>(node);

  if (prop_list == nullptr)
    throw parse_failure("node is not a property list");

  auto &cameras_node = prop_list->map.at("cameras");

  if (cameras_node == nullptr)
    throw parse_failure("#cameras not found");

  auto *cameras_list = dynamic_cast<mp::List *>(cameras_node.get());

  if (cameras_list == nullptr)
    throw parse_failure("#cameras is malformed (not a linear list)");

  // four point() are expected
  for (auto &pos_node : cameras_list->elements) {
    auto *pos_call = dynamic_cast<mp::GCall *>(pos_node.get());

    if (pos_call == nullptr)
      throw parse_failure(
          "#cameras is malformed (position is not a global call)");

    if (pos_call->name != "point")
      throw parse_failure("#cameras is malformed (position is not a point)");

    const auto &args = pos_call->args;

    if (args.size() != 2)
      throw parse_failure("#cameras is malformed (position is a point but with "
                          "invalid length of arguments)");

    // TODO: complete this
  }
}

void deser_size(const mp::Node *line_node, uint16_t &width, uint16_t &height) {
  const mp::Props *props = dynamic_cast<const mp::Props *>(line_node);

  if (props == nullptr) {
    throw deserialization_failure("sizes line is not a property list");
  }

  auto size_node = props->map.at("size").get();

  auto *size_gcall = dynamic_cast<mp::GCall *>(size_node);

  if (size_gcall == nullptr) {
    throw deserialization_failure("size node is not a global call");
  }

  if (size_gcall->args.size() < 2) {
    throw deserialization_failure(
        "size global call has insufficient arguments (expected at least 2)");
  }

  auto parsed_width = size_gcall->args[0].get();
  auto parsed_height = size_gcall->args[1].get();

  auto *width_int = dynamic_cast<mp::Int *>(parsed_width);
  auto *height_int = dynamic_cast<mp::Int *>(parsed_height);

  if (width_int == nullptr) {
    throw deserialization_failure(
        "width was not an integer (arithmetical operators are not allowed)");
  }

  if (height_int == nullptr) {
    throw deserialization_failure(
        "height was not an integer (arithmetical operators are not allowed)");
  }

  width = width_int->number;
  height = height_int->number;
}

std::unique_ptr<Level> deser_level(const std::filesystem::path &path) {
  std::unique_ptr<ProjectSaveFileNodes> nodes = deser_project(path);

  uint16_t width, height;

  deser_size(nodes->seed_and_sizes.get(), width, height);

  auto level = std::make_unique<Level>(width, height);

  deser_geometry_matrix(nodes->geometry.get(), level->get_geo_matrix());

  return level;
}

std::shared_ptr<config> load_config(const std::filesystem::path &path) {
  toml::parse_result parsed;

  try {
    parsed = toml::parse_file(path.string());
  } catch (toml::parse_error &pe) {
    std::cerr << "exception: could not parse config file: " << pe.what() << ": "
              << pe.description() << std::endl;
  } catch (std::exception &e) {
    std::cerr << "exception: could not read config file: " << e.what()
              << std::endl;
    return nullptr;
  }

  auto c = std::make_shared<config>();

  return nullptr;
}

int deser_int(const mp::Node *node) {
  const mp::Int *int_node = dynamic_cast<const mp::Int*>(node);
  if (int_node == nullptr) throw deserialization_failure("node is not an Int");
  return int_node->number;
}
int8_t deser_int8(const mp::Node *node) {
  const mp::Int *int_node = dynamic_cast<const mp::Int*>(node);
  if (int_node == nullptr) throw deserialization_failure("node is not an uint8");
  return (int8_t)int_node->number;
}
uint8_t deser_uint8(const mp::Node *node) {
  const mp::Int *int_node = dynamic_cast<const mp::Int*>(node);
  if (int_node == nullptr) throw deserialization_failure("node is not an uint8");
  return (uint8_t)int_node->number;
}
std::string deser_string(const mp::Node *node) {
  const mp::String *str_node = dynamic_cast<const mp::String*>(node);
  if (str_node == nullptr) throw deserialization_failure("node is not a String");
  return str_node->str;
}
Color deser_color(const mp::Node *node) {
  const mp::GCall *color_gcall_node = dynamic_cast<const mp::GCall*>(node);
  if (color_gcall_node == nullptr)
    throw deserialization_failure("color is not a global call");
  if (color_gcall_node->name != "color")
    throw deserialization_failure("color is not a global call named 'color'");
  if (color_gcall_node->args.size() < 3)
    throw deserialization_failure("color has insufficient arguments (expected at least 3)");

  uint8_t r, g, b;

  try {
    r = deser_uint8(color_gcall_node->args[0].get());
  } catch (deserialization_failure &e) {
    std::string msg("invalid color argument 1: ");
    msg.append(e.what());

    throw deserialization_failure(msg);
  }

  try {
    g = deser_uint8(color_gcall_node->args[1].get());
  } catch (deserialization_failure &e) {
    std::string msg("invalid color argument 2: ");
    msg.append(e.what());
  
    throw deserialization_failure(msg);
  }

  try {
    b = deser_uint8(color_gcall_node->args[2].get());
  } catch (deserialization_failure &e) {
    std::string msg("invalid color argument 2: ");
    msg.append(e.what());
    
    throw deserialization_failure(msg);
  }

  return Color{r, g, b, 255};
}
std::vector<std::string> deser_string_vec(const mp::Node *node) {
  const mp::List *list = dynamic_cast<const mp::List*>(node);
  if (list == nullptr) throw deserialization_failure("node is not a linear list");

  std::vector<std::string> strings;
  strings.reserve(list->elements.size());

  try {
    for (auto &element : list->elements) {
      mp::Node *element_node = element.get();
      mp::String *element_string = dynamic_cast<mp::String*>(element_node);
      if (element_string == nullptr) throw deserialization_failure("failed to deserialize list element: node is not a string");
      strings.push_back(element_string->str);
    }
  } catch (deserialization_failure &e) {
    std::string msg("failed to deserialize list element: ");
    msg += e.what();
    throw deserialization_failure(msg);
  }

  return strings;
}
std::vector<int8_t> deser_int8_vec (const mp::Node *node) {
  const mp::List *list = dynamic_cast<const mp::List*>(node);
  if (list == nullptr) throw deserialization_failure("node is not a linear list");

  std::vector<int8_t> numbers;
  numbers.reserve(list->elements.size());

  try {
    for (auto &element : list->elements) {
      int8_t number = deser_int8(element.get());
      numbers.push_back(number);
    }
  } catch (deserialization_failure &e) {
    std::string msg("failed to deserialize list element: ");
    msg += e.what();
    throw deserialization_failure(msg);
  }

  return numbers;
}
std::vector<uint8_t> deser_uint8_vec (const mp::Node *node) {
  const mp::List *list = dynamic_cast<const mp::List*>(node);
  if (list == nullptr) throw deserialization_failure("node is not a linear list");

  std::vector<uint8_t> numbers;
  numbers.reserve(list->elements.size());

  try {
    for (auto &element : list->elements) {
      uint8_t number = deser_uint8(element.get());
      numbers.push_back(number);
    }
  } catch (deserialization_failure &e) {
    std::string msg("failed to deserialize list element: ");
    msg += e.what();
    throw deserialization_failure(msg);
  }

  return numbers;
}
void deser_point(const mp::Node *node, int &x, int &y) {
  const mp::GCall *gcall_node = dynamic_cast<const mp::GCall*>(node);
  
  if (gcall_node == nullptr) throw deserialization_failure("node is not a Global Call");
  if (gcall_node->name != "point") throw deserialization_failure("global call is not a point");
  if (gcall_node->args.size() < 2) throw deserialization_failure("point global call has insufficient arguments (expected at least 2)");

  int value_x, value_y;

  try {
    value_x = deser_int(gcall_node->args[0].get());
  } catch (deserialization_failure &e) {
    std::string msg("failed to deserialize point argument 1: ");
    msg += e.what();
    throw deserialization_failure(msg);
  }

  try {
    value_y = deser_int(gcall_node->args[1].get());
  } catch (deserialization_failure &e) {
    std::string msg("failed to deserialize point argument 2: ");
    msg += e.what();
    throw deserialization_failure(msg);
  }

  x = value_x;
  y = value_y;
}

void define_tile_matrix(Matrix<TileCell> &mtx, const TileDex &dex) {
  for (uint16_t x = 0; x < mtx.get_width(); x++) {
    for (uint16_t y = 0; y < mtx.get_height(); y++) {
      for (uint16_t z = 0; z < 3; z++) {
        auto &cell = mtx.get(x, y, z);

        switch (cell.type) {
          case TileType::head:
          {
            cell.tile_def = dex.tile(cell.und_name);
          }
          break;

          case TileType::body:
          {
            if (mtx.is_in_bounds(cell.head_pos_x, cell.head_pos_y, cell.head_pos_z)) {
              auto &supposed_head = mtx.get(cell.head_pos_x, cell.head_pos_y, cell.head_pos_z);

              if (supposed_head.type == TileType::head) {
                if (supposed_head.tile_def == nullptr) {
                  auto *def = dex.tile(supposed_head.und_name);

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
            // TODO: Continue here..
          }
          break;

          default: continue;
        }
      }
    }
  }
}

}; // namespace mr
