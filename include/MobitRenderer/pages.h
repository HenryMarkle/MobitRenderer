#pragma once

#include <cstdint>
#include <initializer_list>
#include <memory>
#include <thread>
#include <vector>

#include <spdlog/logger.h>

#include <MobitRenderer/default_array.h>
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

  Page &operator=(Page const&) = delete;
  Page &operator=(Page&&) noexcept = delete;

  Page(Page const&) = delete;
  Page(Page&&) noexcept = delete;
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

virtual void on_level_selected() noexcept;
virtual void on_level_loaded() noexcept;
virtual void on_level_created() noexcept;
virtual void on_level_unloaded() noexcept;
virtual void on_page_selected() noexcept;

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
  void on_level_loaded() noexcept override;
  void on_level_unloaded() noexcept override;
  void on_level_selected() noexcept override;
  void on_page_selected() noexcept override;

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
  void on_level_loaded() noexcept override;
  void on_level_unloaded() noexcept override;
  void on_level_selected() noexcept override;
  void on_page_selected() noexcept override;
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
  _selected_tile_category_index, 
  _selected_tile_index,
  _selected_material_category_index,
  _selected_material_index;

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

MaterialDef *_selected_material;

void _redraw_tile_preview_rt() noexcept;
void _redraw_tile_texture_rt() noexcept;
void _redraw_tile_specs_rt() noexcept;

public:
  void process() override;
  void draw() noexcept override;
  void windows() noexcept override;
  void order_level_redraw() noexcept override;
  void on_level_loaded() noexcept override;
  void on_level_unloaded() noexcept override;
  void on_level_selected() noexcept override;
  void on_page_selected() noexcept override;
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
  void on_level_loaded() noexcept override;
  void on_level_unloaded() noexcept override;
  void on_level_selected() noexcept override;
  void on_page_selected() noexcept override;

  Camera_Page(context*);
  ~Camera_Page() override;

};

class Light_Page : public LevelPage {

protected:

  bool 
    _hovering_on_window, 
    _should_redraw,
    _custom_rotation,
    _click_lock;

  size_t _brush_index;

  float
    _brush_width, 
    _brush_height, 
    _half_brush_width,
    _half_brush_height;

  float _brush_growth_speed, _brush_growth_accelaration, _brush_rotation;

  /// @brief 0 - paint shadows; 1 - paint light
  uint8_t _paint_mode;

  Vector2 _projection_angle, _brush_pinned_pos;

  void _update_projection_angle() noexcept;

public:

  void on_level_loaded() noexcept override;
  void on_level_unloaded() noexcept override;
  void on_level_selected() noexcept override;
  void on_page_selected() noexcept override;

  void process() override;
  void draw() noexcept override;
  void windows() noexcept override;
  void f3() const noexcept override;

  Light_Page(context*);
  ~Light_Page();

};
class Effects_Page : public LevelPage {

public:

  void on_level_loaded() noexcept override;
  void on_level_unloaded() noexcept override;
  void on_level_selected() noexcept override;
  void on_page_selected() noexcept override;
  Effects_Page(context*);
  ~Effects_Page();

};
class Dimensions_Page : public LevelPage {

public:
  void on_level_loaded() noexcept override;
  void on_level_unloaded() noexcept override;
  void on_level_selected() noexcept override;
  void on_page_selected() noexcept override;
  Dimensions_Page(context*);
  ~Dimensions_Page();

};
class Props_Page : public LevelPage {

private:

  bool _hovering_on_window;

  bool _should_redraw,
    _should_redraw1,
    _should_redraw2,
    _should_redraw3,
    _should_redraw_tile1,
    _should_redraw_tile2,
    _should_redraw_tile3,
    _should_redraw_props;

  
  default_array<bool> _selected, _hidden;

  TileDef 
    *_selected_tile, 
    *_hovered_tile,
    *_previously_drawn_tile_texture;
  
  PropDef 
    *_selected_prop, 
    *_hovered_prop,
    *_previously_drawn_prop_texture;

  size_t 
    _selected_tile_category_index, 
    _selected_tile_index,
    _selected_prop_category_index,
    _selected_prop_index,
    _selected_count;

  RenderTexture2D _tile_texture_rt, _prop_texture_rt;

  void _redraw_tile_preview_rt() noexcept;
  void _redraw_prop_preview_rt() noexcept;
  void resize_indices() noexcept;

public:

  void on_level_loaded() noexcept override;
  void on_level_unloaded() noexcept override;
  void on_level_selected() noexcept override;
  void on_page_selected() noexcept override;

  void process() noexcept override;
  void draw() noexcept override;
  void windows() noexcept override;
  void f3() const noexcept override;


  Props_Page(context*);
  ~Props_Page();

};
class Settings_Page : public LevelPage {

private:

  int category_index;

public:

  void process() noexcept override;
  void draw() noexcept override;
  void windows() noexcept override;
  void f3() const noexcept override;

  Settings_Page(context*);
  ~Settings_Page();

};
class Render_Page : public LevelPage {};
class AutoRender_Page : public LevelPage {};

class pager {

private:

std::vector<LevelPage*> _pages;
LevelPage *_selected_page, *_previous_page;

size_t _selected, _previous;

public:

void select(size_t) noexcept;
inline size_t get_selected_index() const noexcept { return _selected; }
inline size_t get_previous_index() const noexcept { return _previous; }
inline LevelPage *get_selected() const noexcept { return _pages[_selected]; }
inline LevelPage *get_previous() const noexcept { return _pages[_previous]; }
inline const std::vector<LevelPage*> &get_pages() const noexcept { return _pages; }

pager &operator=(pager const&) = delete;

pager(context*);
pager(pager const&) = delete;
~pager();

};

}; // namespace mr::pages
