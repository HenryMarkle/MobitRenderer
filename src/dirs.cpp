#include <filesystem>

#include <MobitRenderer/dirs.h>
#include <MobitRenderer/io.h>

using std::filesystem::exists;
using std::filesystem::is_directory;
using std::filesystem::create_directory;

namespace mr {

void Dirs::set_data(std::filesystem::path dir, bool include_cast) {
  if (!is_directory(dir))
      throw std::invalid_argument(dir.string()+" is not a directory");
  
  data = dir;
  props = data / "Props";
  tiles = data / "Graphics";
  materials = data / "Materials";
  if (include_cast) cast = data / "Cast";

  data_found = exists(data);
  props_found = exists(props);
  tiles_found = exists(tiles);
  materials_found = exists(materials);
  if (include_cast) cast_found = exists(cast);
}
void Dirs::set_levels(std::filesystem::path dir) {
  if (!is_directory(dir))
      throw std::invalid_argument(dir.string()+" is not a directory");
  
  levels = dir;
  levels_found = exists(levels);
}

Dirs::Dirs() {

  executable = mr::get_executable_dir();

  
  #ifdef IS_DEBUG_BUILD
  const auto src_dir = std::filesystem::path(PROJECT_SRC_DIR);
  data = src_dir / "Data";
  datapacks = src_dir / "DataPacks";
  assets = src_dir / "Assets";
  #else
  data = executable / "Data";
  datapacks = executable / "DataPacks";
  assets = executable / "Assets";
  #endif

  palettes = assets / "Palettes";

  tilepacks = datapacks / "Tiles";
  proppacks = datapacks / "Props";
  materialpacks = datapacks / "Materials";

  projects = executable / "Projects";
  levels = executable / "Levels";
  logs = executable / "Logs";

  if (!exists(projects)) create_directory(projects);
  if (!exists(levels)) create_directory(levels);
  if (!exists(logs)) create_directory(logs);

  #ifdef FEATURE_PALETTES
  if (!exists(palettes)) create_directory(palettes);
  #endif

  #ifdef FEATURE_DATAPACKS
  if (!exists(datapacks)) create_directory(datapacks);
  if (!exists(tilepacks)) create_directory(tilepacks);
  if (!exists(proppacks)) create_directory(proppacks);
  if (!exists(materialpacks)) create_directory(materialpacks);
  #endif

  shaders = assets / "Shaders";
  fonts = assets / "Fonts";
  materials = data / "Materials";
  tiles = data / "Graphics";
  props = data / "Props";
  cast = data / "Cast";

  executable_found = exists(executable);

  assets_found = exists(assets);
  palettes_found = exists(palettes);
  shaders_found = exists(shaders);
  fonts_found = exists(fonts);

  data_found = exists(data);
  tiles_found = exists(tiles);
  materials_found = exists(materials);
  props_found = exists(props);
  cast_found = exists(cast);

  ok = executable_found && 
    assets_found &&
    shaders_found &&
    data_found &&
    tiles_found &&
    materials_found &&
    props_found &&
    cast_found;
}

Dirs::Dirs(const std::filesystem::path &executable_directory) {
  if (!exists(executable_directory))
    throw std::invalid_argument("invalid executable directory");

  assets = executable_directory / "Assets";
  projects = executable_directory / "Projects";
  levels = executable_directory / "Levels";
  data = executable_directory / "Data";
  logs = executable_directory / "Logs";

  if (!exists(assets))
    throw std::invalid_argument("assets does not exist");
  if (!exists(data))
    throw std::invalid_argument("data does not exist");

  if (!exists(projects)) {
    create_directory(projects);
  }

  if (!exists(levels)) {
    create_directory(levels);
  }

  if (!exists(levels)) {
    create_directory(logs);
  }

  shaders = assets / "Shaders";
  fonts = assets / "Fonts";
  materials = data / "Materials";
  tiles = data / "Graphics";
  props = data / "Props";
  cast = data / "Cast";

  if (!exists(shaders))
    throw std::invalid_argument("Assets/Shaders does not exist");
  if (!exists(fonts))
    create_directory(fonts);
  if (!exists(materials))
    throw std::invalid_argument("Data/Materials does not exist");
  if (!exists(tiles))
    throw std::invalid_argument("Data/Graphics does not exist");
  if (!exists(props))
    throw std::invalid_argument("Data/Props does not exist");
  if (!exists(cast))
    throw std::invalid_argument("Data/Cast does not exist");

  executable = executable_directory;
}

};
