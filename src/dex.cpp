#include <unordered_map>
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
#include <MobitRenderer/castlibs.h>
#include <MobitRenderer/exceptions.h>
#include <MobitRenderer/definitions.h>
#include <MobitRenderer/serialization.h>

using std::filesystem::path;
using std::filesystem::exists;

namespace mr {

TileDef *TileDex::tile(const std::string &name) const noexcept {
    auto tile_ptr = _tiles.find(name);
    if (tile_ptr == _tiles.end()) return nullptr;
    return tile_ptr->second;
}
const std::unordered_map<std::string, TileDef*> &TileDex::tiles() const noexcept { return _tiles; }
const std::vector<TileDefCategory> &TileDex::categories() const noexcept { return _categories; }
const std::vector<std::vector<TileDef*>> &TileDex::sorted_tiles() const noexcept { return _sorted_tiles; }
const std::unordered_map<std::string, std::vector<TileDef*>> &TileDex::category_tiles() const noexcept { return _category_tiles; }

void TileDex::register_from(path const&file, CastLibs const*libs) {
    if (!exists(file)) throw dex_error(std::string("file does not exist: "+file.string()));

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
                auto category = mr::serde::deser_tiledef_category(category_node.get());
            
                _categories.push_back(category);
                _category_tiles[category.name] = std::vector<TileDef*>();
                _sorted_tiles.push_back(std::vector<TileDef*>());

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
                auto tiledef = mr::serde::deser_tiledef(def_node.get());

                if (_tiles.find(tiledef->get_name()) != _tiles.end()) {

                    #ifdef IS_DEBUG_BUILD
                    std::cout << "Warning: skipped duplicate tile definition \"" << tiledef->get_name() << '"' << std::endl;
                    #endif
                    
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

                auto &tags = tiledef->get_tags();

                if (tags.find("INTERNAL") != tags.end()) {
                    if (libs == nullptr) 
                        throw dex_error(
                            std::string("tile '"+tiledef->get_name()+"' resource is internal but CastLibs* argument was nullptr")
                        );
                
                    try {
                        auto *member = libs->member_or_throw(tiledef->get_name());
                        tiledef->set_texture_path(member->get_texture_path());
                    } catch (std::runtime_error &e) {
                        throw dex_error(
                            std::string("tile '"+tiledef->get_name()+"' internal resouce was not found")
                        );
                    }
                } 
                else {
                    tiledef->set_texture_path(init_dir / (tiledef->get_name() + ".png"));
                }

                tiledef->set_category(current_category.name);
                tiledef->set_color(current_category.color);

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

void TileDex::unload_all() {
    for (auto def : _tiles) delete def.second;

    _tiles.clear();
    _categories.clear();
    _sorted_tiles.clear();
    _category_tiles.clear();
}


TileDex::TileDex() : _tiles({}), _category_tiles({}) {}

TileDex::~TileDex() {
    unload_all();
}

// Material dex

void MaterialDex::unload_textures() {
    for (auto &pair : _materials) {
        MaterialDef *m = pair.second;
        if (m->get_type() != MaterialRenderType::custom_unified) continue;

        CustomMaterialDef *cm = dynamic_cast<CustomMaterialDef *>(m);
   
        if (cm != nullptr) {
            cm->unload_textures();
        }
    }
}

void MaterialDex::load_internals() {
    MaterialDef* materials[] = {
        new MaterialDef("Standard",        "Materials", Color{ 150, 150, 150, 255 }, MaterialRenderType::unified),
        new MaterialDef("Concrete",        "Materials", Color{ 150, 255, 255, 255 }, MaterialRenderType::unified),
        new MaterialDef("RainStone",       "Materials", Color{   0,   0, 255, 255 }, MaterialRenderType::unified),
        new MaterialDef("Bricks",          "Materials", Color{ 200, 150, 100, 255 }, MaterialRenderType::unified),
        new MaterialDef("BigMetal",        "Materials", Color{ 255,   0,   0, 255 }, MaterialRenderType::unified),
        new MaterialDef("Scaffolding",     "Materials", Color{ 60,  60,  40, 255 }, MaterialRenderType::unified),
        new MaterialDef("Dense Pipes",     "Materials", Color{  10,  10, 255, 255 }, MaterialRenderType::dense_pipe),
        new MaterialDef("SuperStructure",  "Materials", Color{ 160, 180, 255, 255 }, MaterialRenderType::unified),
        new MaterialDef("SuperStructure2", "Materials", Color{ 190, 160,   0, 255 }, MaterialRenderType::unified),
        new MaterialDef("Tiled Stone",     "Materials", Color{ 100,   0, 255, 255 }, MaterialRenderType::tiles),
        new MaterialDef("Chaotic Stone",   "Materials", Color{ 255,   0, 255, 255 }, MaterialRenderType::tiles),
        new MaterialDef("Small Pipes",     "Materials", Color{ 255, 255,   0, 255 }, MaterialRenderType::pipe),
        new MaterialDef("Trash",           "Materials", Color{  90, 255,   0, 255 }, MaterialRenderType::pipe),
        new MaterialDef("Invisible",       "Materials", Color{ 200, 200, 200, 255 }, MaterialRenderType::invisible),
        new MaterialDef("LargeTrash",      "Materials", Color{ 150,  30, 255, 255 }, MaterialRenderType::large_trash),
        new MaterialDef("3DBricks",        "Materials", Color{ 255, 150,   0, 255 }, MaterialRenderType::tiles),
        new MaterialDef("Random Machines", "Materials", Color{  72, 116,  80, 255 }, MaterialRenderType::tiles),
        new MaterialDef("Dirt",            "Materials", Color{ 124,  72,  52, 255 }, MaterialRenderType::dirt),
        new MaterialDef("Ceramic Tile",    "Materials", Color{  60,  60, 100, 255 }, MaterialRenderType::ceramic),
        new MaterialDef("Temple Stone",    "Materials", Color{   0, 120, 180, 255 }, MaterialRenderType::tiles),
        new MaterialDef("Circuits",        "Materials", Color{  15, 200,  15, 255 }, MaterialRenderType::dense_pipe),
        new MaterialDef("Ridge",           "Materials", Color{ 200,  15,  60, 255 }, MaterialRenderType::ridge),
    };

    MaterialDef *drought_materials[] = {
        new MaterialDef( "Steel",                "Drought Materials", Color{220, 170, 195, 255}, MaterialRenderType::unified),
        new MaterialDef( "4Mosaic",              "Drought Materials", Color{227,  76,  13, 255}, MaterialRenderType::tiles),
        new MaterialDef( "Color A Ceramic",      "Drought Materials", Color{120,  0,   90, 255}, MaterialRenderType::ceramic_a),
        new MaterialDef( "Color B Ceramic",      "Drought Materials", Color{  0, 175, 175, 255}, MaterialRenderType::ceramic_b),
        new MaterialDef( "Random Pipes",         "Drought Materials", Color{ 80,   0, 140, 255}, MaterialRenderType::random_pipes),
        new MaterialDef( "Rocks",                "Drought Materials", Color{185, 200,   0, 255}, MaterialRenderType::rock),
        new MaterialDef( "Rough Rock",           "Drought Materials", Color{155, 170,   0, 255}, MaterialRenderType::rough_rock),
        new MaterialDef( "Random Metal",         "Drought Materials", Color{180,  10,  10, 255}, MaterialRenderType::tiles),
        new MaterialDef( "Cliff",                "Drought Materials", Color{ 75,  75,  75, 255}, MaterialRenderType::tiles),
        new MaterialDef( "Non-Slip Metal",       "Drought Materials", Color{180,  80,  80, 255}, MaterialRenderType::unified),
        new MaterialDef( "Stained Glass",        "Drought Materials", Color{180,  80, 180, 255}, MaterialRenderType::unified),
        new MaterialDef( "Sandy Dirt",           "Drought Materials", Color{180, 180,  80, 255}, MaterialRenderType::sandy),
        new MaterialDef( "MegaTrash",            "Drought Materials", Color{135,  10, 255, 255}, MaterialRenderType::unified),
        new MaterialDef( "Shallow Dense Pipes",  "Drought Materials", Color{ 13,  23, 110, 255}, MaterialRenderType::dense_pipe),
        new MaterialDef( "Sheet Metal",          "Drought Materials", Color{145, 135, 125, 255}, MaterialRenderType::wv),
        new MaterialDef( "Chaotic Stone 2",      "Drought Materials", Color{ 90,  90,  90, 255}, MaterialRenderType::tiles),
        new MaterialDef( "Asphalt",              "Drought Materials", Color{115, 115, 115, 255}, MaterialRenderType::unified),
    };

    MaterialDef* community_materials[] = {
        new MaterialDef("Shallow Circuits",     "Community Materials", Color{ 15, 200, 155, 255}, MaterialRenderType::dense_pipe),
        new MaterialDef("Random Machines 2",    "Community Materials", Color{116, 116,  80, 255}, MaterialRenderType::tiles),
        new MaterialDef("Small Machines",       "Community Materials", Color{ 80, 116, 116, 255}, MaterialRenderType::tiles),
        new MaterialDef("Random Metals",        "Community Materials", Color{255,   0,  80, 255}, MaterialRenderType::tiles),
        new MaterialDef("ElectricMetal",        "Community Materials", Color{255,   0, 100, 255}, MaterialRenderType::unified),
        new MaterialDef("Grate",                "Community Materials", Color{190,  50, 190, 255}, MaterialRenderType::unified),
        new MaterialDef("CageGrate",            "Community Materials", Color{ 50, 190, 190, 255}, MaterialRenderType::unified),
        new MaterialDef("BulkMetal",            "Community Materials", Color{ 50,  19, 190, 255}, MaterialRenderType::unified),
        new MaterialDef("MassiveBulkMetal",     "Community Materials", Color{255,  19,  19, 255}, MaterialRenderType::unified),
        new MaterialDef("Dune Sand",            "Community Materials", Color{255, 255, 100, 255}, MaterialRenderType::tiles),
    };

    _categories.reserve(3);
    _sorted_materials.reserve(3);

    _categories.push_back("Materials");
    std::vector<MaterialDef*> mats_vec;
    mats_vec.reserve(22);
    for (auto *def : materials) {
        _materials[def->get_name()] = def;
        mats_vec.push_back(def);
    }
    _sorted_materials.push_back(mats_vec);
    _category_materials["Materials"] = mats_vec;

    _categories.push_back("Drought Materials");
    std::vector<MaterialDef*> drought_vec;
    drought_vec.reserve(17);
    for (auto *def : drought_materials) {
        _materials[def->get_name()] = def;
        drought_vec.push_back(def);
    }
    _sorted_materials.push_back(drought_vec);
    _category_materials["Drought Materials"] = drought_vec;

    _categories.push_back("Community Materials");
    std::vector<MaterialDef*> community_vec;
    community_vec.reserve(10);
    for (auto *def : community_materials) {
        _materials[def->get_name()] = def;
        community_vec.push_back(def);
    }
    _sorted_materials.push_back(community_vec);
    _category_materials["Community Materials"] = community_vec;
}

MaterialDex::~MaterialDex() {
    for (auto &pair : _materials) delete pair.second;
}

MaterialDex::MaterialDex() {}

PropDef *PropDex::prop(const std::string &name) const noexcept {
    auto prop_iter = _props.find(name);
    if (prop_iter == _props.end()) return nullptr;
    return prop_iter->second;
}

const std::unordered_map<std::string, PropDef*> &PropDex::props() const noexcept {
    return _props;
}

const std::vector<PropDefCategory> &PropDex::categories() const noexcept {
    return _categories;
}

const std::vector<std::vector<PropDef*>> &PropDex::sorted_props() const noexcept {
    return _sorted_props;
}

const std::unordered_map<std::string, std::vector<PropDef*>> &PropDex::category_props() const noexcept {
    return _category_props;
}

void PropDex::register_from(std::filesystem::path const &file, CastLibs const *libs) {
    if (!exists(file)) throw dex_error(std::string("file does not exist: "+file.string()));

    path init_dir = file.parent_path();

    std::ifstream init(file);
    if (!init.is_open()) {
        std::string msg("failed to open tile init file '");
        msg.append(file.string());
        msg.append("'");

        throw std::runtime_error(msg.c_str());
    }

    int counter = 0;
    PropDefCategory current_category = {"", Color{0}};
    bool category_parsed = false;


    while (init.peek() && init.peek() != EOF) {
        counter++;

        while (init.peek() == '\n') { counter++; init.get();}

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
                auto category = mr::serde::deser_propdef_category(category_node.get());
            
                _categories.push_back(category);
                _category_props[category.name] = std::vector<PropDef*>();
                _sorted_props.push_back(std::vector<PropDef*>());

                current_category = category;
                category_parsed = true;
            } 
            catch (mp::parse_failure &pe) {
                std::ostringstream msg;
                msg 
                    << "failed to parse prop init category at line "
                    << counter
                    << ": "
                    << pe.what();
                
                init.close();

                throw std::runtime_error(msg.str());
            } 
            catch (deserialization_failure &e) {
                std::ostringstream msg;
                msg 
                    << "failed to deserialize prop init category at line "
                    << counter
                    << ": "
                    << e.what();
                
                init.close();

                throw std::runtime_error(msg.str());
            }
        }
        // prop
        else {
            if (!category_parsed) {
                init.close();
                throw std::runtime_error("failed to parse prop init: must begin with a category");
            }
        
            std::vector<mp::token> tokens;
            if (!mp::tokenize_line(init, tokens)) break;

            try {
                auto def_node = mp::parse(tokens);
                auto propdef = mr::serde::deser_propdef(def_node.get());

                if (_props.find(propdef->name) != _props.end()) {
                    
                    #ifdef IS_DEBUG_BUILD
                    std::cout << "Warning: skipped duplicate prop definition \"" << propdef->name << '"' << std::endl;
                    #endif

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

                auto &tags = propdef->tags;

                if (tags.find("INTERNAL") != tags.end()) {
                    if (libs == nullptr) 
                        throw dex_error(
                            std::string("prop '"+propdef->name+"' resource is internal but CastLibs* argument was nullptr")
                        );
                
                    try {
                        auto *member = libs->member_or_throw(propdef->name);
                        propdef->set_texture_path(member->get_texture_path());
                    } catch (std::runtime_error &e) {
                        throw dex_error(
                            std::string("prop '"+propdef->name+"' internal resouce was not found")
                        );
                    }
                } 
                else {
                    propdef->set_texture_path(init_dir / (propdef->name + ".png"));
                }

                propdef->set_category(current_category.name);
                propdef->set_color(current_category.color);

                _props[propdef->name] = propdef;
                _sorted_props[_categories.size() - 1].push_back(propdef);
                _category_props[current_category.name].push_back(propdef);
            }
            catch (mp::parse_failure &pe) {
                std::ostringstream msg;
                msg 
                    << "failed to parse prop init at line "
                    << counter
                    << ": "
                    << pe.what();
                
                init.close();

                throw deserialization_failure(msg.str());
            }
            catch (deserialization_failure &e) {
                std::ostringstream msg;
                msg 
                    << "failed to deserialize prop init at line "
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

void PropDex::register_tiles(const TileDex *dex) {
    for (size_t c = 0; c < dex->categories().size(); c++) {
        const auto &category = dex->categories()[c];
        const auto &tiles = dex->sorted_tiles()[c];

        std::vector<TileDef*> tiles_as_props;

        for (size_t t = 0; t < tiles.size(); t++) {
            auto *tile = tiles[t];

            const auto prop_iter = _props.find(tile->get_name());
            if (prop_iter != _props.end()) 
            {
                #ifdef IS_DEBUG_BUILD
                std::cout 
                    << "Warning: skipped duplicate tile-as-prop definition \"" 
                    << tile->get_name() 
                    << "\" (conflicting with a prop definition)" 
                << std::endl;
                #endif
                // throw dex_error(std::string("duplicate tile/prop definition identifier \""+tile->get_name()+"\""));
            }

            const auto &tags = tile->get_tags();

            auto not_trash_flag = tags.find("notTrashProp");
            auto not_prop_flag = tags.find("notProp");

            if (not_prop_flag == tags.end() && not_prop_flag == tags.end()) {
                if (_tiles.find(tile->get_name()) != _tiles.end())
                {
                    #ifdef IS_DEBUG_BUILD
                    std::cout << "Warning: skipped duplicate tile-as-prop definition \"" << tile->get_name() << '"' << std::endl;
                    #endif
                    // throw dex_error("duplicate tile definition identifier \""+tile->get_name()+"\"");
                }

                _tiles[tile->get_name()] = tile;
                tiles_as_props.push_back(tile);
            }
        }

        if (!tiles_as_props.empty()) {
            _tile_categories.push_back(category);
            _category_tiles[category.name] = tiles_as_props;
            _sorted_tiles.push_back(tiles_as_props);
        }
    }

    
}

void PropDex::unload_textures() {
    for (auto &pair : _props) pair.second->unload_texture();
}

void PropDex::unload_all() {
    for (auto &pair : _props) delete pair.second;

    _props.clear();
    _categories.clear();
    _sorted_props.clear();
    _category_props.clear();
    _tiles.clear();
    _tile_categories.clear();
    _sorted_tiles.clear();
    _category_tiles.clear();
}

PropDex::PropDex() {}
PropDex::~PropDex() {
    unload_all();
}

};