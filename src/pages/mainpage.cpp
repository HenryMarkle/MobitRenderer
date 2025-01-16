#if defined(_WIN32) || defined(_WIN64)
  #define WIN32_LEAN_AND_MEAN
#endif

#include <cstdint>
#include <iostream>

#define MIN(a, b) a > b ? b : a

#include <raylib.h>
#include <imgui.h>

#include <MobitRenderer/draw.h>
#include <MobitRenderer/matrix.h>
#include <MobitRenderer/pages.h>



namespace mr::pages {

void Main_Page::order_redraw() noexcept { should_redraw = true; }

void Main_Page::process() {
  auto f3 = ctx->f3_;

  f3->enqueue("W ");
  f3->enqueue(ctx->get_selected_level()->get_width(), true);
  f3->enqueue(" H ", true);
  f3->enqueue(ctx->get_selected_level()->get_height(), true);
}

void Main_Page::draw() noexcept {

  // Draw the level into the main buffer.
  // TODO: Change This.
  if (should_redraw) {
    BeginTextureMode(ctx->textures_->get_main_level_viewport());

    ClearBackground(Color{240, 240, 240, 255});

    auto *level = ctx->get_selected_level();
    auto &gmatrix = level->get_geo_matrix();

    for (uint16_t x = 0; x < gmatrix.get_width(); x++) {
      for (uint16_t y = 0; y < gmatrix.get_height(); y++) {
        auto cell1 = gmatrix.get_copy(x, y, 0);
        auto cell2 = gmatrix.get_copy(x, y, 1);
        auto cell3 = gmatrix.get_copy(x, y, 2);

        mr::draw_mtx_geo_type(cell1, x, y, 20, BLACK);
        mr::draw_mtx_geo_type(cell2, x, y, 20,
                              Color{ 0, 255, 0, 80});
        mr::draw_mtx_geo_type(cell3, x, y, 20,
                              Color{ 255, 0, 0, 80});
      }
    }

    EndTextureMode();

    should_redraw = false;
  }

  ClearBackground(DARKGRAY);

  BeginShaderMode(ctx->get_shaders_const().vflip());
  const Shader &shader = ctx->get_shaders_const().vflip();
  const auto texture = ctx->textures_->get_main_level_viewport().texture;
  SetShaderValueTexture(shader, GetShaderLocation(shader, "texture0"), texture);

  float scale = MIN((GetScreenWidth() - 80) / (texture.width*1.0f), (GetScreenHeight() - 80) / (texture.height*1.0f));
  float scaledx = texture.width * scale, 
        scaledy = texture.height * scale;
  float bspacex = GetScreenWidth() - scaledx, 
        bspacey = GetScreenHeight() - scaledy;

  DrawTexturePro(
    texture, 
    Rectangle{0, 0, (float)texture.width, (float)texture.height}, 
    Rectangle{
      bspacex/2.0f, 
      bspacey/2.0f, 
      texture.width * scale, 
      texture.height * scale
    }, 
    Vector2{0, 0}, 
    0, 
    WHITE
  );


  EndShaderMode();
}

void Main_Page::windows() noexcept {
  
}

void Main_Page::order_level_redraw() noexcept {
  should_redraw = true;
}

Main_Page::Main_Page(context *ctx) : Page(ctx), should_redraw(true) {}

Main_Page::~Main_Page() {}

}; // namespace mr::pages
