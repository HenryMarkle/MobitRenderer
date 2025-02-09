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

CustomMaterialDef *deser_materialdef(const mp::Node *node) {
  const mp::Props *props = dynamic_cast<const mp::Props*>(node);
   
  if (props == nullptr) throw deserialization_failure("node is not a property list");

  std::string name;
  Color color;
  MaterialDefTexture *texture_params = nullptr;
  MaterialDefBlock *block_params = nullptr;
  MaterialDefSlope *slope_params = nullptr;
  MaterialDefFloor *floor_params = nullptr;

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

  // color
  try {
    const auto &color_node = dict.at("color");
    color = deser_color(color_node.get());
  } catch (std::out_of_range &e) {
    color = RED;
  } catch (deserialization_failure &de) {
    std::string msg("failed to deserialize property 'color': ");
    msg += de.what();
    throw deserialization_failure(msg);
  }

  // Params

  auto texture_params_iter = dict.find("texture");
  if (texture_params_iter != dict.end()) {
    mp::Node *texture_params_node = texture_params_iter->second.get();

    mp::Props *texture_props = dynamic_cast<mp::Props*>(texture_params_node);
    if (texture_props == nullptr) throw deserialization_failure("#texture property is not a property list");

    uint16_t tw, th;
    std::vector<uint8_t> trepeat;
    std::unordered_set<std::string> ttags;

    try {
      mp::Node *size_node = texture_props->map.at("sz").get();
      int w, h;
      deser_point(size_node, w, h);

      tw = (uint16_t)w;
      th = (uint16_t)h;
    } catch (std::out_of_range &re) {
      throw deserialization_failure("missing #texture property 'sz'");
    } catch (deserialization_failure &de) {
      std::string msg("failed to deserialize #texture property 'sz': ");
      msg += de.what();
      throw deserialization_failure(msg);
    }

    auto trepeat_iter = texture_props->map.find("repeatl");
    if (trepeat_iter != texture_props->map.end()) {
      try {
        trepeat = deser_uint8_vec(trepeat_iter->second.get());
      } catch (deserialization_failure &de) {
        throw deserialization_failure(
          std::string("failed to deserialize #texture property 'repeatL': ") + de.what()
        );
      }
    }

    auto ttags_iter = texture_props->map.find("tags");
    if (ttags_iter != texture_props->map.end()) {
      try {
        ttags = deser_string_set(ttags_iter->second.get());
      } catch (deserialization_failure &de) {
        throw deserialization_failure(
          std::string("failed to deserialize #texture property 'tags': ") + de.what()
        );
      }
    }

    texture_params = new MaterialDefTexture(tw, th, trepeat, ttags);
  }

  auto block_params_iter = dict.find("block");
  if (block_params_iter != dict.end()) {
    std::vector<uint8_t> brepeat;
    int brnd;
    uint8_t bbuffer;
    std::unordered_set<std::string> btags;

    mp::Props *block_props = dynamic_cast<mp::Props*>(block_params_iter->second.get());
    if (block_props == nullptr) throw deserialization_failure("#block property is not a property list");

    /// TODO: continue here..

    auto brnd_iter = block_props->map.find("rnd");
    if (brnd_iter != block_props->map.end()) {
      try {
        brnd = deser_int(brnd_iter->second.get());
      } catch (deserialization_failure &de) {
        throw deserialization_failure(
          std::string("failed to deserialize #block property 'rnd': ") + de.what()
        );
      }
    }

    auto bbuffer_iter = block_props->map.find("bftiles");
    if (bbuffer_iter != block_props->map.end()) {
      try {
        bbuffer = deser_uint8(bbuffer_iter->second.get());
      } catch (deserialization_failure &de) {
        throw deserialization_failure(
          std::string("failed to deserialize #block property 'bfTiles': ") + de.what()
        );
      }
    }

    auto btags_iter = block_props->map.find("tags");
    if (btags_iter != block_props->map.end()) {
      try {
        btags = deser_string_set(btags_iter->second.get());
      } catch (deserialization_failure &de) {
        throw deserialization_failure(
          std::string("failed to deserialize #block property 'tags': ") + de.what()
        );
      }
    }

    block_params = new MaterialDefBlock(
      brepeat,
      brnd,
      bbuffer,
      btags
    );
  }

  auto slope_params_iter = dict.find("slope");
  if (slope_params_iter != dict.end()) {
    std::vector<uint8_t> brepeat;
    int brnd;
    uint8_t bbuffer;
    std::unordered_set<std::string> btags;

    mp::Props *block_props = dynamic_cast<mp::Props*>(block_params_iter->second.get());
    if (block_props == nullptr) throw deserialization_failure("#block property is not a property list");

    /// TODO: continue here..

    auto brnd_iter = block_props->map.find("rnd");
    if (brnd_iter != block_props->map.end()) {
      try {
        brnd = deser_int(brnd_iter->second.get());
      } catch (deserialization_failure &de) {
        throw deserialization_failure(
          std::string("failed to deserialize #slope property 'rnd': ") + de.what()
        );
      }
    }

    auto bbuffer_iter = block_props->map.find("bftiles");
    if (bbuffer_iter != block_props->map.end()) {
      try {
        bbuffer = deser_uint8(bbuffer_iter->second.get());
      } catch (deserialization_failure &de) {
        throw deserialization_failure(
          std::string("failed to deserialize #slope property 'bfTiles': ") + de.what()
        );
      }
    }

    auto btags_iter = block_props->map.find("tags");
    if (btags_iter != block_props->map.end()) {
      try {
        btags = deser_string_set(btags_iter->second.get());
      } catch (deserialization_failure &de) {
        throw deserialization_failure(
          std::string("failed to deserialize #slope property 'tags': ") + de.what()
        );
      }
    }

    slope_params = new MaterialDefSlope(
      brepeat,
      brnd,
      bbuffer,
      btags
    );
  }

  auto floor_params_iter = dict.find("floor");
  if (floor_params_iter != dict.end()) {
    std::vector<uint8_t> brepeat;
    int brnd;
    uint8_t bbuffer;
    std::unordered_set<std::string> btags;

    mp::Props *block_props = dynamic_cast<mp::Props*>(block_params_iter->second.get());
    if (block_props == nullptr) throw deserialization_failure("#block property is not a property list");

    /// TODO: continue here..

    auto brnd_iter = block_props->map.find("rnd");
    if (brnd_iter != block_props->map.end()) {
      try {
        brnd = deser_int(brnd_iter->second.get());
      } catch (deserialization_failure &de) {
        throw deserialization_failure(
          std::string("failed to deserialize #floor property 'rnd': ") + de.what()
        );
      }
    }

    auto bbuffer_iter = block_props->map.find("bftiles");
    if (bbuffer_iter != block_props->map.end()) {
      try {
        bbuffer = deser_uint8(bbuffer_iter->second.get());
      } catch (deserialization_failure &de) {
        throw deserialization_failure(
          std::string("failed to deserialize #floor property 'bfTiles': ") + de.what()
        );
      }
    }

    auto btags_iter = block_props->map.find("tags");
    if (btags_iter != block_props->map.end()) {
      try {
        btags = deser_string_set(btags_iter->second.get());
      } catch (deserialization_failure &de) {
        throw deserialization_failure(
          std::string("failed to deserialize #floor property 'tags': ") + de.what()
        );
      }
    }

    floor_params = new MaterialDefFloor(
      brepeat,
      brnd,
      bbuffer,
      btags
    );
  }

  return new CustomMaterialDef(
    name, 
    color, 
    texture_params,
    block_params,
    slope_params,
    floor_params
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
std::unique_ptr<ProjectSaveFileNodes> parse_project(const std::filesystem::path &file_path) {
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
      try {
        nodes->terrain_settings = mp::parse(tokens, true);
      } catch (std::exception &e) {
        std::string msg = "failed to parse terrain settings: ";
        msg.append(e.what());
        throw parse_failure(msg);
      }
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
      try {
        nodes->cameras = mp::parse(tokens, true);
      } catch (std::exception &e) {
        std::string msg = "failed to parse cameras: ";
        msg.append(e.what());
        throw parse_failure(msg);
      }
      break;
    case 7:
      try {
        nodes->water = mp::parse(tokens, true);
      } catch (std::exception &e) {
        throw parse_failure(std::string("failed to parse water: ")+e.what());
      }
      break;
    case 8:
      try {
        nodes->props = mp::parse(tokens, true);
      } catch (std::exception &e) {
        throw parse_failure(std::string("failed to parse props: ")+e.what());
      }
      break;
    }

    if (++counter > 8)
      break;
    ;
  }

  file.close();

  return nodes;
}
std::unique_ptr<ProjectSaveFileNodes> parse_project(const std::unique_ptr<ProjectSaveFileLines> &file_lines) {
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
    throw deserialization_failure("top level node (columns) is not a linear list");

  if (columns->elements.size() != matrix.get_width()) {
    std::stringstream sb;

    sb << "incorrect matrix width; expected (" << matrix.get_width()
       << "), but got (" << columns->elements.size() << ')';

    throw deserialization_failure(sb.str());
  }

  for (auto x = 0; x < matrix.get_width(); x++) {
    auto *rows = dynamic_cast<mp::List *>(columns->elements[x].get());

    if (rows == nullptr) {
      std::stringstream sb;

      sb << "malformed geometry (expected a list of rows at column " << x
         << ")";

      throw deserialization_failure(sb.str());
    }

    if (rows->elements.size() != matrix.get_height()) {
      std::stringstream sb;

      sb << "incorrect geometry height; expected (" << matrix.get_height()
         << ") but got (" << rows->elements.size() << ')';

      throw deserialization_failure(sb.str());
    }

    for (auto y = 0; y < matrix.get_height(); y++) {
      auto *depth = dynamic_cast<mp::List *>(rows->elements[y].get());

      if (depth == nullptr)
        throw deserialization_failure(
            "malformed geometry (expected a list of cell layers)");

      if (depth->elements.size() != 3) {
        std::stringstream sb;

        sb << "incorrect geometry depth; expected (3) but got ("
           << depth->elements.size() << ')';

        throw deserialization_failure(sb.str());
      }

      auto *l1 = dynamic_cast<mp::List *>(depth->elements[0].get());
      auto *l2 = dynamic_cast<mp::List *>(depth->elements[1].get());
      auto *l3 = dynamic_cast<mp::List *>(depth->elements[2].get());

      if (l1 == nullptr) {
        std::stringstream sb;

        sb << "malformed geometry cell at (x: " << x << ", y: " << y
           << ", z: 0)";
        throw deserialization_failure(sb.str());
      }

      if (l2 == nullptr) {
        std::stringstream sb;

        sb << "malformed geometry cell at (x: " << x << ", y: " << y
           << ", z: 1)";
        throw deserialization_failure(sb.str());
      }

      if (l3 == nullptr) {
        std::stringstream sb;

        sb << "malformed geometry cell at (x: " << x << ", y: " << y
           << ", z: 2)";
        throw deserialization_failure(sb.str());
      }

      if (l1->elements.size() < 2) {
        std::stringstream sb;
        sb << "malformed geometry cell at (x: " << x << ", y: " << y
           << ", z: 0)";

        throw deserialization_failure(sb.str());
      }

      if (l2->elements.size() < 2) {
        std::stringstream sb;
        sb << "malformed geometry cell at (x: " << x << ", y: " << y
           << ", z: 1)";

        throw deserialization_failure(sb.str());
      }

      if (l3->elements.size() < 2) {
        std::stringstream sb;
        sb << "malformed geometry cell at (x: " << x << ", y: " << y
           << ", z: 2)";

        throw deserialization_failure(sb.str());
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

        throw deserialization_failure(sb.str());
      }

      if (geo2 == nullptr) {
        std::stringstream sb;
        sb << "malformed geometry cell type at (x: " << x << ", y: " << y
           << ", z: 1)";

        throw deserialization_failure(sb.str());
      }

      if (geo3 == nullptr) {
        std::stringstream sb;
        sb << "malformed geometry cell type at (x: " << x << ", y: " << y
           << ", z: 2)";

        throw deserialization_failure(sb.str());
      }

      if (features1 == nullptr) {
        std::stringstream sb;

        sb << "malformed geometry cell features at (x: " << x << ", y: " << y
           << ", z: 0";

        throw deserialization_failure(sb.str());
      }

      if (features2 == nullptr) {
        std::stringstream sb;

        sb << "malformed geometry cell features at (x: " << x << ", y: " << y
           << ", z: 1";

        throw deserialization_failure(sb.str());
      }

      if (features3 == nullptr) {
        std::stringstream sb;

        sb << "malformed geometry cell features at (x: " << x << ", y: " << y
           << ", z: 2";

        throw deserialization_failure(sb.str());
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
void deser_cameras(const mp::Node *node, std::vector<LevelCamera> &cameras) {
  const mp::Props *prop_list = dynamic_cast<const mp::Props *>(node);

  if (prop_list == nullptr)
    throw deserialization_failure("node is not a Property List");

  auto cameras_node = prop_list->map.find("cameras");

  if (cameras_node == prop_list->map.end())
    throw deserialization_failure("#cameras not found");

  auto *cameras_list = dynamic_cast<mp::List *>(cameras_node->second.get());

  if (cameras_list == nullptr)
    throw deserialization_failure("#cameras is not a Linear List");

  auto quads_node = prop_list->map.find("quads");
  
  if (quads_node == prop_list->map.end())
    throw deserialization_failure("#quads not found");

  auto *quads_list = dynamic_cast<mp::List *>(quads_node->second.get());

  if (cameras_list->elements.size() != quads_list->elements.size())
    throw deserialization_failure("#cameras and #quads mismatch (unequal element size)");

  std::vector<LevelCamera> _cameras;
  _cameras.reserve(cameras_list->elements.size());

  for (size_t e = 0; e < cameras_list->elements.size(); e++) {
    const mp::Node *camera_node = cameras_list->elements[e].get();
    const mp::Node *quad_node = quads_list->elements[e].get();

    LevelCamera camera;
    Vector2 position;

    try {
      deser_point(camera_node, position.x, position.y);
      camera.set_position(position);
    } catch (deserialization_failure &de) {
      throw deserialization_failure(
        std::string("failed to deserialize camera position at element #")
        +std::to_string(e)
        +": "
        +de.what()
      );
    }

    // Quads

    const mp::List *quad_points_node = dynamic_cast<const mp::List*>(quad_node);
    if (quad_points_node == nullptr) 
      throw deserialization_failure(
        std::string("camera quad #")
        +std::to_string(e)
        +" is not a Linear List"
      );

    // Top left
    const mp::List *tl_node = dynamic_cast<const mp::List*>(quad_points_node->elements[0].get());
    if (tl_node == nullptr)
      throw deserialization_failure(
        std::string("camera quad #")
        +std::to_string(e)
        +" top left node is not a Linear List"
      );

    if (tl_node->elements.size() < 2)
      throw deserialization_failure(
        std::string("camera quad #")
        +std::to_string(e)
        +" top left node has insufficient element count (expected at least 2)"
      );

    try {
      camera.set_top_left_angle(deser_int(tl_node->elements[0].get()));
    } catch (deserialization_failure &de) {
      throw deserialization_failure(
        std::string("failed to deserialize camera quad #")
        +std::to_string(e)
        +" top left radius: "
        +de.what()
      );
    }

    try {
      camera.set_top_left_radius(deser_float(tl_node->elements[1].get()));
    } catch (deserialization_failure &de) {
      throw deserialization_failure(
        std::string("failed to deserialize camera quad #")
        +std::to_string(e)
        +" top left angle: "
        +de.what()
      );
    }

    // Top right
    const mp::List *tr_node = dynamic_cast<const mp::List*>(quad_points_node->elements[1].get());
    if (tr_node == nullptr)
      throw deserialization_failure(
        std::string("camera quad #")
        +std::to_string(e)
        +" top left right is not a Linear List"
      );

    if (tr_node->elements.size() < 2)
      throw deserialization_failure(
        std::string("camera quad #")
        +std::to_string(e)
        +" top right node has insufficient element count (expected at least 2)"
      );

    try {
      camera.set_top_right_angle(deser_int(tr_node->elements[0].get()));
    } catch (deserialization_failure &de) {
      throw deserialization_failure(
        std::string("failed to deserialize camera quad #")
        +std::to_string(e)
        +" top right angle: "
        +de.what()
      );
    }

    try {
      camera.set_top_right_radius(deser_float(tr_node->elements[1].get()));
    } catch (deserialization_failure &de) {
      throw deserialization_failure(
        std::string("failed to deserialize camera quad #")
        +std::to_string(e)
        +" top right radius: "
        +de.what()
      );
    }

    // Bottom right
    const mp::List *br_node = dynamic_cast<const mp::List*>(quad_points_node->elements[2].get());
    if (br_node == nullptr)
      throw deserialization_failure(
        std::string("camera quad #")
        +std::to_string(e)
        +" bottom right node is not a Linear List"
      );

    if (br_node->elements.size() < 2)
      throw deserialization_failure(
        std::string("camera quad #")
        +std::to_string(e)
        +" bottom right node has insufficient element count (expected at least 2)"
      );

    try {
      camera.set_bottom_right_angle(deser_int(br_node->elements[0].get()));
    } catch (deserialization_failure &de) {
      throw deserialization_failure(
        std::string("failed to deserialize camera quad #")
        +std::to_string(e)
        +" bottom right angle: "
        +de.what()
      );
    }

    try {
      camera.set_bottom_right_radius(deser_float(br_node->elements[1].get()));
    } catch (deserialization_failure &de) {
      throw deserialization_failure(
        std::string("failed to deserialize camera quad #")
        +std::to_string(e)
        +" bottom right radius: "
        +de.what()
      );
    }

    // Bottom left
    const mp::List *bl_node = dynamic_cast<const mp::List*>(quad_points_node->elements[3].get());
    if (bl_node == nullptr)
      throw deserialization_failure(
        std::string("camera quad #")
        +std::to_string(e)
        +" bottom left node is not a Linear List"
      );

    if (bl_node->elements.size() < 2)
      throw deserialization_failure(
        std::string("camera quad #")
        +std::to_string(e)
        +" bottom left node has insufficient element count (expected at least 2)"
      );

    try {
      camera.set_bottom_left_angle(deser_int(bl_node->elements[0].get()));
    } catch (deserialization_failure &de) {
      throw deserialization_failure(
        std::string("failed to deserialize camera quad #")
        +std::to_string(e)
        +" bottom left angle: "
        +de.what()
      );
    }

    try {
      camera.set_bottom_left_radius(deser_float(bl_node->elements[1].get()));
    } catch (deserialization_failure &de) {
      throw deserialization_failure(
        std::string("failed to deserialize camera quad #")
        +std::to_string(e)
        +" bottom left radius: "
        +de.what()
      );
    }

    _cameras.push_back(camera);
  }

  cameras = std::move(_cameras);
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

void deser_buffer_geos(const mp::Node *line_node, BufferGeos &geos) {
  const mp::Props *props = dynamic_cast<const mp::Props*>(line_node);
  if (props == nullptr) 
    throw deserialization_failure("line node is not a Property List");

  const auto extra_iter = props->map.find("extratiles");
  if (extra_iter == props->map.end())
    throw deserialization_failure("#extraTiles property not found");
  
  const auto *node = extra_iter->second.get(); 

  const mp::List *list = dynamic_cast<const mp::List*>(node);

  if (list == nullptr) 
    throw deserialization_failure("node is not a Linear List");

  if (list->elements.size() < 4)
    throw deserialization_failure("list has insufficient elements (expected at least 4)");

  BufferGeos bg;

  try {
    bg.left = deser_uint16(list->elements[0].get());
  } catch (deserialization_failure &de) {
    throw deserialization_failure(
      std::string("failed to deserialize extra tiles #1 element (left): ")+de.what()
    );
  }

  try {
    bg.top = deser_uint16(list->elements[1].get());
  } catch (deserialization_failure &de) {
    throw deserialization_failure(
      std::string("failed to deserialize extra tiles #2 element (top): ")+de.what()
    );
  }

  try {
    bg.right = deser_uint16(list->elements[2].get());
  } catch (deserialization_failure &de) {
    throw deserialization_failure(
      std::string("failed to deserialize extra tiles #3 element (right): ")+de.what()
    );
  }

  try {
    bg.bottom = deser_uint16(list->elements[3].get());
  } catch (deserialization_failure &de) {
    throw deserialization_failure(
      std::string("failed to deserialize extra tiles #4 element (bottom): ")+de.what()
    );
  }

  geos = bg;
}

void deser_seed(const mp::Node *line_node, int &seed) {
  const mp::Props *props = dynamic_cast<const mp::Props*>(line_node);
  if (props == nullptr) 
    throw deserialization_failure("line node is not a Property List");

  const auto seed_iter = props->map.find("tileseed");
  if (seed_iter == props->map.end())
    throw deserialization_failure("#tileSeed property not found");
  
  const auto *node = seed_iter->second.get(); 

  try {
    seed = deser_int(node);
  } catch (deserialization_failure &de) {
    throw deserialization_failure(
      std::string("failed to deserialize #tileSeed node: ")+de.what()
    );
  }
}

void deser_water(const mp::Node *line_node, int &level, bool &in_front) {
  const mp::Props *props = dynamic_cast<const mp::Props*>(line_node);
  if (props == nullptr) 
    throw deserialization_failure("line node is not a Property List");

  const auto level_iter = props->map.find("waterlevel");
  if (level_iter == props->map.end())
    throw deserialization_failure("#waterLevel property not found");
  
  const auto infront_iter = props->map.find("waterinfront");
  if (infront_iter == props->map.end())
    throw deserialization_failure("#waterInFront property not found");
  
  int level_v;
  bool infront_v;

  try {
    level_v = deser_int(level_iter->second.get());
  } catch (deserialization_failure &de) {
    throw deserialization_failure(
      std::string("failed to deserialize #waterLevel property: ")+de.what()
    );
  }

  try {
    infront_v = deser_bool(infront_iter->second.get());
  } catch (deserialization_failure &de) {
    throw deserialization_failure(
      std::string("failed to deserialize #waterInFront property: ")+de.what()
    );
  }

  level = level_v;
  in_front = infront_v;
}

void deser_light(const mp::Node *line_node, bool &light) {
  const mp::Props *props = dynamic_cast<const mp::Props*>(line_node);
  if (props == nullptr) 
    throw deserialization_failure("line node is not a Property List");

  const auto light_iter = props->map.find("light");
  if (light_iter == props->map.end())
    throw deserialization_failure("#light property not found");
  
  try {
    light = deser_bool(light_iter->second.get());
  } catch (deserialization_failure &de) {
    throw deserialization_failure(
      std::string("failed to deserialize #light property: ")+de.what()
    );
  }
}

void deser_terrain_medium(const mp::Node *line_node, bool &setting) {
  const mp::Props *props = dynamic_cast<const mp::Props*>(line_node);
  if (props == nullptr) 
    throw deserialization_failure("line node is not a Property List");

  const auto terr_iter = props->map.find("defaultterrain");
  if (terr_iter == props->map.end())
    throw deserialization_failure("#defaultTerrain property not found");
  
  try {
    setting = deser_bool(terr_iter->second.get());
  } catch (deserialization_failure &de) {
    throw deserialization_failure(std::string("failed to deserialize #defaultTerrain property: ")+de.what());
  }
}

std::unique_ptr<Level> deser_level(const std::filesystem::path &path) {
  std::unique_ptr<ProjectSaveFileNodes> nodes = parse_project(path);

  uint16_t width, height;

  deser_size(nodes->seed_and_sizes.get(), width, height);

  auto level = std::make_unique<Level>(width, height);

  level->set_path(path);

  // Geometry

  try {
    deser_geometry_matrix(nodes->geometry.get(), level->get_geo_matrix());
  } catch (deserialization_failure &gde) {
    throw deserialization_failure(
      std::string("failed to deserialize the geometry matrix: ")+gde.what()
    );
  }

  // Seed

  try {
    deser_seed(nodes->seed_and_sizes.get(), level->seed);
  } catch (deserialization_failure &de) {
    throw deserialization_failure(
      std::string("failed to deserialize seed: ")+de.what()
    );
  }

  // Light

  try {
    deser_light(nodes->seed_and_sizes.get(), level->light);
  } catch (deserialization_failure &de) {
    throw deserialization_failure(
      std::string("failed to deserialize light: ")+de.what()
    );
  }

  // Terrain

  try {
    deser_terrain_medium(nodes->terrain_settings.get(), level->terrain);
  } catch (deserialization_failure &de) {
    throw deserialization_failure(
      std::string("failed to deserialize default terrain: ")+de.what()
    );
  } 

  // Extra geometry tiles

  try {
    deser_buffer_geos(nodes->seed_and_sizes.get(), level->buffer_geos);
  } catch (deserialization_failure &de) {
    throw deserialization_failure(
      std::string("failed to deserialize extra tiles: ")+de.what()
    );
  }

  // Tiles
  
  const mp::Props* tile_proplist = dynamic_cast<const mp::Props*>(nodes->tiles.get());
  if (tile_proplist == nullptr) 
    throw deserialization_failure("failed to deserialize the tile matrix: top-level node is not a Property list");
  
  const auto tile_matrix_iter = tile_proplist->map.find("tlmatrix");
  if (tile_matrix_iter == tile_proplist->map.end())
    throw deserialization_failure("failed to deserialize the tile matrix: #tlMatrix not found");

  try {
    deser_tile_matrix(tile_matrix_iter->second.get(), level->get_tile_matrix());
  } catch (deserialization_failure &mde) {
    throw deserialization_failure(
      std::string("failed to deserialize the tile matrix: ")+mde.what()
    );
  }

  // Cameras

  try {
    deser_cameras(nodes->cameras.get(), level->cameras);
  } catch (deserialization_failure &de) {
    throw deserialization_failure(
      std::string("failed to deserialize cameras: ")
      +de.what()
    );
  }

  // Water

  try {
    deser_water(nodes->water.get(), level->water, level->front_water);
  } catch (deserialization_failure &de) {
    throw deserialization_failure(
      std::string("failed to deserialize water: ")+de.what()
    );
  }

  // Props

  // I know that props some times are not included in the project file, 
  // but I don't care.

  const mp::Props* props_line_node = dynamic_cast<const mp::Props*>(nodes->props.get());
  if (props_line_node == nullptr) throw deserialization_failure(
    "failed to parse props: props line is not a Property List*"
  );

  auto props_iter = props_line_node->map.find("props");
  if (props_iter == props_line_node->map.end()) throw deserialization_failure(
    "failed to parse props: #props not found"
  );

  try {
    deser_props(props_iter->second.get(), level->props);
  } catch (deserialization_failure &de) {
    throw deserialization_failure(
      std::string("failed to deserialize props: ")+de.what()
    );
  }

  return level;
}

std::shared_ptr<Config> load_config(const std::filesystem::path &path) {
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

  auto c = std::make_shared<Config>();

  return nullptr;
}


}; // namespace mr
