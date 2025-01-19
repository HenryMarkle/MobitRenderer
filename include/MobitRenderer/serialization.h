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
#include <MobitRenderer/dex.h>
#include <MobitParser/nodes.h>

namespace mr {

struct ProjectSaveFileLines {
  std::string geometry;
  std::string tiles;
  std::string effects;
  std::string light_settings;
  std::string terrain_settings;
  std::string seed_and_sizes;
  std::string cameras;
  std::string water;
  std::string props;
};

struct ProjectSaveFileNodes {
  std::unique_ptr<mp::Node> geometry;
  std::unique_ptr<mp::Node> tiles;
  std::unique_ptr<mp::Node> effects;
  std::unique_ptr<mp::Node> light_settings;
  std::unique_ptr<mp::Node> terrain_settings;
  std::unique_ptr<mp::Node> seed_and_sizes;
  std::unique_ptr<mp::Node> cameras;
  std::unique_ptr<mp::Node> water;
  std::unique_ptr<mp::Node> props;
};

// Save file parsers

std::unique_ptr<ProjectSaveFileLines> read_project(const std::filesystem::path &);
std::unique_ptr<ProjectSaveFileNodes> deser_project(const std::unique_ptr<ProjectSaveFileLines> &);
std::unique_ptr<ProjectSaveFileNodes> deser_project(const std::filesystem::path &);

void deser_size           (const mp::Node*, uint16_t &width, uint16_t &height);
void deser_geometry_matrix(const mp::Node*, Matrix<GeoCell>&);
void deser_tile_matrix    (const mp::Node*, Matrix<TileCell>&);
void deser_cameras        (const mp::Node*, std::vector<mr::LevelCamera>&);

std::unique_ptr<Level> deser_level(const std::filesystem::path&);

// Init line parsers

TileDefCategory deser_tiledef_category(const mp::Node*);
TileDef* deser_tiledef(const mp::Node*);

// Deserialization utilities

/// @brief Deserializes an mp::Int node; expects no operators.
int deser_int(const mp::Node*);

/// @brief Deserializes an mp::Int node; expects no operators.
/// @returns A casted int to uint8 (unsigned char).
int8_t deser_int8(const mp::Node*);

/// @brief Deserializes an mp::Int node; expects no operators.
/// @returns A casted int to uint8 (unsigned char).
uint8_t deser_uint8(const mp::Node*);

/// @brief Deserializes a string node.
std::string deser_string(const mp::Node*);

/// @brief Deserializes an mp::GCall node; expects no operators.
/// @returns A raylib color with Alpha channel set to 255.
Color deser_color(const mp::Node*);

/// @brief Deserializes a linear list into a vector of std::string.
/// @param node The underlying pointer must be mp::List*.
std::vector<std::string> deser_string_vec(const mp::Node *node);

/// @brief Deserializes a linear list into a vector of int8 (char).
/// @param node The underlying pointer must be mp::List*.
std::vector<int8_t> deser_int8_vec(const mp::Node *node);

/// @brief Deserializes a linear list into a vector of uint8 (unsigned char).
/// @param node The underlying pointer must be mp::List*.
std::vector<uint8_t> deser_uint8_vec(const mp::Node *node);

/// @brief Deserializes a 'point' global call.
/// @param node The underlying pointer must be mp::GCall* and the name must be 'point'.
void deser_point(const mp::Node *node, int&, int&);

// TODO: Maybe move this section somewhere else

/// @brief Goes through a tile matrix and defines each cell that 
/// is a tile head, body, or a material.
/// @note This function fails silently.
void define_tile_matrix(Matrix<TileCell>&, TileDex const&);

}; // namespace mr
