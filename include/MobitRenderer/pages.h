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

/// @brief An abstract class representing a page to draw on screen.
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

  virtual ~Page() = default;
};

/// @brief Represents a page that reviews a level
class LevelPage : public Page {

protected:

bool _is_mouse_in_mtx_bounds;
mr::ivec2 _mtx_mouse_pos;

void update_mtx_mouse_pos() noexcept;

LevelPage(context*);

public:

/// @brief Propagates texture buffer refreshing.
virtual void order_level_redraw() noexcept;

virtual ~LevelPage() = default;

};

/// @brief Stores and manages pages.
/// @warning Pager takes ownership of the Page* pointers and
/// WILL DEALLOCATE THEM UPON DESTRUCTION.
/// Do not keep the Page* pointers that were passed to this
/// class beyond the life delete of this class.
class Pager {
  std::vector<LevelPage *> pages;
  uint8_t current_page, previous_page;

  LevelPage *current_page_ptr, *previous_page_ptr;

public:
  void push_page(LevelPage *) noexcept;
  uint8_t get_current_page_index() const noexcept;
  uint8_t get_previous_page_index() const noexcept;

  LevelPage *get_current_page() const noexcept;
  LevelPage *get_previous_page() const noexcept;
  void select_page(uint8_t) noexcept;

  Pager();
  Pager(std::initializer_list<LevelPage *>);
  ~Pager();
};

class Start_Page : public LevelPage {
private:
  ProjectExplorer explorer_;
  std::unique_ptr<std::thread> project_load_thread;
  std::unique_ptr<Level> loaded_level;
  bool explorer_file_clicked;
  bool project_thread_done;
  bool loaded_project_was_handled;

public:
  void process() override;
  void draw() noexcept override;
  void windows() noexcept override;
  void order_level_redraw() noexcept override;

  Start_Page(context*);

  ~Start_Page() override;
};

class Main_Page : public LevelPage {
private:
  bool should_redraw;

public:
  void order_redraw() noexcept;

  void process() override;
  void draw() noexcept override;
  void windows() noexcept override;
  void order_level_redraw() noexcept override;

  Main_Page(context*);

  ~Main_Page() override;
};

class Geo_Page : public LevelPage {
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
  void process() override;
  void draw() noexcept override;
  void windows() noexcept override;
  void order_level_redraw() noexcept override;
  void f3() const noexcept;

  Geo_Page(context*);

  ~Geo_Page() override;
};

class Tile_Page : public LevelPage {
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
  
TileCell *_hovered_cell;

void _redraw_tile_preview_rt() noexcept;
void _redraw_tile_texture_rt() noexcept;
void _redraw_tile_specs_rt() noexcept;

public:
  void process() override;
  void draw() noexcept override;
  void windows() noexcept override;
  void order_level_redraw() noexcept override;
  void f3() const noexcept;

  Tile_Page(context*);

  ~Tile_Page() override;
};

class Camera_Page : public LevelPage {

private:

bool 
  _should_redraw1,
  _should_redraw2,
  _should_redraw3,
  _should_redraw;

public:

void process() override;
void draw() noexcept override;
void windows() noexcept override;
void f3() const noexcept override;

void order_level_redraw() noexcept override;

Camera_Page(context*);
~Camera_Page() override;

};

class Light_Page : public LevelPage {};
class Effects_Page : public LevelPage {};
class Props_Page : public LevelPage {

private:

  bool _hovering_on_window;

  bool _should_redraw;

  TileDef 
    *_selected_tile, 
    *_hovered_tile;
  
  PropDef 
    *_selected_prop, 
    *_hovered_prop;

  size_t 
    _selected_tile_category_index, 
    _selected_tile_index,
    _selected_prop_category_index,
    _selected_prop_index;

public:

  void process() noexcept override;
  void draw() noexcept override;
  void windows() noexcept override;
  void f3() const noexcept;

  Props_Page(context*);
  ~Props_Page();

};
class Render_Page : public LevelPage {};
class AutoRender_Page : public LevelPage {};

class pager {

private:

Start_Page _start_page;
Main_Page _main_page;
Geo_Page _geo_page;
Tile_Page _tile_page;
Camera_Page _camera_page;

Props_Page _props_page;

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
