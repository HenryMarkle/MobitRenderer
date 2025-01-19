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
  context *ctx;

  Page(context*);

public:
  /// @brief Updates the state of the page.
  /// @attention Do not call drawing functions here.
  virtual void process();

  /// @brief Draws the content of the page
  virtual void draw() noexcept;

  /// @brief Draws ImGui windows
  virtual void windows() noexcept;

  /// @brief Draws debug information
  virtual void f3() const noexcept;

  /// @brief Resets data that need to be reset every frame.
  virtual void reset_frame_data() noexcept;

  /// @brief Propagates texture buffer refreshing.
  virtual void order_level_redraw() noexcept;

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

  Start_Page(context*);

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

  Main_Page(context*);

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
  virtual void f3() const noexcept;

  Geo_Page(context*);

  virtual ~Geo_Page() override;
};

class Tile_Page : public Page {
private:
  bool 
    _should_redraw, 
    _should_redraw1,
    _should_redraw2,
    _should_redraw3,
    _should_redraw_tile1,
    _should_redraw_tile2,
    _should_redraw_tile3;

bool _hovering_on_window;

size_t 
  _selected_category_index, 
  _selected_tile_index;

/// @brief Viewport to draw currently selected tile preview.
RenderTexture2D _tile_preview_rt;

/// @brief Viewport to draw currently selected tile texture.
RenderTexture2D _tile_texture_rt;

/// @brief Viewport to draw currently selected tile specs.
RenderTexture2D _tile_specs_rt;

TileDef 
  *_selected_tile, 
  *_hovered_tile,
  *_previously_drawn_texture,
  *_previously_drawn_preview,
  *_previously_drawn_specs;

void _redraw_tile_preview_rt() noexcept;
void _redraw_tile_texture_rt() noexcept;
void _redraw_tile_specs_rt() noexcept;

public:
  virtual void process() override;
  virtual void draw() noexcept override;
  virtual void windows() noexcept override;
  virtual void order_level_redraw() noexcept override;
  virtual void f3() const noexcept;

  Tile_Page(context*);

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
void current_f3() const noexcept;

pager &operator=(pager const&) = delete;

pager(context*);
pager(pager const&) = delete;
~pager();

};

}; // namespace mr::pages
