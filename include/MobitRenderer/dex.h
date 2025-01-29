#pragma once

#include <unordered_map>
#include <vector>
#include <string>
#include <memory>
#include <filesystem>

#include <raylib.h>

#include <MobitRenderer/definitions.h>
#include <MobitRenderer/castlibs.h>

namespace mr {

/// @brief Manages tile definitions.
/// @note Either the destructor or unload_textures() 
/// must be destroyed before CloseWindow().
class TileDex {

private:

    std::unordered_map<std::string, TileDef*> _tiles;

    /// @brief An array of tile categories, in the 
    /// order they were registered.
    std::vector<TileDefCategory> _categories;

    /// @brief An array of tiles ordered by registering 
    /// and grouped by associated category orderer.
    std::vector<std::vector<TileDef*>> _sorted_tiles; 

    /// @brief A map of category name with associated tiles, 
    /// that are ordered by registering orderer.
    std::unordered_map<std::string, std::vector<TileDef*>> _category_tiles;

public:

    /// @brief Retrieves a tile definition by its name.
    /// @return A pointer to the tile if found; otherwise a null pointer is returned.
    TileDef *tile(const std::string&) const noexcept;

    const std::unordered_map<std::string, TileDef*> &tiles() const noexcept;

    /// @brief An array of tile categories, in the 
    /// order they were registered.
    const std::vector<TileDefCategory> &categories() const noexcept;

    /// @brief An array of tiles ordered by registering 
    /// and grouped by associated category orderer.
    const std::vector<std::vector<TileDef*>> &sorted_tiles() const noexcept;

    /// @brief A map of category name with associated tiles, 
    /// that are ordered by registering orderer.
    const std::unordered_map<std::string, std::vector<TileDef*>> &category_tiles() const noexcept;

    /// @brief Registers tiles from an Init text file.
    /// @param file The path to the Init.txt file.
    /// @param libs A pointer to the Cast libraries for internal tiles.
    /// @attention The path's parent directory must contain the tile textures.
    void register_from(std::filesystem::path const&file, CastLibs const*libs = nullptr);

    /// @brief Unloads all textures of tiles. 
    /// Must be called before CloseWindow().
    void unload_textures();

    /// @brief Unloads all tile definitions along with
    /// their textures.
    /// @note Must only be called within an OpenGL context.
    void unload_all();

    TileDex &operator=(TileDex const&) = delete;

    TileDex();
    TileDex(TileDex const&) = delete;
    ~TileDex();

};

/// @brief Manages prop definitions.
/// @note Either the destructor or unload_textures() 
/// must be destroyed before CloseWindow().
class PropDex {

private:

    std::unordered_map<std::string, PropDef*> _props;

    /// @brief An array of prop categories, in the 
    /// order they were registered.
    std::vector<PropDefCategory> _categories;

    /// @brief An array of props ordered by registering 
    /// and grouped by associated category orderer.
    std::vector<std::vector<PropDef*>> _sorted_props; 

    /// @brief A map of category name with associated props, 
    /// that are ordered by registering orderer.
    std::unordered_map<std::string, std::vector<PropDef*>> _category_props;

    /// Tiles as props
    std::unordered_map<std::string, TileDef*> _tiles;
    std::vector<TileDefCategory> _tile_categories;
    std::vector<std::vector<TileDef*>> _sorted_tiles;
    std::unordered_map<std::string, std::vector<TileDef*>> _category_tiles;

public:

    /// @brief Retrieves a prop definition by its name.
    /// @return A pointer to the prop if found; otherwise a null pointer is returned.
    PropDef *prop(const std::string&) const noexcept;

    const std::unordered_map<std::string, PropDef*> &props() const noexcept;

    /// @brief An array of prop categories, in the 
    /// order they were registered.
    const std::vector<PropDefCategory> &categories() const noexcept;

    /// @brief An array of props ordered by registering 
    /// and grouped by associated category orderer.
    const std::vector<std::vector<PropDef*>> &sorted_props() const noexcept;

    /// @brief A map of category name with associated props, 
    /// that are ordered by registering orderer.
    const std::unordered_map<std::string, std::vector<PropDef*>> &category_props() const noexcept;

    /// @brief Registers props from an Init text file.
    /// @param file The path to the Init.txt file.
    /// @param libs A pointer to the Cast libraries for internal props.
    /// @attention The path's parent directory must contain the prop textures.
    void register_from(std::filesystem::path const&file, CastLibs const*libs = nullptr);

    void register_tiles(const TileDex *);

    /// @brief Unloads all textures of props. 
    /// Must be called before CloseWindow().
    void unload_textures();

    /// @brief Unloads all prop definitions along with
    /// their textures.
    /// @note Must only be called within an OpenGL context.
    void unload_all();

    PropDex &operator=(PropDex const&) = delete;

    PropDex();
    PropDex(PropDex const&) = delete;
    ~PropDex();

};

class MaterialDex {

private:

    std::unordered_map<std::string, MaterialDef*> _materials;
    std::vector<std::string> _categories;
    std::vector<std::vector<MaterialDef*>> _sorted_materials;
    std::unordered_map<std::string, std::vector<MaterialDef*>> _category_materials;

public:

    inline MaterialDef *material(std::string const&name) const noexcept {
        auto def = _materials.find(name);
        if (def == _materials.end()) return nullptr;
        return def->second;
    }
    inline const std::vector<std::string> &categories() const noexcept { return _categories; }
    inline const std::vector<std::vector<MaterialDef*>> &sorted_materials() const noexcept { return _sorted_materials; }
    inline const std::unordered_map<std::string, std::vector<MaterialDef*>> &category_materials() const noexcept { return _category_materials; }

    inline void unload_all() noexcept {
        for (auto &m : _materials) delete m.second;

        _materials.clear();
        _categories.clear();
        _sorted_materials.clear();
        _category_materials.clear();
    }

    void unload_textures();

    /// @brief Loads internal materials.
    void load_internals();

    void register_from(std::filesystem::path const &file);

    MaterialDex &operator=(MaterialDex const&) = delete;

    MaterialDex();
    MaterialDex(MaterialDex const&) = delete;
    ~MaterialDex();

};

};