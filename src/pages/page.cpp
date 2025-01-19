#include <MobitRenderer/pages.h>
#include <MobitRenderer/state.h>

namespace mr::pages {

Page::Page(context *ctx) : ctx(ctx) {}

void Page::process() {}
void Page::draw() noexcept {}
void Page::windows() noexcept {}
void Page::f3() const noexcept {}
void Page::reset_frame_data() noexcept {}
void Page::order_level_redraw() noexcept {}

};