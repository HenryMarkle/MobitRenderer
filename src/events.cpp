#include <any>
#include <cstdint>
#include <iostream>

#include <MobitRenderer/events.h>
#include <MobitRenderer/pages.h>
#include <MobitRenderer/state.h>

namespace mr {

void handle_level_loaded(context *ctx, pages::Pager *pager,
                         const std::any &payload) {
  pager->select_page(1);
}

void handle_goto_page(context *ctx, pages::Pager *pager,
                         const std::any &payload) {
  if (payload.type() != typeid(int)) return;

  int page = std::any_cast<int>(payload);

  pager->select_page(page);
}

}; // namespace mr
