#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include <filesystem>

#include <raylib.h>

#include <token.hpp>
#include <node.hpp>

#include <MobitRenderer/definitions.h>
#include <MobitRenderer/matrix.h>

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
        std::unique_ptr<hmp::AST_Node> geometry;
        std::unique_ptr<hmp::AST_Node> tiles;
        std::unique_ptr<hmp::AST_Node> effects;
        std::unique_ptr<hmp::AST_Node> light_settings;
        std::unique_ptr<hmp::AST_Node> terrain_settings;
        std::unique_ptr<hmp::AST_Node> seed_and_buffers;
        std::unique_ptr<hmp::AST_Node> cameras_and_size;
        std::unique_ptr<hmp::AST_Node> water;
        std::unique_ptr<hmp::AST_Node> props;
    };

    // Save file parsers

    std::unique_ptr<ProjectSaveFileLines> read_project_save_file(const std::filesystem::path&);
    std::unique_ptr<ProjectSaveFileNodes> parse_project_save_lines(const std::unique_ptr<ProjectSaveFileLines>&);

    std::unique_ptr<Matrix<GeoCell>>   parse_geo_matrix(const std::unique_ptr<hmp::AST_Node>&);
    std::unique_ptr<Matrix<TileCell>> parse_tile_matrix(const std::unique_ptr<hmp::AST_Node>&);
    
    // Init line parsers

    std::shared_ptr<TileDef>              parse_tiledef(const std::unique_ptr<hmp::AST_Node>&);

};