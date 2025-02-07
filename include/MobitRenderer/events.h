#pragma once

#include <any>

#include <MobitRenderer/pages.h>
#include <MobitRenderer/state.h>

namespace mr {

void handle_level_loaded(context *, pages::pager *, const std::any &);
void handle_level_selected(context *, pages::pager *, const std::any &);
void handle_goto_page(context *, pages::pager *, const std::any &);

}; // namespace mr
