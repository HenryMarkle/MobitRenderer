#include <iostream>

#include <raylib.h>
#include <imgui.h>

#include <MobitRenderer/pages.h>

namespace mr::pages {

void Settings_Page::process() noexcept {}
void Settings_Page::draw() noexcept {
    ClearBackground(DARKGRAY);
}
void Settings_Page::windows() noexcept {
    if (
        ImGui::Begin("Settings##SettingsPageMainWindow", nullptr, 
        ImGuiWindowFlags_NoCollapse | 
        ImGuiWindowFlags_NoMove | 
        ImGuiWindowFlags_NoResize | 
        ImGuiWindowFlags_NoTitleBar)
    ) {
        ImGui::SetWindowPos({0, 18});
        ImGui::SetWindowSize(ImVec2{static_cast<float>(GetScreenWidth()), GetScreenHeight() - 18.0f});

        auto config = ctx->get_config();

        if (ImGui::BeginTable("layout", 2, ImGuiTableFlags_Resizable)) {
            ImGui::TableSetupColumn("CategoriesColumn", ImGuiTableColumnFlags_WidthFixed, 200);

            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);

            if (ImGui::BeginListBox("##Categories", ImGui::GetContentRegionAvail())) {
                
                if (ImGui::Selectable("General", category_index == 0)) category_index = 0;
                if (ImGui::Selectable("Geometry Editor", category_index == 2)) category_index = 2;
                if (ImGui::Selectable("Tiles Editor", category_index == 3)) category_index = 3;
                if (ImGui::Selectable("Cameras Editor", category_index == 4)) category_index = 4;
                if (ImGui::Selectable("light", category_index == 5)) category_index = 5;
                if (ImGui::Selectable("Effects", category_index == 6)) category_index = 6;
                if (ImGui::Selectable("Props", category_index == 7)) category_index = 7;
                if (ImGui::Selectable("Resources", category_index == 8)) category_index = 8;
                if (ImGui::Selectable("Misc", category_index == 1)) category_index = 1;
                
                ImGui::EndListBox();
            }

            ImGui::TableSetColumnIndex(1);

            switch (category_index) {
                case 1: 
                {
                    ImGui::Checkbox("Splash Screen", &config->splashscreen);
                    ImGui::Checkbox("F3 Menu", &config->f3);
                    ImGui::Checkbox("Crash on Esc", &config->crash_on_esc);
                    ImGui::Checkbox("Blue Screen of Death", &config->blue_screen_of_death);
                }
                break;

                case 2:
                {
                    if (ImGui::BeginTable("##GeometrySprites", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
                        
                        ImGui::TableSetupColumn("Sprite");
                        ImGui::TableSetupColumn("Inherit From General");
                        ImGui::TableSetupColumn("Visible");
                        ImGui::TableSetupColumn("Opacity");
                        ImGui::TableHeadersRow();

                        ImGui::TableNextRow();

                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("Tiles");

                        ImGui::TableSetColumnIndex(1);
                        ImGui::Checkbox("##11", &config->geometry.tiles.inherit);
                        
                        ImGui::TableSetColumnIndex(2);
                        ImGui::Checkbox("##12", &config->geometry.tiles.visible);

                        ImGui::TableSetColumnIndex(3);
                        ImGui::InputInt("##13", &config->geometry.tiles.opacity);

                        ImGui::TableNextRow();
                        
                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("Props");

                        ImGui::TableSetColumnIndex(1);
                        ImGui::Checkbox("##14", &config->geometry.props.inherit);

                        ImGui::TableSetColumnIndex(2);
                        ImGui::Checkbox("##15", &config->geometry.props.visible);

                        ImGui::TableSetColumnIndex(3);
                        ImGui::InputInt("##16", &config->geometry.props.opacity);

                        ImGui::TableNextRow();
                        
                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("Materials");

                        ImGui::TableSetColumnIndex(1);
                        ImGui::Checkbox("##17", &config->geometry.materials.inherit);

                        ImGui::TableSetColumnIndex(2);
                        ImGui::Checkbox("##18", &config->geometry.materials.visible);

                        ImGui::TableSetColumnIndex(3);
                        ImGui::InputInt("##19", &config->geometry.materials.opacity);

                        ImGui::TableNextRow();
                        
                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("Water");

                        ImGui::TableSetColumnIndex(1);
                        ImGui::Checkbox("##20", &config->geometry.water.inherit);

                        ImGui::TableSetColumnIndex(2);
                        ImGui::Checkbox("##21", &config->geometry.water.visible);

                        ImGui::TableSetColumnIndex(3);
                        ImGui::InputInt("##22", &config->geometry.water.opacity);

                        ImGui::TableNextRow();
                        
                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("Grid");

                        ImGui::TableSetColumnIndex(1);
                        ImGui::Checkbox("##23", &config->geometry.grid.inherit);

                        ImGui::TableSetColumnIndex(2);
                        ImGui::Checkbox("##24", &config->geometry.grid.visible);

                        ImGui::TableSetColumnIndex(3);
                        ImGui::InputInt("##25", &config->geometry.grid.opacity);

                        ImGui::EndTable();
                    }
                }
                break;

                case 3:
                {
                    if (ImGui::BeginTable("##TilesSprites", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
                        
                        ImGui::TableSetupColumn("Sprite");
                        ImGui::TableSetupColumn("Inherit From General");
                        ImGui::TableSetupColumn("Visible");
                        ImGui::TableSetupColumn("Opacity");
                        ImGui::TableHeadersRow();

                        ImGui::TableNextRow();

                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("Tiles");

                        ImGui::TableSetColumnIndex(1);
                        ImGui::Checkbox("##26", &config->tiles.tiles.inherit);
                        
                        ImGui::TableSetColumnIndex(2);
                        ImGui::Checkbox("##27", &config->tiles.tiles.visible);

                        ImGui::TableSetColumnIndex(3);
                        ImGui::InputInt("##28", &config->tiles.tiles.opacity);

                        ImGui::TableNextRow();
                        
                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("Props");

                        ImGui::TableSetColumnIndex(1);
                        ImGui::Checkbox("##29", &config->tiles.props.inherit);

                        ImGui::TableSetColumnIndex(2);
                        ImGui::Checkbox("##30", &config->tiles.props.visible);

                        ImGui::TableSetColumnIndex(3);
                        ImGui::InputInt("##31", &config->tiles.props.opacity);

                        ImGui::TableNextRow();
                        
                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("Materials");

                        ImGui::TableSetColumnIndex(1);
                        ImGui::Checkbox("##32", &config->tiles.materials.inherit);

                        ImGui::TableSetColumnIndex(2);
                        ImGui::Checkbox("##33", &config->tiles.materials.visible);

                        ImGui::TableSetColumnIndex(3);
                        ImGui::InputInt("##34", &config->tiles.materials.opacity);

                        ImGui::TableNextRow();
                        
                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("Water");

                        ImGui::TableSetColumnIndex(1);
                        ImGui::Checkbox("##35", &config->tiles.water.inherit);

                        ImGui::TableSetColumnIndex(2);
                        ImGui::Checkbox("##36", &config->tiles.water.visible);

                        ImGui::TableSetColumnIndex(3);
                        ImGui::InputInt("##37", &config->tiles.water.opacity);

                        ImGui::TableNextRow();
                        
                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("Grid");

                        ImGui::TableSetColumnIndex(1);
                        ImGui::Checkbox("##38", &config->tiles.grid.inherit);

                        ImGui::TableSetColumnIndex(2);
                        ImGui::Checkbox("##39", &config->tiles.grid.visible);

                        ImGui::TableSetColumnIndex(3);
                        ImGui::InputInt("##40", &config->tiles.grid.opacity);

                        ImGui::EndTable();
                    }
                }
                break;
                
                case 8:
                {
                    ImGui::SeparatorText("Graphics");

                    if (ImGui::BeginTable("##Graphics", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
                        
                        ImGui::TableSetupColumn("Resource");
                        ImGui::TableSetupColumn("");
                        ImGui::TableSetupColumn("");
                        ImGui::TableHeadersRow();

                        ImGui::TableNextRow();

                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("Textures");

                        ImGui::TableSetColumnIndex(1);
                        if (ImGui::Button("Reload##1")) ctx->_textures->reload_all_textures();

                        ImGui::TableSetColumnIndex(2);
                        // if (ImGui::Button("Unload"));

                        ImGui::TableNextRow();

                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("Shaders");

                        ImGui::TableSetColumnIndex(1);
                        if (ImGui::Button("Reload##2")) ctx->_shaders->reload_all();

                        ImGui::TableSetColumnIndex(2);
                        if (ImGui::Button("Unload##2")) ctx->_shaders->unload_all();

                        ImGui::EndTable();
                    }
                
                    ImGui::SeparatorText("Cast Libraries");

                    if (ImGui::Button("Reload All")) {
                        for (const auto &pair : ctx->_castlibs->libs()) pair.second->reload_members();
                    }

                    if (ImGui::Button("Unload All")) {
                        for (const auto &pair : ctx->_castlibs->libs()) pair.second->unload_members();
                    }

                    if (ImGui::BeginTable("##CastLibs", 5, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
                        ImGui::TableSetupColumn("Name");
                        ImGui::TableSetupColumn("Members");
                        ImGui::TableSetupColumn("Loaded");
                        ImGui::TableHeadersRow();

                        for (const auto &pair : ctx->_castlibs->libs()) {
                            ImGui::TableNextRow();

                            ImGui::TableSetColumnIndex(0);
                            ImGui::Text(pair.first.c_str());

                            ImGui::TableSetColumnIndex(1);
                            ImGui::Text(std::to_string(pair.second->get_members().size()).c_str());

                            ImGui::TableSetColumnIndex(2);
                            ImGui::Text(pair.second->is_loaded() ? "Yes" : "No");

                            ImGui::TableSetColumnIndex(3);
                            if (ImGui::Button((std::string("Reload##") + pair.first).c_str())) pair.second->reload_members();
                            
                            ImGui::TableSetColumnIndex(4);
                            if (ImGui::Button((std::string("Unload##") + pair.first).c_str())) pair.second->unload_members();
                        }

                        ImGui::EndTable();
                    }
                }
                break;

                default: 
                {
                    ImGui::SetNextItemWidth(100);
                    ImGui::InputInt("Event handle per frame", &config->event_handle_per_frame);
                    
                    ImGui::SetNextItemWidth(100);
                    ImGui::InputInt("Load per frame", &config->load_per_frame);
                    
                    ImGui::Checkbox("List wrap", &config->list_wrap);
                    ImGui::Checkbox("Strict deserialization", &config->strict_deserialization);

                    ImGui::SeparatorText("Sprites");

                    if (ImGui::BeginTable("##GeneralSprites", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
                        
                        ImGui::TableSetupColumn("Sprite");
                        ImGui::TableSetupColumn("Visible");
                        ImGui::TableSetupColumn("Opacity");
                        ImGui::TableHeadersRow();

                        ImGui::TableNextRow();

                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("Tiles");

                        ImGui::TableSetColumnIndex(1);
                        ImGui::Checkbox("##1", &config->default_sprites.tiles.visible);

                        ImGui::TableSetColumnIndex(2);
                        ImGui::InputInt("##2", &config->default_sprites.tiles.opacity);

                        ImGui::TableNextRow();
                        
                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("Props");

                        ImGui::TableSetColumnIndex(1);
                        ImGui::Checkbox("##3", &config->default_sprites.props.visible);

                        ImGui::TableSetColumnIndex(2);
                        ImGui::InputInt("##4", &config->default_sprites.props.opacity);

                        ImGui::TableNextRow();
                        
                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("Materials");

                        ImGui::TableSetColumnIndex(1);
                        ImGui::Checkbox("##5", &config->default_sprites.materials.visible);

                        ImGui::TableSetColumnIndex(2);
                        ImGui::InputInt("##6", &config->default_sprites.materials.opacity);

                        ImGui::TableNextRow();
                        
                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("Water");

                        ImGui::TableSetColumnIndex(1);
                        ImGui::Checkbox("##7", &config->default_sprites.water.visible);

                        ImGui::TableSetColumnIndex(2);
                        ImGui::InputInt("##8", &config->default_sprites.water.opacity);

                        ImGui::TableNextRow();
                        
                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("Grid");

                        ImGui::TableSetColumnIndex(1);
                        ImGui::Checkbox("##9", &config->default_sprites.grid.visible);

                        ImGui::TableSetColumnIndex(2);
                        ImGui::InputInt("##10", &config->default_sprites.grid.opacity);

                        ImGui::EndTable();
                    }

                    ImGui::SeparatorText("Pre-rendering");

                    ImGui::Checkbox("Tinted Tiles", &config->tiles_prerender.tinted);
                    ImGui::Checkbox("Preview Tiles", &config->tiles_prerender.preview);
                    ImGui::Checkbox("Palette Tiles", &config->tiles_prerender.palette);

                    ImGui::Checkbox("Tinted Props", &config->props_prerender.tinted);
                    ImGui::Checkbox("Palette Props", &config->props_prerender.palette);

                    ImGui::Checkbox("Palette Materials", &config->materials_prerender.palette);

                    ImGui::Checkbox("Shadows", &config->shadows);
                }
                break;
            }

            ImGui::EndTable();
        }
    }

    ImGui::End();
}
void Settings_Page::f3() const noexcept {}

Settings_Page::Settings_Page(context *ctx) : LevelPage(ctx), category_index(0) {}
Settings_Page::~Settings_Page() {}

};