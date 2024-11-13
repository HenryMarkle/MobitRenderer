#pragma once

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

#include <raylib.h>

#include <MobitRenderer/managed.h>
#include <MobitRenderer/matrix.h>
#include <MobitRenderer/state.h>

namespace mr {

class ProjectExplorer {
private:
  std::filesystem::path current_dir;

  std::vector<std::filesystem::path> entry_paths;
  std::vector<std::string> entry_names;
  std::vector<bool> entry_is_dir;

  size_t selected_entry, hovered_entry;

  size_t path_max_len;
  char *current_path;
  std::vector<std::string> filters;

  std::unique_ptr<Matrix<GeoCell>> level_geo;

  Texture2D file_icon, folder_icon;
  RenderTexture2D preview_rt;

  /// Used to as cursors for drawing the project preview.
  uint16_t x, y, z;

  void go_to(const std::filesystem::path &dir);

  void draw_preview() const noexcept;
  void new_preview(uint16_t width, uint16_t height);

public:
  /// @attention This function must be called after rlImGuiBegin() and before
  /// rlImGuiEnd().
  void draw() noexcept;

  const std::vector<std::string> &get_filters() const;
  void set_filters(std::vector<std::string> &&filters_);
  void set_filters(std::vector<std::string> filters_);

  /// Initializes the explorer with the executable's directory as the current
  /// one.
  ProjectExplorer();

  /// Initializes the explorer and sets the current directory to the given one.
  ProjectExplorer(std::shared_ptr<dirs> dirs);

  ~ProjectExplorer();
};

}; // namespace mr
