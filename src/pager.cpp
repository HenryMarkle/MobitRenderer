#if defined(_WIN32) || defined(_WIN64)
  #define WIN32_LEAN_AND_MEAN
#endif

#include <cstdint>
#include <initializer_list>
#include <vector>

#include <spdlog/logger.h>

#include <MobitRenderer/pages.h>
#include <MobitRenderer/state.h>

namespace mr::pages {

void Pager::push_page(Page *page) noexcept { pages.push_back(page); }
uint8_t Pager::get_current_page_index() const noexcept { return current_page; }
uint8_t Pager::get_previous_page_index() const noexcept {
  return previous_page;
}
Page *Pager::get_current_page() const noexcept { return current_page_ptr; }
Page *Pager::get_previous_page() const noexcept { return previous_page_ptr; }
void Pager::select_page(uint8_t index) noexcept {
  if (index >= pages.size())
    return;

  previous_page = current_page;
  current_page = index;
  previous_page_ptr = current_page_ptr;
  current_page_ptr = pages[index];

  current_page_ptr->order_level_redraw();
}

Pager::Pager() : current_page(-1), previous_page(-1) {}
Pager::Pager(std::initializer_list<Page *> pages)
    : pages({}), current_page_ptr(nullptr), current_page(-1),
      previous_page(-1) {
  this->pages.reserve(pages.size());
  for (auto *e : pages) {
    this->pages.push_back(e);
  }

  if (!this->pages.empty()) {
    current_page = 0;
    current_page_ptr = this->pages[0];
  }
}
Pager::~Pager() {
  if (pages.empty())
    return;

  for (auto *p : pages)
    delete p;
}

void pager::select(int index) noexcept {
  if (index < 0 || index > 3) return;
  _previous = _selected;
  _selected = index;

  switch (_selected) {
    case 0: _start_page.order_level_redraw(); break;
    case 1: _main_page.order_level_redraw(); break;
    case 2: _geo_page.order_level_redraw(); break;
    case 3: _tile_page.order_level_redraw(); break;
  }
}

int pager::get_selected_index() const noexcept { return _selected; }
int pager::get_previous_index() const noexcept { return _previous; }

void pager::current_process() {
  switch (_selected) {
    case 0: _start_page.process(); break;
    case 1: _main_page.process(); break;
    case 2: _geo_page.process(); break;
    case 3: _tile_page.process(); break;
  }
}
void pager::current_draw() noexcept {
  switch (_selected) {
    case 0: _start_page.draw(); break;
    case 1: _main_page.draw(); break;
    case 2: _geo_page.draw(); break;
    case 3: _tile_page.draw(); break;
  }
}
void pager::current_windows() noexcept {
  switch (_selected) {
    case 0: _start_page.windows(); break;
    case 1: _main_page.windows(); break;
    case 2: _geo_page.windows(); break;
    case 3: _tile_page.windows(); break;
  }
}
void pager::current_f3() const noexcept {
  switch (_selected) {
    case 0: _start_page.f3(); break;
    case 1: _main_page.f3(); break;
    case 2: _geo_page.f3(); break;
    case 3: _tile_page.f3(); break;
  }
}

pager::pager(context *ctx) :
  _selected(0),
  _previous(0),
  _start_page(Start_Page(ctx)),
  _main_page(Main_Page(ctx)),
  _geo_page(Geo_Page(ctx)),
  _tile_page(Tile_Page(ctx)) {}

pager::~pager() {}

}; // namespace mr::pages
