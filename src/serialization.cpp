#include <filesystem>
#include <fstream>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include <MobitParser/nodes.h>

#include <MobitRenderer/exceptions.h>
#include <MobitRenderer/level.h>
#include <MobitRenderer/matrix.h>
#include <MobitRenderer/serialization.h>

namespace mr {

std::shared_ptr<TileDef> parse_tiledef(std::unique_ptr<mp::Node> const &nodes) {
  return nullptr;
}

std::unique_ptr<ProjectSaveFileLines>
read_project(const std::filesystem::path &file_path) {
  std::fstream file(file_path);

  if (!file.is_open()) {
    std::stringstream ss;
    ss << "could not open project save file '" << file_path << "'";
    throw file_read_error();
  }

  auto lines = std::make_unique<ProjectSaveFileLines>();

  std::string line;
  int counter = 0;

  while (std::getline(file, line, '\r')) {
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
      lines->seed_and_buffers = std::move(line);
      break;
    case 6:
      lines->cameras_and_size = std::move(line);
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

std::unique_ptr<ProjectSaveFileNodes>
parse_project(const std::unique_ptr<ProjectSaveFileLines> &file_lines) {
  if (file_lines == nullptr)
    return nullptr;

  auto nodes = std::make_unique<ProjectSaveFileNodes>();

  nodes->geometry = mp::parse(file_lines->geometry);
  nodes->tiles = mp::parse(file_lines->tiles);

  return nodes;
}

void deser_geometry(const std::unique_ptr<mp::Node> &node,
                    Matrix<GeoCell> &matrix) {
  auto *columns = dynamic_cast<mp::List *>(node.get());

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
                          GeoCell{.type = get_geo_type(geo1->number),
                                  .features = get_geo_features(features1)});
      matrix.set_noexcept(x, y, 1,
                          GeoCell{.type = get_geo_type(geo2->number),
                                  .features = get_geo_features(features2)});
      matrix.set_noexcept(x, y, 2,
                          GeoCell{.type = get_geo_type(geo3->number),
                                  .features = get_geo_features(features3)});
    }
  }
}

std::vector<LevelCamera> parse_cameras(const std::unique_ptr<mp::Node> &node) {
  auto *ptr = node.get();

  auto *prop_list = dynamic_cast<mp::Props *>(ptr);

  if (prop_list == nullptr)
    throw parse_failure("node is not a property list");

  auto &cameras_node = prop_list->map.at("cameras");

  if (cameras_node == nullptr)
    throw parse_failure("#cameras not found");

  auto *cameras_list = dynamic_cast<mp::List *>(cameras_node.get());

  if (cameras_list == nullptr)
    throw parse_failure("#cameras is malformed (not a linear list)");

  std::vector<LevelCamera> parsed_cameras;

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

  return {};
}
}; // namespace mr
