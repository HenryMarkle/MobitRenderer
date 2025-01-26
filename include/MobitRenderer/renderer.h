#pragma once

#include <vector>

#include <raylib.h>
#include <raymath.h>

#include <MobitRenderer/level.h>
#include <MobitRenderer/state.h>
#include <MobitRenderer/matrix.h>
#include <MobitRenderer/definitions.h>

namespace mr::renderer {

struct RenderConfig {

    /// @brief Render level without light.
    bool no_light;
    
    /// @brief Render level without props.
    bool no_props;

    /// @brief Render level without effects.
    bool no_effects;

    /// @brief Render level without tiles.
    bool no_tiles;

    /// @brief Skips any undefined material without 
    /// throwing an exception.
    bool skip_undefined_materials;

    /// @brief Skips any undefined effect 
    /// without throwing an exception.
    bool skip_undefined_effects;

    /// @brief Skips any undefined prop 
    /// without throwing an exception.
    bool skip_undefined_props;
    
    /// @brief Skips any undefined tile 
    /// without throwing an exception.
    bool skip_undefined_tiles;
    
    /// @brief Skips any undefined reference 
    /// (tile, prop, etc..) without throwing an exception.
    bool skip_undefined;

    /// @brief An array of selected cameras (indices) to render.
    std::vector<size_t> cameras;

};

class Renderer {

private:
protected:
public:

};

};
