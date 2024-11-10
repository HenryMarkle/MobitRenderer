#include "MobitRenderer/level.h"
#include <filesystem>
#include <fstream>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include <MobitParser/nodes.h>

#include <MobitRenderer/exceptions.h>
#include <MobitRenderer/serialization.h>

namespace mr {

std::shared_ptr<TileDef>
parse_tiledef(std::unique_ptr<mp::Node> const &nodes) {
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

std::unique_ptr<ProjectSaveFileNodes>
parse_project(const std::unique_ptr<ProjectSaveFileLines> &file_lines) {
  if (file_lines == nullptr)
    return nullptr;

  auto nodes = std::make_unique<ProjectSaveFileNodes>();

  nodes->geometry = mp::parse(file_lines->geometry);
  nodes->tiles = mp::parse(file_lines->tiles);

  return nodes;
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
