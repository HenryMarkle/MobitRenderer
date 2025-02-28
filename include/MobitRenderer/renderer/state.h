#pragma once

#include <raylib.h>

#include <MobitRenderer/matrix.h>
#include <MobitRenderer/definitions.h>

namespace mr::renderer {

/// @brief An abstract class representing a temporary data used to render a material.
class MaterialRenderState {
    public:
        
        const MaterialDef *def;
    
        MaterialRenderState(const MaterialDef *def) : def(def) {}
        virtual ~MaterialRenderState() = default;
};

class Standard_MRS : public MaterialRenderState {

public:

    RenderTexture2D rt;

    Standard_MRS(const MaterialDef *def, int width, int height) : 
        MaterialRenderState(def),
        rt(LoadRenderTexture(width, height))
    {
        BeginTextureMode(rt);
        ClearBackground(WHITE);
        EndTextureMode();
    }

    ~Standard_MRS() {
        UnloadRenderTexture(rt);
    }

};

class ChaoticStone_MRS : public MaterialRenderState {

public:

    Matrix<bool> used_space;

    ChaoticStone_MRS(const MaterialDef *def, matrix_t width, matrix_t height) : 
        MaterialRenderState(def),
        used_space(width, height, 3)
    {}

};
    

};
