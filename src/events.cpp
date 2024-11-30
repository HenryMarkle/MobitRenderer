#include <any>

#include <MobitRenderer/events.h>
#include <MobitRenderer/pages.h>
#include <MobitRenderer/state.h>

namespace mr {

void handle_level_loaded(context *ctx, pages::Pager *pager,
                         const std::any &payload) {
  pager->select_page(1);
}

}; // namespace mr
