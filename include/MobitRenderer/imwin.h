#pragma once

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

#include <raylib.h>

#include <MobitRenderer/matrix.h>

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

  RenderTexture2D preview_rt;

  /// Used to as cursors for drawing the project preview.
  uint16_t x, y, z;

  void go_to(const std::filesystem::path &dir);

  void draw_preview() const noexcept;
  void new_preview(uint16_t width, uint16_t height);

public:
  /// Draws the window.
  void draw() noexcept;

  const std::vector<std::string> &get_filters() const;
  void set_filters(std::vector<std::string> &&filters_);
  void set_filters(std::vector<std::string> filters_);

  /// Initializes the explorer with the executable's directory as the current
  /// one.
  ProjectExplorer();

  /// Initializes the explorer and sets the current directory to the given one.
  ProjectExplorer(const std::string &&dir_or_file);

  ~ProjectExplorer();
};

}; // namespace mr
