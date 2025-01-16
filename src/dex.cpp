#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <istream>
#include <memory>
#include <exception>
#include <filesystem>
#include <iostream>

#include <MobitParser/exceptions.h>
#include <MobitParser/tokens.h>
#include <MobitParser/nodes.h>

#include <MobitRenderer/dex.h>
#include <MobitRenderer/exceptions.h>
#include <MobitRenderer/definitions.h>
#include <MobitRenderer/serialization.h>

using std::filesystem::path;
using std::filesystem::exists;

namespace mr {

const TileDef *TileDex::tile(const std::string &name) const noexcept {
    auto tile_ptr = _tiles.find(name);
    if (tile_ptr == _tiles.end()) return nullptr;
    return tile_ptr->second.get();
}
const std::map<std::string, std::shared_ptr<TileDef>> &TileDex::tiles() const noexcept { return _tiles; }
const std::vector<TileDefCategory> &TileDex::categories() const noexcept { return _categories; }
const std::vector<std::vector<std::shared_ptr<TileDef>>> &TileDex::sorted_tiles() const noexcept { return _sorted_tiles; }
const std::map<std::string, std::vector<std::shared_ptr<TileDef>>> &TileDex::category_tiles() const noexcept { return _category_tiles; }
const std::map<std::string, Color> &TileDex::colors() const noexcept { return _category_colors; }

void TileDex::register_from(path const&file) {
    if (!exists(file)) return;

    path init_dir = file.parent_path();

    std::ifstream init(file);
    if (!init.is_open()) {
        std::string msg("failed to open tile init file '");
        msg.append(file.string());
        msg.append("'");

        throw std::runtime_error(msg.c_str());
    }

    int counter = 0;
    TileDefCategory current_category = {"", Color{0}};
    bool category_parsed = false;

    while (init.peek() && init.peek() != EOF) {
        counter++;

        while (init.peek() == '\n') init.get();

        // category
        if (init.peek() == '-') {
            init.get();

            // Skip comments
            if (init.peek() == '-') {
                while (init.get() != '\n') {}
                continue;
            }

            try {
                auto tokens = mp::tokenize_line(init);
                auto category_node = mp::parse(tokens);
                auto category = mr::deser_tiledef_category(category_node.get());
            
                _categories.push_back(category);
                _category_colors[category.name] = category.color;
                _category_tiles[category.name] = std::vector<std::shared_ptr<TileDef>>();
                _sorted_tiles.push_back(std::vector<std::shared_ptr<TileDef>>());

                current_category = category;
                category_parsed = true;
            } 
            catch (mp::parse_failure &pe) {
                std::ostringstream msg;
                msg 
                    << "failed to parse tile init category at line "
                    << counter
                    << ": "
                    << pe.what();
                
                init.close();

                throw std::runtime_error(msg.str());
            } 
            catch (deserialization_failure &e) {
                std::ostringstream msg;
                msg 
                    << "failed to deserialize tile init category at line "
                    << counter
                    << ": "
                    << e.what();
                
                init.close();

                throw std::runtime_error(msg.str());
            }
        }
        // tile
        else {
            if (!category_parsed) {
                init.close();
                throw std::runtime_error("failed to parse tile init: must begin with a category");
            }
        
            std::vector<mp::token> tokens;
            if (!mp::tokenize_line(init, tokens)) break;

            try {
                auto def_node = mp::parse(tokens);
                auto tiledef = deser_tiledef(def_node.get());

                if (_tiles.find(tiledef->get_name()) != _tiles.end()) {
                    continue;

                    // init.close();
                    // std::ostringstream msg;
                    // msg 
                    //     << "duplicate tile definition at line " 
                    //     << counter 
                    //     << " '" 
                    //     << tiledef->get_name() 
                    //     << "'";
                    // throw dex_error(msg.str());
                }

                tiledef->set_texture_path(init_dir / (tiledef->get_name() + ".png"));
                tiledef->set_category(current_category.name);

                _tiles[tiledef->get_name()] = tiledef;
                _sorted_tiles[_categories.size() - 1].push_back(tiledef);
                _category_tiles[current_category.name].push_back(tiledef);
            }
            catch (mp::parse_failure &pe) {
                std::ostringstream msg;
                msg 
                    << "failed to parse tile init at line "
                    << counter
                    << ": "
                    << pe.what();
                
                init.close();

                throw deserialization_failure(msg.str());
            }
            catch (deserialization_failure &e) {
                std::ostringstream msg;
                msg 
                    << "failed to deserialize tile init at line "
                    << counter
                    << ": "
                    << e.what();
                
                init.close();

                throw std::runtime_error(msg.str());
            }
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
TileDex::~TileDex() {
    unload_textures();
}

};