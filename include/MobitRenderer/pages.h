#pragma once

#include <cstdint>
#include <initializer_list>
#include <memory>
#include <thread>
#include <vector>

#include <spdlog/logger.h>

#include <MobitRenderer/imwin.h>
#include <MobitRenderer/state.h>
#include <MobitRenderer/level.h>

namespace mr::pages {

class Page {
protected:
  std::shared_ptr<context> ctx;

  Page(std::shared_ptr<context>);

public:
  /// @brief Updates the state of the page.
  /// @attention Do not call drawing functions here.
  virtual void process() = 0;

  /// @brief Draws the content of the page
  virtual void draw() noexcept = 0;

  /// @brief Draws ImGui windows
  virtual void windows() noexcept = 0;

  /// @brief Propagates texture buffer refreshing.
  virtual void order_level_redraw() noexcept = 0;

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
  void push_page(Page *) noexcept;
  uint8_t get_current_page_index() const noexcept;
  uint8_t get_previous_page_index() const noexcept;

  Page *get_current_page() const noexcept;
  Page *get_previous_page() const noexcept;
  void select_page(uint8_t) noexcept;

  Pager();
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
  virtual void order_level_redraw() noexcept override;

  Start_Page(std::shared_ptr<context>);

  virtual ~Start_Page() override;
};

class Main_Page : public Page {
private:
  bool should_redraw;

public:
  void order_redraw() noexcept;

  virtual void process() override;
  virtual void draw() noexcept override;
  virtual void windows() noexcept override;
  virtual void order_level_redraw() noexcept override;

  Main_Page(std::shared_ptr<context>);

  virtual ~Main_Page() override;
};

class Geo_Page : public Page {
private:
  bool 
    should_redraw, 
    should_redraw1,
    should_redraw2,
    should_redraw3,
    should_redraw_feature1,
    should_redraw_feature2,
    should_redraw_feature3;

public:
  virtual void process() override;
  virtual void draw() noexcept override;
  virtual void windows() noexcept override;
  virtual void order_level_redraw() noexcept override;

  Geo_Page(std::shared_ptr<context>);

  virtual ~Geo_Page() override;
};

class Tile_Page : public Page {
private:
  bool 
    should_redraw, 
    should_redraw1,
    should_redraw2,
    should_redraw3,
    should_redraw_tile1,
    should_redraw_tile2,
    should_redraw_tile3;

public:
  virtual void process() override;
  virtual void draw() noexcept override;
  virtual void windows() noexcept override;
  virtual void order_level_redraw() noexcept override;

  Tile_Page(std::shared_ptr<context>);

  virtual ~Tile_Page() override;
};

class pager {

private:

Start_Page _start_page;
Main_Page _main_page;
Geo_Page _geo_page;
Tile_Page _tile_page;

int _selected, _previous;

public:

void select(int) noexcept;
int get_selected_index() const noexcept;
int get_previous_index() const noexcept;

void current_process();
void current_draw() noexcept;
void current_windows() noexcept;

pager &operator=(pager const&) = delete;

pager(std::shared_ptr<context>);
pager(pager const&) = delete;
~pager();
};
}; // namespace mr::pages
