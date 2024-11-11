#pragma once

#include <cstdint>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

#include <raylib.h>

#include <MobitRenderer/definitions.h>
#include <MobitRenderer/level.h>
#include <MobitRenderer/matrix.h>

#include <MobitParser/nodes.h>

namespace mr {
struct ProjectSaveFileLines {
  std::string geometry;
  std::string tiles;
  std::string effects;
  std::string light_settings;
  std::string terrain_settings;
  std::string seed_and_buffers;
  std::string cameras_and_size;
  std::string water;
  std::string props;
};

struct ProjectSaveFileNodes {
  std::unique_ptr<mp::Node> geometry;
  std::unique_ptr<mp::Node> tiles;
  std::unique_ptr<mp::Node> effects;
  std::unique_ptr<mp::Node> light_settings;
  std::unique_ptr<mp::Node> terrain_settings;
  std::unique_ptr<mp::Node> seed_and_buffers;
  std::unique_ptr<mp::Node> cameras_and_size;
  std::unique_ptr<mp::Node> water;
  std::unique_ptr<mp::Node> props;
};

// Save file parsers

std::unique_ptr<ProjectSaveFileLines>
read_project(const std::filesystem::path &);
std::unique_ptr<ProjectSaveFileNodes>
parse_project(const std::unique_ptr<ProjectSaveFileLines> &);

void parse_size(const std::unique_ptr<mp::Node> &, uint16_t &, uint16_t &);

void parse_geo_matrix(const std::unique_ptr<mp::Node> &, Matrix<GeoCell> &);
std::unique_ptr<Matrix<TileCell>>
parse_tile_matrix(const std::unique_ptr<mp::Node> &);
std::vector<mr::LevelCamera> parse_cameras(const std::unique_ptr<mp::Node> &);

// Init line parsers

std::shared_ptr<TileDef> parse_tiledef(const std::unique_ptr<mp::Node> &);

}; // namespace mr
