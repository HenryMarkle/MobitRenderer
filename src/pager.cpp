#include <cstdint>
#include <initializer_list>
#include <vector>

#include <spdlog/logger.h>

#include <MobitRenderer/pages.h>
#include <MobitRenderer/state.h>

namespace mr::pages {

void Pager::push_page(LevelPage *page) noexcept { pages.push_back(page); }
uint8_t Pager::get_current_page_index() const noexcept { return current_page; }
uint8_t Pager::get_previous_page_index() const noexcept {
  return previous_page;
}
LevelPage *Pager::get_current_page() const noexcept { return current_page_ptr; }
LevelPage *Pager::get_previous_page() const noexcept { return previous_page_ptr; }
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
Pager::Pager(std::initializer_list<LevelPage *> pages)
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

void pager::select(size_t index) noexcept {
  if (index < 0 || index > 9) return;

  _previous = _selected;
  _selected = index;

  _previous_page = _selected_page;
  _selected_page = _pages[_selected];
  
  _selected_page->on_page_selected();
}

pager::pager(context *ctx) :
  _selected(0),
  _previous(0),
  _pages({
    new Start_Page(ctx),
    new Main_Page(ctx),
    new Geo_Page(ctx),
    new Tile_Page(ctx),
    new Camera_Page(ctx),
    new Light_Page(ctx),
    new Dimensions_Page(ctx),
    new Effects_Page(ctx),
    new Props_Page(ctx),
    new Settings_Page(ctx)
  }),
  _selected_page(nullptr),
  _previous_page(nullptr) {
    _selected_page = _previous_page = _pages[0];
  }

pager::~pager() {
  for (auto  *page : _pages) delete page;
}

}; // namespace mr::pages
