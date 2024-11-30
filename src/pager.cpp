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

}; // namespace mr::pages
