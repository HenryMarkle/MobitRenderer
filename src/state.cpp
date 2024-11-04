#include "state.h"

#include <memory>
#include <filesystem>

#include "spdlog/spdlog.h"

const std::filesystem::path& mr::dirs::get_executable() const  { return executable; }

const std::filesystem::path& mr::dirs::get_assets() const      { return assets; }
const std::filesystem::path& mr::dirs::get_projects() const    { return projects; }
const std::filesystem::path& mr::dirs::get_levels() const      { return levels; }
const std::filesystem::path& mr::dirs::get_data() const        { return data; }
const std::filesystem::path& mr::dirs::get_logs() const        { return logs; }

const std::filesystem::path& mr::dirs::get_shaders() const     { return shaders; }
const std::filesystem::path& mr::dirs::get_materials() const   { return materials; }
const std::filesystem::path& mr::dirs::get_tiles() const       { return tiles; }
const std::filesystem::path& mr::dirs::get_props() const       { return props; }
const std::filesystem::path& mr::dirs::get_cast() const        { return cast; }

mr::dirs::dirs() {
    #ifdef __linux__
    {
        char result[PATH_MAX];
        ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
        
        if (count == -1) throw "could not retrieve executable's path";
        result[count] = '\0';
        executable = std::filesystem::absolute(result).parent_path();
    }
    #endif

    assets   = executable / "Assets";
    projects = executable / "Projects";
    levels   = executable / "Levels";
    data     = executable / "Data";
    logs     = executable / "Logs";

    if (!exists(assets)) throw std::invalid_argument("assets does not exist");
    if (!exists(data)) throw std::invalid_argument("data does not exist");

    if (!exists(projects)) {
        std::filesystem::create_directory(projects);
    }

    if (!exists(levels)) {
        std::filesystem::create_directory(levels);
    }

    if (!exists(levels)) {
        std::filesystem::create_directory(logs);
    }

    shaders = assets / "Shaders";
    materials = data / "Materials";
    tiles = data / "Graphics";
    props = data / "Props";
    cast = data / "Cast";

    if (!exists(shaders))   throw std::invalid_argument("Assets/Shaders does not exist");
    if (!exists(materials)) throw std::invalid_argument("Data/Materials does not exist");
    if (!exists(tiles))     throw std::invalid_argument("Data/Graphics does not exist");
    if (!exists(props))     throw std::invalid_argument("Data/Props does not exist");
    if (!exists(cast))      throw std::invalid_argument("Data/Cast does not exist");
}

mr::dirs::dirs(const std::filesystem::path& executable_directory) {
    if (!std::filesystem::exists(executable_directory)) throw std::invalid_argument("invalid executable directory");

    assets   = executable_directory / "Assets";
    projects = executable_directory / "Projects";
    levels   = executable_directory / "Levels";
    data     = executable_directory / "Data";
    logs     = executable_directory / "Logs";

    if (!exists(assets)) throw std::invalid_argument("assets does not exist");
    if (!exists(data)) throw std::invalid_argument("data does not exist");

    if (!exists(projects)) {
        std::filesystem::create_directory(projects);
    }

    if (!exists(levels)) {
        std::filesystem::create_directory(levels);
    }

    if (!exists(levels)) {
        std::filesystem::create_directory(logs);
    }

    shaders = assets / "Shaders";
    materials = data / "Materials";
    tiles = data / "Graphics";
    props = data / "Props";
    cast = data / "Cast";

    if (!exists(shaders))   throw std::invalid_argument("Assets/Shaders does not exist");
    if (!exists(materials)) throw std::invalid_argument("Data/Materials does not exist");
    if (!exists(tiles))     throw std::invalid_argument("Data/Graphics does not exist");
    if (!exists(props))     throw std::invalid_argument("Data/Props does not exist");
    if (!exists(cast))      throw std::invalid_argument("Data/Cast does not exist");

    executable = executable_directory;
}

spdlog::logger* mr::context::get_logger() const { return logger.get(); }
void mr::context::set_logger(const std::shared_ptr<spdlog::logger> _logger) { logger = _logger; }

const mr::dirs& mr::context::get_dirs() const { return directories; }
void mr::context::set_dirs(mr::dirs _dirs) { directories = _dirs; }

mr::context::context() : logger(nullptr), directories(mr::dirs()), textures(mr::Textures(directories)) { }
mr::context::~context() { }

mr::Textures::Textures(const dirs &_dirs)
{
}

mr::Textures::~Textures()
{
}
