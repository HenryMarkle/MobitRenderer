#include <raylib.h>

#include <MobitRenderer/atlas.h>
#include <MobitRenderer/draw.h>
#include <MobitRenderer/matrix.h>

namespace mr {

namespace draw {



void draw_mtx_geo_type(
  GeoCell cell, 
  int x, 
  int y, 
  float scale,
  Color color
) {
  switch (cell.type) {
  case GeoType::solid: {
    DrawRectangleRec(
        Rectangle{
            x * scale, 
            y * scale, 
            scale, 
            scale
        },
        color
    );
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
  Color color, const GE_Textures &atlas) {
  
  Rectangle target = { x * scale, y * scale, scale, scale };
  
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

  Texture2D texture;



  if (cell.has_feature(GeoFeature::bathive)) {
    texture = atlas.bathive().get();
    DrawTexturePro(
      texture,
      Rectangle{0, 0, (float)texture.width, (float)texture.height},
      target,
      {0, 0},
      0,
      color
    );
  }
  if (cell.has_feature(GeoFeature::forbid_fly_chains)) {
    texture = atlas.forbid().get();
    DrawTexturePro(
      texture,
      Rectangle{0, 0, (float)texture.width, (float)texture.height},
      target,
      {0, 0},
      0,
      color
    );
  }
  if (cell.has_feature(GeoFeature::worm_grass)) {
    texture = atlas.wormgrass().get();
    DrawTexturePro(
      texture,
      Rectangle{0, 0, (float)texture.width, (float)texture.height},
      target,
      {0, 0},
      0,
      color
    );
  }
  if (cell.has_feature(GeoFeature::place_rock)) {
    texture = atlas.rock().get();
    DrawTexturePro(
      texture,
      Rectangle{0, 0, (float)texture.width, (float)texture.height},
      target,
      {0, 0},
      0,
      color
    );
  }
  if (cell.has_feature(GeoFeature::place_spear)) {
    texture = atlas.spear().get();
    DrawTexturePro(
      texture,
      Rectangle{0, 0, (float)texture.width, (float)texture.height},
      target,
      {0, 0},
      0,
      color
    );
  }
  if (cell.has_feature(GeoFeature::waterfall)) {
    texture = atlas.waterfall().get();
    DrawTexturePro(
      texture,
      Rectangle{0, 0, (float)texture.width, (float)texture.height},
      target,
      {0, 0},
      0,
      color
    );
  }

  if (cell.has_feature(GeoFeature::room_entrance)) {
    texture = atlas.passage().get();
    DrawTexturePro(
      texture,
      Rectangle{0, 0, (float)texture.width, (float)texture.height},
      target,
      {0, 0},
      0,
      color
    );
  }
  if (cell.has_feature(GeoFeature::garbage_worm_hole)) {
    texture = atlas.garbageworm().get();
    DrawTexturePro(
      texture,
      Rectangle{0, 0, (float)texture.width, (float)texture.height},
      target,
      {0, 0},
      0,
      color
    );
  }
  if (cell.has_feature(GeoFeature::scavenger_hole)) {
    texture = atlas.scav().get();
    DrawTexturePro(
      texture,
      Rectangle{0, 0, (float)texture.width, (float)texture.height},
      target,
      {0, 0},
      0,
      color
    );
  }
  if (cell.has_feature(GeoFeature::dragon_den)) {
    texture = atlas.den().get();
    DrawTexturePro(
      texture,
      Rectangle{0, 0, (float)texture.width, (float)texture.height},
      target,
      {0, 0},
      0,
      color
    );
  }
  if (cell.has_feature(GeoFeature::wack_a_mole_hole)) {
    texture = atlas.wack().get();
    DrawTexturePro(
      texture,
      Rectangle{0, 0, (float)texture.width, (float)texture.height},
      target,
      {0, 0},
      0,
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

void draw_geo_layer(
  Matrix<GeoCell> const& matrix, 
  uint8_t layer, 
  Color color,
  float scale
) {
  if (layer > 2) return;
  if (color.a == 0) return;

  for (uint16_t x = 0; x < matrix.get_width(); x++) {
    for (uint16_t y = 0; y < matrix.get_height(); y++) {
      auto cell1 = matrix.get_copy(x, y, layer);

      draw_mtx_geo_type(cell1, x, y, scale, color);
    }
  }
}

void draw_geo_layer(
  Matrix<GeoCell> const* matrix, 
  uint8_t layer, 
  Color color,
  float scale
) {
  if (matrix == nullptr) return;
  if (layer > 2) return;
  if (color.a == 0) return;

  for (uint16_t x = 0; x < matrix->get_width(); x++) {
    for (uint16_t y = 0; y < matrix->get_height(); y++) {
      auto cell1 = matrix->get_copy(x, y, layer);

      draw_mtx_geo_type(cell1, x, y, scale, color);
    }
  }
}

void draw_geo_and_poles_layer(
  Matrix<GeoCell> const& matrix, 
  uint8_t layer,
  Color color,
  float scale
) {
  if (layer > 2) return;
  if (color.a == 0) return;

  for (uint16_t x = 0; x < matrix.get_width(); x++) {
    for (uint16_t y = 0; y < matrix.get_height(); y++) {
      auto cell1 = matrix.get_copy(x, y, layer);

      draw_mtx_geo_type(cell1, x, y, scale, color);
      draw_mtx_geo_poles(cell1, x, y, scale, color);
    }
  }
}

void draw_geo_features_layer(
  Matrix<GeoCell> const& matrix, 
  const GE_Textures &atlas,
  uint8_t layer,
  Color color, 
  float scale
) {
  if (layer > 2) return;
  if (color.a == 0) return;

  for (uint16_t x = 0; x < matrix.get_width(); x++) {
    for (uint16_t y = 0; y < matrix.get_height(); y++) {
      auto cell1 = matrix.get_copy(x, y, 0);

      draw_mtx_geo_features(cell1, x, y, 20, BLACK, atlas);
    }
  }
}

};

};
