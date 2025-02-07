#include <MobitRenderer/pages.h>
#include <MobitRenderer/state.h>

namespace mr::pages {

void Light_Page::on_level_loaded() noexcept {}
void Light_Page::on_level_unloaded() noexcept {}
void Light_Page::on_level_selected() noexcept {}
void Light_Page::on_page_selected() noexcept {}

Light_Page::Light_Page(context *ctx) : LevelPage(ctx) {}
Light_Page::~Light_Page() {}

};