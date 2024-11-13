#include <algorithm>
#include <cstdint>
#include <cstring>
#include <exception>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

#include <imgui.h>
#include <raylib.h>
#include <rlImGui.h>

#include <MobitRenderer/imwin.h>
#include <MobitRenderer/state.h>

#ifdef __linux__
#include <limits.h>
#endif

#ifdef _WIN32
#include <windows.h>
#endif

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

      if (!filters.empty()) {
        for (auto &filter : filters) {
          if (filter == path.extension())
            goto skip_filters;
        }

        continue;
      }

    skip_filters:

      paths.push_back(path);
      names.push_back(path.stem());
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

  path_source.copy(current_path, std::min(path_source.length(), path_max_len));
  path_source[std::min(path_source.length(), path_max_len) - 1] = 0;

  current_dir = dir;
  entry_paths = paths;
  entry_names = names;
  entry_is_dir = is_dir;
}

ProjectExplorer::ProjectExplorer() : level_geo(nullptr) {
  auto exec_dir = get_current_dir();

#ifdef __linux__
  path_max_len = PATH_MAX;
#endif

#ifdef _WIN32
  path_max_len = MAX_PATH;
#endif

  current_path = new char[path_max_len];
  memset(current_path, 0, path_max_len);

  preview_rt = {.id = 0};

  go_to(exec_dir);
}

ProjectExplorer::ProjectExplorer(std::shared_ptr<dirs> dirs,
                                 std::shared_ptr<textures> _textures) {
  std::filesystem::path path(dirs->get_projects());
  textures_ = _textures;

  const auto &icons_dir = dirs->get_assets() / "Icons";
  const auto file_icon_path = icons_dir / "file icon.png";
  const auto folder_icon_path = icons_dir / "folder icon.png";

#ifdef __linux__
  path_max_len = PATH_MAX;
#endif

#ifdef _WIN32
  path_max_len = MAX_PATH;
#endif

  current_path = new char[path_max_len];
  memset(current_path, 0, path_max_len);

  preview_rt = {.id = 0};

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

/// @attention This function must be called after rlImGuiBegin() and before
/// rlImGuiEnd().
void ProjectExplorer::draw() noexcept {
  auto opened = ImGui::Begin("Project Explorer");

  if (opened) {
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
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

          rlImGuiImageRect((entry_is_dir[n] ? folder_icon : file_icon), 20, 20,
                           Rectangle{0, 0, (float)file_icon->width,
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
                // TODO: trigger load level here
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
}

const std::vector<std::string> &ProjectExplorer::get_filters() const {
  return filters;
}

void ProjectExplorer::set_filters(std::vector<std::string> &&filters_) {
  filters = std::move(filters_);

  go_to(current_dir);
}

void ProjectExplorer::set_filters(std::vector<std::string> filters_) {
  filters = std::move(filters_);

  go_to(current_dir);
}

}; // namespace mr
