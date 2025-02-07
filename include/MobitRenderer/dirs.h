#pragma once

#include <filesystem>

namespace mr {

class Dirs {

private:

  std::filesystem::path executable;
  std::filesystem::path assets, projects, levels, data, datapacks, logs;
  std::filesystem::path shaders, palettes, fonts, materials, tiles, props, cast;
  std::filesystem::path tilepacks, proppacks, materialpacks;

  bool 
    ok, 
    executable_found,
    
    assets_found,
    palettes_found,
    shaders_found,
    fonts_found,
    
    data_found,
    datapacks_found,
    tiles_found,
    materials_found,
    props_found,
    cast_found,

    tilepacks_found,
    proppacks_found,
    materialpacks_found,
    
    projects_found,
    levels_found,
    logs_found;

public:

  inline const std::filesystem::path &get_executable() const noexcept { return executable; }

  inline const std::filesystem::path &get_assets() const noexcept { return assets; }
  inline const std::filesystem::path &get_palettes() const noexcept { return palettes; }
  inline const std::filesystem::path &get_projects() const noexcept { return projects; }
  inline const std::filesystem::path &get_levels() const noexcept { return levels; }
  inline const std::filesystem::path &get_data() const noexcept { return data; }
  inline const std::filesystem::path &get_datapacks() const noexcept { return datapacks; }
  inline const std::filesystem::path &get_logs() const noexcept { return logs; }

  inline const std::filesystem::path &get_tilepacks() const noexcept { return tilepacks;}
  inline const std::filesystem::path &get_proppacks() const noexcept { return proppacks;}
  inline const std::filesystem::path &get_materialpacks() const noexcept { return materialpacks;}

  inline const std::filesystem::path &get_shaders() const noexcept { return shaders; }
  inline const std::filesystem::path &get_fonts() const noexcept { return fonts; }
  inline const std::filesystem::path &get_materials() const noexcept { return materials; }
  inline const std::filesystem::path &get_tiles() const noexcept { return tiles; }
  inline const std::filesystem::path &get_props() const noexcept { return props; }
  inline const std::filesystem::path &get_cast() const noexcept { return cast; }

  void set_data(std::filesystem::path, bool include_cast = true);
  void set_levels(std::filesystem::path);

  inline bool is_ok() const noexcept { return ok; } 
  inline bool is_executable_found() const noexcept { return executable_found; }
  
  inline bool is_assets_found() const noexcept { return assets_found; }
  inline bool is_palettes_found() const noexcept { return palettes_found; }
  inline bool is_shaders_found() const noexcept { return shaders_found; }
  inline bool is_fonts_found() const noexcept { return fonts_found; }

  inline bool is_tilepacks_found() const noexcept { return tilepacks_found; }
  inline bool is_proppacks_found() const noexcept { return proppacks_found; }
  inline bool is_materialpacks_found() const noexcept { return materialpacks_found; }
  
  inline bool is_data_found() const noexcept { return data_found; }
  inline bool is_datapacks_found() const noexcept { return datapacks_found; }
  inline bool is_tiles_found() const noexcept { return tiles_found; }
  inline bool is_materials_found() const noexcept { return materials_found; }
  inline bool is_props_found() const noexcept { return props_found; }
  inline bool is_cast_found() const noexcept { return cast_found; }
  
  Dirs();
  Dirs(const std::filesystem::path &);
};

};