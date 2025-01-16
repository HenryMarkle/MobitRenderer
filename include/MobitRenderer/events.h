#pragma once

#include <any>

// DO NOT include the current header file inside the following two header files:
#include <MobitRenderer/pages.h>
#include <MobitRenderer/state.h>

namespace mr {

void handle_level_loaded(context *, pages::pager *, const std::any &);
void handle_goto_page(context *, pages::pager *, const std::any &);
}; // namespace mr
