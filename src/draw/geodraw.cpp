#include <raylib.h>

#include <MobitRenderer/draw.h>
#include <MobitRenderer/matrix.h>

namespace mr {

void draw_mtx_geo_type(GeoCell cell, int x, int y, float scale,
                           Color color) {
  switch (cell.type) {
  case GeoType::solid: {
    DrawRectangleRec(
        Rectangle{
            x * scale, y * scale, scale, scale},
        color);
  } break;

  case GeoType::platform: {
    DrawRectangleRec(Rectangle{x * scale,
                               y * scale,
                               scale,
                               scale / 2.0f},
                     color);
  } break;

  case GeoType::slope_ne: {
    float tx = x * scale;
    float ty = y * scale;

    float tsx = tx + scale;
    float tsy = ty + scale;

    DrawTriangle(Vector2{tx, ty}, Vector2{tsx, tsy},
                 Vector2{tx, tsy}, color);
  } break;

  case GeoType::slope_nw: {
    float tx = x * scale;
    float ty = y * scale;

    float tsx = tx + scale;
    float tsy = ty + scale;

    DrawTriangle(Vector2{tsx, ty}, Vector2{tsx, tsy},
                 Vector2{tx, tsy}, color);
  } break;

  case GeoType::slope_es: {
    float tx = x * scale;
    float ty = y * scale;

    float tsx = tx + scale;
    float tsy = ty + scale;

    DrawTriangle(Vector2{tx, ty}, Vector2{tsx, ty},
                 Vector2{tx, tsy}, color);
  } break;

  case GeoType::slope_sw: {
    float tx = x * scale;
    float ty = y * scale;

    float tsx = tx + scale;
    float tsy = ty + scale;

    DrawTriangle(Vector2{tx, ty}, Vector2{tsx, ty},
                 Vector2{tsx, tsy}, color);
  } break;

  case GeoType::air: break;

  case GeoType::glass: {
    auto ninth = scale / 3.0f;

    DrawRectangleRec(
      Rectangle{
        x * scale, 
        y * scale, 
        ninth, 
        ninth
      }, 
      color
    );

    DrawRectangleRec(
      Rectangle{
        x * scale + ninth * 2, 
        y * scale, 
        ninth, 
        ninth
      }, 
      color
    );

    DrawRectangleRec(
      Rectangle{
        x * scale + ninth, 
        y * scale + ninth, 
        ninth, 
        ninth
      }, 
      color
    );

    DrawRectangleRec(
      Rectangle{
        x * scale, 
        y * scale + ninth * 2, 
        ninth, 
        ninth
      }, 
      color
    );

    DrawRectangleRec(
      Rectangle{
        x * scale + ninth * 2, 
        y * scale + ninth * 2, 
        ninth, 
        ninth
      }, 
      color
    );

    break;
  }
  case GeoType::shortcut_entrance: {
    auto ninth = scale / 3.0f;

    DrawRectangleRec(
      Rectangle{
        x * scale + ninth, 
        y * scale, 
        ninth, 
        ninth
      }, 
      color
    );

    DrawRectangleRec(
      Rectangle{
        x * scale, 
        y * scale + ninth, 
        ninth, 
        ninth
      }, 
      color
    );

    DrawRectangleRec(
      Rectangle{
        x * scale + ninth * 2, 
        y * scale + ninth, 
        ninth, 
        ninth
      }, 
      color
    );

    DrawRectangleRec(
      Rectangle{
        x * scale + ninth, 
        y * scale + ninth * 2, 
        ninth, 
        ninth
      }, 
      color
    );
  } break;
  }
}

void draw_mtx_geo_features(GeoCell cell, int x, int y, float scale,
  Color color) {
  
  if (cell.has_feature(GeoFeature::shortcut_path)) {
    const float hole_thick_ratio = 4 * scale/20;
    const float hole_begin_ratio = 8 * scale/20;
    
    DrawRectangle(
      x * scale + hole_begin_ratio, 
      y * scale + hole_begin_ratio, 
      hole_thick_ratio, 
      hole_thick_ratio,
      color
    );
  }
}

void draw_mtx_geo_poles(
  GeoCell cell, 
  int x, int y, float scale,
  Color color) {

  const float pole_thick_ratio = 4 * scale/20;
  const float pole_begin_ratio = 8 * scale/20;

  if (cell.has_feature(GeoFeature::vertical_pole)) {
    DrawRectangle(
      x *scale + pole_begin_ratio, y * scale, pole_thick_ratio, scale, 
      color
    );
  }

  if (cell.has_feature(GeoFeature::horizontal_pole)) {
    DrawRectangle(
      x *scale, y * scale + pole_begin_ratio, scale, pole_thick_ratio,
      color
    );
  }
}

void draw_mtx_shortcut_entrances(
  Matrix<GeoCell> const &matrix,
  uint16_t x,
  uint16_t y,
  float scale, 
  Color color
) {
  const GeoCell *cell = matrix.get_const_ptr(x, y, 0);
  if (!cell) return;
  if (!cell->has_feature(GeoFeature::shortcut_entrance)) return;

  // TODO: Complete this
}

};
