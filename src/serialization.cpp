#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include <filesystem>
#include <sstream>
#include <vector>

#include <MobitRenderer/parsing.h>
#include <MobitRenderer/serialization.h>
#include <MobitRenderer/exceptions.h>

namespace mr {

    std::shared_ptr<TileDef> parse_tiledef(std::unique_ptr<mr::AST_Node> const& nodes) {
        return nullptr;
    }

    std::unique_ptr<ProjectSaveFileLines> read_project(const std::filesystem::path& file_path) {
        std::fstream file(file_path);

        if (!file.is_open()) {
            std::stringstream ss;
            ss << "could not open project save file '" << file_path << "'"; 
            throw file_read_error();
        }

        auto lines = std::make_unique<ProjectSaveFileLines>();

        std::string line;
        int counter = 0;

        while (std::getline(file, line, '\r')) {
            switch (counter) {
                case 0: lines->geometry         = std::move(line); break;
                case 1: lines->tiles            = std::move(line); break;
                case 2: lines->effects          = std::move(line); break;
                case 3: lines->light_settings   = std::move(line); break;
                case 4: lines->terrain_settings = std::move(line); break;
                case 5: lines->seed_and_buffers = std::move(line); break;
                case 6: lines->cameras_and_size = std::move(line); break;
                case 7: lines->water            = std::move(line); break;
                case 8: lines->props            = std::move(line); break;
            }

            if(++counter > 8) break;;
        }

        file.close();

        return lines;
    }

    std::unique_ptr<ProjectSaveFileNodes> parse_project(const std::unique_ptr<ProjectSaveFileLines>& file_lines) {
        if (file_lines == nullptr) return nullptr;

        auto nodes = std::make_unique<ProjectSaveFileNodes>();
    
        std::vector<token> geo_tokens, tile_tokens;

        tokenize(file_lines->geometry, geo_tokens);
        tokenize(file_lines->tiles, tile_tokens);

        parse_tokens(geo_tokens, nodes->geometry);
        parse_tokens(tile_tokens, nodes->tiles);

        return nodes;
    }
};
