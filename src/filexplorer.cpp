#include <algorithm>
#include <cstdint>
#include <cstring>
#include <exception>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

#include <raylib.h>
#include <imgui.h>
#include <rlImGui.h>

#include <MobitRenderer/imwin.h>
#include <MobitRenderer/state.h>
#include <MobitRenderer/io.h>

namespace mr {

void ProjectExplorer::new_preview(uint16_t width, uint16_t height) {
  x = y = z = 0;

  if (preview_rt.id != 0)
    UnloadRenderTexture(preview_rt);

  preview_rt = LoadRenderTexture(width, height);

  BeginTextureMode(preview_rt);
  ClearBackground(BLACK);
  EndTextureMode();
}

void ProjectExplorer::go_to(const std::filesystem::path &dir) {
  std::vector<std::filesystem::path> paths;
  std::vector<std::string> names;
  std::vector<bool> is_dir;

  try {
    for (const auto &entry : std::filesystem::directory_iterator(dir)) {
      auto path = entry.path();

      if (!path.has_stem())
        continue;
      if (!entry.is_directory() && !path.has_extension())
        continue;

      if (!std::filesystem::is_directory(path) && !filters.empty()) {
        for (auto &filter : filters) {
          if (filter == path.extension())
            goto skip_filters;
        }

        continue;
      }

    skip_filters:

      paths.push_back(path);
      names.push_back(path.stem().string());
      is_dir.push_back(entry.is_directory());
    }
  } catch (const std::filesystem::filesystem_error &e) {

  } catch (const std::exception &e) {
  }

  if (preview_rt.id != 0)
    UnloadRenderTexture(preview_rt);

  x = y = z = 0;

  selected_entry = hovered_entry = 0;

  auto path_source = dir.string();

  memset(current_path, 0, path_max_len);

  path_source.copy(current_path, std::min(path_source.length(), path_max_len));
  path_source[std::min(path_source.length(), path_max_len) - 1] = 0;

  current_dir = dir;
  entry_paths = paths;
  entry_names = names;
  entry_is_dir = is_dir;
}

ProjectExplorer::dialogmode ProjectExplorer::get_dialog_mode() const noexcept {
  return dialog_mode;
}

void ProjectExplorer::set_dialog_mode(
    ProjectExplorer::dialogmode mode) noexcept {
  dialog_mode = mode;
}

const char *ProjectExplorer::get_current_path() const noexcept {
  return current_path;
}
const std::filesystem::path *
ProjectExplorer::get_selected_entry_ptr() const noexcept {
  if (entry_paths.empty())
    return nullptr;

  return &entry_paths[selected_entry];
}

ProjectExplorer::ProjectExplorer() : level_geo(nullptr) {
  auto exec_dir = get_executable_dir();

  projects_dir = exec_dir / "Projects";

  path_max_len = mr::get_path_max_len();

  current_path = new char[path_max_len];
  memset(current_path, 0, path_max_len);

  preview_rt = {0};

  go_to(exec_dir);
}

ProjectExplorer::ProjectExplorer(std::shared_ptr<Dirs> dirs,
                                 textures *_textures) {
  std::filesystem::path path(dirs->get_projects());

  projects_dir = dirs->get_projects();

  textures_ = _textures;

  const auto &icons_dir = dirs->get_assets() / "Icons";
  const auto file_icon_path = icons_dir / "file icon.png";
  const auto folder_icon_path = icons_dir / "folder icon.png";

  path_max_len = mr::get_path_max_len();

  current_path = new char[path_max_len];
  memset(current_path, 0, path_max_len);

  preview_rt = {0};

  if (std::filesystem::is_directory(path))
    go_to(path);
  else {
    auto parent = path.parent_path();

    go_to(parent);

    for (int p = 0; p < entry_paths.size(); p++) {
      if (entry_paths[p] == path) {
        selected_entry = p;
        break;
      }
    }
  }
}

ProjectExplorer::~ProjectExplorer() {
  if (preview_rt.id != 0)
    UnloadRenderTexture(preview_rt);

  delete[] current_path;
}

bool ProjectExplorer::draw() noexcept {
  bool dialog_selected = false;

  auto opened = ImGui::Begin("Project Explorer");

  if (opened) {
    if (current_path_updated) {

      std::filesystem::path buffer_path(current_path);
      if (std::filesystem::exists(buffer_path) &&
          std::filesystem::is_directory(buffer_path)) {
        go_to(buffer_path);
      }
      current_path_updated = false;
    }

    { // input handling

      if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_DownArrow)) {
        if (++selected_entry >= entry_paths.size())
          selected_entry = entry_paths.size() - 1;
      } else if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_UpArrow)) {
        if (selected_entry-- == 0)
          selected_entry = 0;
      } else if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_LeftArrow)) {
        go_to(current_dir.parent_path());
      } else if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_RightArrow)) {
        if (!entry_paths.empty() && entry_is_dir[selected_entry])
          go_to(entry_paths[selected_entry]);
      }

      if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Enter)) {
        if (!entry_paths.empty()) {
          if (entry_is_dir[selected_entry]) {
            if (dialog_mode == folder)
              dialog_selected = true;
            else
              go_to(entry_paths[selected_entry]);
          } else {
            if (dialog_mode == file)
              dialog_selected = true;
          }
        }
      }
    }

    bool up_clicked = false, home_clicked = false;

    if (textures_ == nullptr) {
      up_clicked = ImGui::Button("Up");
    } else {
      up_clicked = rlImGuiImageButtonSize("##up", textures_->up_icon.get_ptr(),
                                          ImVec2(20, 20));
    }

    if (ImGui::IsItemHovered()) {
      ImGui::BeginTooltip();
      ImGui::Text("Go up a directory");
      ImGui::EndTooltip();
    }
    ImGui::SameLine();

    if (textures_ == nullptr) {
      home_clicked = ImGui::Button("Home");
    } else {
      home_clicked = rlImGuiImageButtonSize(
          "##home", textures_->home_icon.get_ptr(), ImVec2(20, 20));
    }

    if (ImGui::IsItemHovered()) {
      ImGui::BeginTooltip();
      ImGui::Text("Return to /Projects");
      ImGui::EndTooltip();
    }
    ImGui::SameLine();

    if (up_clicked)
      go_to(current_dir.parent_path());
    if (home_clicked)
      go_to(projects_dir);

    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    current_path_updated |=
        ImGui::InputText("##path", current_path, path_max_len);

    if (ImGui::BeginTable("layout", 2, ImGuiTableFlags_Resizable)) {
      ImGui::TableSetupColumn("entries_col", ImGuiTableColumnFlags_WidthFixed,
                              300);

      ImGui::TableNextRow();

      ImGui::TableSetColumnIndex(0);
      if (ImGui::BeginListBox("##entries", ImGui::GetContentRegionAvail())) {
        for (auto n = 0; n < entry_names.size(); n++) {
          const auto &name = entry_names[n];

          auto *file_icon = textures_->file_icon.get_ptr();
          auto *folder_icon = textures_->folder_icon.get_ptr();

          rlImGuiImageRect((entry_is_dir[n] ? folder_icon : file_icon), 20, 20, Rectangle{0, 0, (float)file_icon->width,
                                     (float)file_icon->height});
          ImGui::SameLine();
          bool is_clicked =
              ImGui::Selectable(name.c_str(), n == selected_entry, 0,
                                ImVec2(ImGui::GetContentRegionAvail().x, 20));

          if (is_clicked) {
            if (n == selected_entry) {
              if (entry_is_dir[n])
                go_to(entry_paths[n]);
              else {
                dialog_selected = true;
              }
            } else {
              selected_entry = n;
            }
          }
        }
        ImGui::EndListBox();
      }

      ImGui::TableSetColumnIndex(1);
      if (preview_rt.id != 0) {
        rlImGuiImageRenderTextureFit(&preview_rt, false);
      }

      ImGui::EndTable();
    }
  }

  ImGui::End();

  return dialog_selected;
}

const std::vector<std::string> &ProjectExplorer::get_filters() const {
  return filters;
}

void ProjectExplorer::set_filters(std::vector<std::string> filters_) {
  filters = std::move(filters_);

  go_to(current_dir);
}

}; // namespace mr
