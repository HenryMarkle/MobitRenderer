#include <MobitRenderer/pages.h>
#include <MobitRenderer/state.h>

namespace mr::pages {

void Effects_Page::on_level_loaded() noexcept {}
void Effects_Page::on_level_unloaded() noexcept {}
void Effects_Page::on_level_selected() noexcept {}
void Effects_Page::on_page_selected() noexcept {}

Effects_Page::Effects_Page(context *ctx) : LevelPage(ctx) {}
Effects_Page::~Effects_Page() {}

};