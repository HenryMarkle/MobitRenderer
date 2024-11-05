#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include <filesystem>

#include <raylib.h>

#include <MobitRenderer/definitions.h>
#include <MobitRenderer/matrix.h>
#include <MobitRenderer/parsing.h>

namespace mr {
    struct ProjectSaveFileLines {
        std::string geometry;
        std::string tiles;
        std::string effects;
        std::string light_settings;
        std::string terrain_settings;
        std::string seed_and_buffers;
        std::string cameras_and_size;
        std::string water;
        std::string props;
    };

    struct ProjectSaveFileNodes {
        std::unique_ptr<AST_Node> geometry;
        std::unique_ptr<AST_Node> tiles;
        std::unique_ptr<AST_Node> effects;
        std::unique_ptr<AST_Node> light_settings;
        std::unique_ptr<AST_Node> terrain_settings;
        std::unique_ptr<AST_Node> seed_and_buffers;
        std::unique_ptr<AST_Node> cameras_and_size;
        std::unique_ptr<AST_Node> water;
        std::unique_ptr<AST_Node> props;
    };

    // Save file parsers

    std::unique_ptr<ProjectSaveFileLines> read_project_save_file(const std::filesystem::path&);
    std::unique_ptr<ProjectSaveFileNodes> parse_project_save_lines(const std::unique_ptr<ProjectSaveFileLines>&);

    std::unique_ptr<Matrix<GeoCell>>   parse_geo_matrix(const std::unique_ptr<AST_Node>&);
    std::unique_ptr<Matrix<TileCell>> parse_tile_matrix(const std::unique_ptr<AST_Node>&);
    
    // Init line parsers

    std::shared_ptr<TileDef>              parse_tiledef(const std::unique_ptr<AST_Node>&);

};