#pragma once

#include <map>
#include <vector>
#include <string>
#include <memory>
#include <filesystem>

#include <raylib.h>

#include <MobitRenderer/definitions.h>

namespace mr {

class TileDex {

private:

std::map<std::string, std::shared_ptr<TileDef>> _tiles;

/// @brief An array of tile categories, in the 
/// order they were registered.
std::vector<std::string> _categories;

/// @brief An array of tiles ordered by registering 
/// and grouped by associated category orderer.
std::vector<std::vector<std::shared_ptr<TileDef>>> _sorted_tiles; 

/// @brief A map of category name with associated tiles, 
/// that are ordered by registering orderer.
std::map<std::string, std::vector<std::shared_ptr<TileDef>>> _category_tiles;

/// @brief A map of categories with colors.
std::map<std::string, Color> _category_colors;

public:

const TileDef *tile(const std::string&) const noexcept;


const std::map<std::string, std::shared_ptr<TileDef>> &tiles() const noexcept;

/// @brief An array of tile categories, in the 
/// order they were registered.
const std::vector<std::string> &categories() const noexcept;

/// @brief An array of tiles ordered by registering 
/// and grouped by associated category orderer.
const std::vector<std::vector<std::shared_ptr<TileDef>>> &sorted_tiles() const noexcept;

/// @brief A map of category name with associated tiles, 
/// that are ordered by registering orderer.
const std::map<std::string, std::vector<std::shared_ptr<TileDef>>> &category_tiles() const noexcept;

/// @brief A map of categories with colors.
const std::map<std::string, Color> &colors() const noexcept;

/// @brief Registers tiles from a directory.
/// @param file The path to the Init.txt file.
/// @attention The path's parent directory must contain the tile textures.
void register_from_dir(std::filesystem::path const&file);

/// @brief Unloads all textures of tiles.
void unload_textures();

TileDex &operator=(TileDex &&) noexcept;
TileDex &operator=(TileDex const&) = delete;

TileDex();
TileDex(TileDex &&) noexcept;
TileDex(TileDex const&) = delete;
~TileDex();

};

};