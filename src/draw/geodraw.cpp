#include <MobitRenderer/draw.h>
#include <MobitRenderer/matrix.h>

#include <raylib.h>

void mr::draw_mtx_geo_type(GeoType type, int x, int y, float scale,
                           Color color) {
  switch (type) {
  case mr::GeoType::solid: {
    DrawRectangleRec(
        Rectangle{
            x * scale, y * scale, scale, scale},
        color);
  } break;

  case mr::GeoType::platform: {
    DrawRectangleRec(Rectangle{x * scale,
                               y * scale,
                               scale,
                               scale / 2.0f},
                     color);
  } break;

  case mr::GeoType::slope_ne: {
    float tx = x * scale;
    float ty = y * scale;

    float tsx = tx + scale;
    float tsy = ty + scale;

    DrawTriangle(Vector2{tx, ty}, Vector2{tsx, tsy},
                 Vector2{tx, tsy}, color);
  } break;

  case mr::GeoType::slope_nw: {
    float tx = x * scale;
    float ty = y * scale;

    float tsx = tx + scale;
    float tsy = ty + scale;

    DrawTriangle(Vector2{tsx, ty}, Vector2{tsx, tsy},
                 Vector2{tx, tsy}, color);
  } break;

  case mr::GeoType::slope_es: {
    float tx = x * scale;
    float ty = y * scale;

    float tsx = tx + scale;
    float tsy = ty + scale;

    DrawTriangle(Vector2{tx, ty}, Vector2{tsx, ty},
                 Vector2{tx, tsy}, color);
  } break;

  case mr::GeoType::slope_sw: {
    float tx = x * scale;
    float ty = y * scale;

    float tsx = tx + scale;
    float tsy = ty + scale;

    DrawTriangle(Vector2{tx, ty}, Vector2{tsx, ty},
                 Vector2{tsx, tsy}, color);
  } break;

  case mr::GeoType::air:
  case mr::GeoType::glass:
  case mr::GeoType::shortcut_entrance: {

  } break;
  }
}
