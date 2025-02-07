#if defined(_WIN32) || defined(_WIN64)
  #define WIN32_LEAN_AND_MEAN
#endif

#include <any>
#include <cstdint>
#include <iostream>

#include <MobitRenderer/events.h>
#include <MobitRenderer/pages.h>
#include <MobitRenderer/state.h>

namespace mr {

void handle_level_loaded(context *ctx, pages::pager *pager, const std::any &payload) {
  pager->select(1);
  for (auto p : pager->get_pages()) p->on_level_loaded();
}

void handle_level_selected(context *ctx, pages::pager *pager, const std::any &payload) {
  for (auto p : pager->get_pages()) p->on_level_selected();
}

void handle_goto_page(context *ctx, pages::pager *pager,
                         const std::any &payload) {
  if (payload.type() != typeid(int)) return;

  int page = std::any_cast<int>(payload);

  pager->select(page);
}

}; // namespace mr
