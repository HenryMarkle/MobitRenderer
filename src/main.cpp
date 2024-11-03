#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <filesystem>

#include "raylib.h"
#include "imgui.h"
#include "rlImGui.h"

#include "matrix.h"
#include "definitions.h"

using std::string;
using std::filesystem::path;
using std::filesystem::exists;

class Dirs {
    private:
    path assets, projects, levels, data; 
    
    // Subdirectories
    path shaders, materials, tiles, props, cast;

    public:

    const path& get_assets() const      { return assets; }
    const path& get_projects() const    { return projects; }
    const path& get_levels() const      { return levels; }
    const path& get_data() const        { return data; }

    const path& get_shaders() const     { return shaders; }
    const path& get_materials() const   { return materials; }
    const path& get_tiles() const       { return tiles; }
    const path& get_props() const       { return props; }
    const path& get_cast() const        { return cast; }

    Dirs() {
        assets   = std::filesystem::relative("Assets");
        projects = std::filesystem::relative("Projects");
        levels   = std::filesystem::relative("Levels");
        data     = std::filesystem::relative("Data");

        if (!exists(assets)) throw std::invalid_argument("assets does not exist");
        if (!exists(data)) throw std::invalid_argument("data does not exist");

        if (!exists(projects)) {
            std::filesystem::create_directory(projects);
        }

        if (!exists(levels)) {
            std::filesystem::create_directory(levels);
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
};

int main() {
    // std::cout << "Size is: " << sizeof(mr::TileDef) << " bytes" << std::endl;

    // Dirs dirs{};

    SetTargetFPS(40);

    InitWindow(1200, 800, "Renderer");
    
    SetWindowMinSize(1200, 800);
    SetWindowState(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);

    rlImGuiSetup(true);

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; 

    io.ConfigDockingWithShift = true;

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(DARKGRAY);

        DrawText("Hello", 0, 0, 30, WHITE);

        rlImGuiBegin();

        ImGui::DockSpaceOverViewport(ImGui::GetWindowDockID(), ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

        ImGui::Begin("Test");
        ImGui::Text("Hello");
        ImGui::End();

        rlImGuiEnd();

        EndDrawing();
    }

    rlImGuiShutdown();

    CloseWindow();

    return 0;
}