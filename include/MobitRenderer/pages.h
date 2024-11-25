#pragma once

#include "MobitRenderer/level.h"
#include <cstdint>
#include <initializer_list>
#include <memory>
#include <thread>
#include <vector>

#include <spdlog/logger.h>

#include <MobitRenderer/imwin.h>
#include <MobitRenderer/state.h>

namespace mr::pages {

class Page {
protected:
  std::shared_ptr<context> ctx_;
  std::shared_ptr<spdlog::logger> logger_;

  Page(std::shared_ptr<context>, std::shared_ptr<spdlog::logger>);

public:
  /// @brief Updates the state of the page
  virtual void process() = 0;

  /// @brief Draws the content of the page
  virtual void draw() noexcept = 0;

  /// @brief Draws ImGui windows
  virtual void windows() noexcept = 0;

  virtual ~Page() = default;
};

/// @brief Stores and manages pages.
/// @warning Pager takes ownership of the Page* pointers and
/// WILL DEALLOCATE THEM UPON DESTRUCTION.
/// Do not keep the Page* pointers that were passed to this
/// class beyond the life delete of this class.
class Pager {
  std::vector<Page *> pages;
  uint8_t current_page, previous_page;

  Page *current_page_ptr, *previous_page_ptr;

public:
  uint8_t get_current_page_index() const noexcept;
  uint8_t get_previous_page_index() const noexcept;

  Page *get_current_page() const noexcept;
  Page *get_previous_page() const noexcept;
  void select_page(uint8_t) noexcept;

  Pager(std::shared_ptr<context>, std::shared_ptr<spdlog::logger>);
  Pager(std::initializer_list<Page *>);
  ~Pager();
};

class Start_Page : public Page {
private:
  ProjectExplorer explorer_;
  std::unique_ptr<std::thread> project_load_thread;
  std::unique_ptr<Level> loaded_level;
  bool explorer_file_clicked;
  bool project_thread_done;
  bool loaded_project_was_handled;

public:
  virtual void process() override;
  virtual void draw() noexcept override;
  virtual void windows() noexcept override;
  Start_Page(std::shared_ptr<context>, std::shared_ptr<spdlog::logger>);

  virtual ~Start_Page() override;
};

}; // namespace mr::pages
