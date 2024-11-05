#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <memory>

#include <token.hpp>
#include <node.hpp>

#include <MobitRenderer/definitions.h>

namespace mr {

    std::shared_ptr<TileDef> parse_tiledef(std::unique_ptr<hmp::AST_Node> const&);

};