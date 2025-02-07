#include <MobitRenderer/pages.h>
#include <MobitRenderer/state.h>

namespace mr::pages {

void Dimensions_Page::on_level_loaded() noexcept {}
void Dimensions_Page::on_level_unloaded() noexcept {}
void Dimensions_Page::on_level_selected() noexcept {}
void Dimensions_Page::on_page_selected() noexcept {}

Dimensions_Page::Dimensions_Page(context *ctx) : LevelPage(ctx) {}
Dimensions_Page::~Dimensions_Page() {}

};