#pragma once

#include <map>
#include <vector>
#include <string>
#include <memory>
#include <filesystem>

#include <raylib.h>

#include <MobitRenderer/definitions.h>

namespace mr {

/// @brief Manages tile definitions.
/// @note Either the destructor or unload_textures() 
/// must be destroyed before CloseWindow().
class TileDex {

private:

std::map<std::string, TileDef*> _tiles;

/// @brief An array of tile categories, in the 
/// order they were registered.
std::vector<TileDefCategory> _categories;

/// @brief An array of tiles ordered by registering 
/// and grouped by associated category orderer.
std::vector<std::vector<TileDef*>> _sorted_tiles; 

/// @brief A map of category name with associated tiles, 
/// that are ordered by registering orderer.
std::map<std::string, std::vector<TileDef*>> _category_tiles;

/// @brief A map of categories with colors.
std::map<std::string, Color> _category_colors;

public:

/// @brief Retrieves a tile definition by its name.
/// @return A pointer to the tile if found; otherwise a null pointer is returned.
TileDef *tile(const std::string&) const noexcept;

const std::map<std::string, TileDef*> &tiles() const noexcept;

/// @brief An array of tile categories, in the 
/// order they were registered.
const std::vector<TileDefCategory> &categories() const noexcept;

/// @brief An array of tiles ordered by registering 
/// and grouped by associated category orderer.
const std::vector<std::vector<TileDef*>> &sorted_tiles() const noexcept;

/// @brief A map of category name with associated tiles, 
/// that are ordered by registering orderer.
const std::map<std::string, std::vector<TileDef*>> &category_tiles() const noexcept;

/// @brief A map of categories with colors.
const std::map<std::string, Color> &colors() const noexcept;

/// @brief Registers tiles from a directory.
/// @param file The path to the Init.txt file.
/// @attention The path's parent directory must contain the tile textures.
void register_from(std::filesystem::path const&file);

/// @brief Unloads all textures of tiles. 
/// Must be called before CloseWindow().
void unload_textures();

/// @brief Unloads all tile definitions along with
/// their textures.
/// @note Must only be called within an OpenGL context.
void unload_all();

TileDex &operator=(TileDex &&) noexcept;
TileDex &operator=(TileDex const&) = delete;

TileDex();
TileDex(TileDex&&) noexcept;
TileDex(TileDex const&) = delete;
~TileDex();

};

class MaterialDex {

private:

std::map<std::string, MaterialDef*> _materials;
std::vector<std::string> _categories;
std::vector<std::vector<MaterialDef*>> _sorted_materials;
std::map<std::string, std::vector<MaterialDef*>> _category_materials;

public:

inline MaterialDef *material(std::string const&name) const noexcept {
    auto def = _materials.find(name);
    if (def == _materials.end()) return nullptr;
    return def->second;
}
inline const std::vector<std::string> &categories() const noexcept { return _categories; }
inline const std::vector<std::vector<MaterialDef*>> &sorted_materials() const noexcept { return _sorted_materials; }
inline const std::map<std::string, std::vector<MaterialDef*>> &category_materials() const noexcept { return _category_materials; }

inline void unload_all() noexcept {
    for (auto &m : _materials) delete m.second;

    _materials.clear();
    _categories.clear();
    _sorted_materials.clear();
    _category_materials.clear();
}

void unload_textures();

/// @brief Loads internal materials.
/// @param cast_folder The path to the Cast folder.
void load_internals(std::filesystem::path const &cast_folder);

/// @brief Loads embedded materials (Drought content).
/// @param cast_folder The path to the Cast folder.
void load_embedded(std::filesystem::path const &cast_folder);

void register_from(std::filesystem::path const &file);

MaterialDex &operator=(MaterialDex const&) = delete;

MaterialDex(
    bool load_embedded = true, 
    bool load_internals = true
);
MaterialDex(MaterialDex const&) = delete;
~MaterialDex();

};

};