#include <map>
#include <vector>
#include <string>
#include <istream>
#include <memory>
#include <exception>
#include <filesystem>

#include <MobitParser/tokens.h>
#include <MobitParser/nodes.h>

#include <MobitRenderer/dex.h>
#include <MobitRenderer/definitions.h>

using std::filesystem::path;
using std::filesystem::exists;

namespace mr {

const std::map<std::string, std::shared_ptr<TileDef>> &TileDex::tiles() const noexcept { return _tiles; }
const std::vector<std::string> &TileDex::categories() const noexcept { return _categories; }
const std::vector<std::vector<std::shared_ptr<TileDef>>> &TileDex::sorted_tiles() const noexcept { return _sorted_tiles; }
const std::map<std::string, std::vector<std::shared_ptr<TileDef>>> &TileDex::category_tiles() const noexcept { return _category_tiles; }
const std::map<std::string, Color> &TileDex::colors() const noexcept { return _category_colors; }

void TileDex::register_from_dir(path const&file) {
    if (!exists(file)) return;

    std::ifstream init(file);
    if (!init.is_open()) {
        std::string msg("failed to open tile init file '");
        msg.append(file.string());
        msg.append("'");

        throw std::runtime_error(msg.c_str());
    }

    std::string line;
    while (std::getline(init, line)) {
        
        // category
        if (line.at(0) == '-') {
            std::string without_dash;
            for (auto c = line.begin() + 1; c != line.end(); c++) {
                without_dash.push_back(*c);
            }

            auto tokens = mp::tokenize(without_dash);
            auto category_node = mp::parse(tokens);

            // TODO: continue here
        }
        // tile
        else {

        }
    }

    init.close();
}

void TileDex::unload_textures() {
    for (auto pair : _tiles) pair.second->unload_texture();
}

TileDex &TileDex::operator=(TileDex &&other) noexcept {
    if (this == &other)
        return *this;

    _tiles = std::move(other._tiles);
    _category_tiles = std::move(other._category_tiles);
    _category_colors = std::move(other._category_colors);

    return *this;
}

TileDex::TileDex() : _tiles({}), _category_tiles({}), _category_colors({}) {}
TileDex::TileDex(TileDex &&other) noexcept {
    _tiles = std::move(other._tiles);
    _category_tiles = std::move(other._category_tiles);
    _category_colors = std::move(other._category_colors);
}
TileDex::~TileDex() {}

};