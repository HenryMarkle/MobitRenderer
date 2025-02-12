#include <raylib.h>

#include <MobitRenderer/atlas.h>
#include <MobitRenderer/draw.h>
#include <MobitRenderer/matrix.h>

namespace mr {

namespace draw {

void draw_geo_shape(
  GeoType type, 
  float x, 
  float y, 
  float scale, 
  Color color
) {
  switch (type) {
    case GeoType::solid:
    DrawRectangleRec(
      Rectangle{x, y, scale, scale},
      color
    );
    break;

    case GeoType::platform:
    DrawRectangleRec(
      Rectangle{x, y, scale, scale/2.0f},
      color
    );
    break;

    case GeoType::slope_es:
    {
      auto tx = x + scale;
      auto ty = y + scale;

      DrawTriangle(
        Vector2{ x ,  y },
        Vector2{ tx,  y },
        Vector2{ x , ty },
        color
      );
    }
    break;

    case GeoType::slope_ne:
    {
      auto tx = x + scale;
      auto ty = y + scale;

      DrawTriangle(
        Vector2{ x ,  y },
        Vector2{ tx, ty },
        Vector2{ x , ty },
        color
      );
    }
    break;

    case GeoType::slope_nw:
    {
      auto tx = x + scale;
      auto ty = y + scale;

      DrawTriangle(
        Vector2{ tx,  y },
        Vector2{ tx, ty },
        Vector2{ x , ty },
        color
      );
    }
    break;

    case GeoType::slope_sw:
    {
      auto tx = x + scale;
      auto ty = y + scale;

      DrawTriangle(
        Vector2{ x ,  y },
        Vector2{ tx, y },
        Vector2{ tx, ty },
        color
      );
    }
    break;
  }
}

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
    texture = atlas.bathive();
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
    texture = atlas.forbid();
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
    texture = atlas.wormgrass();
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
    texture = atlas.rock();
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
    texture = atlas.spear();
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
    texture = atlas.waterfall();
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
    texture = atlas.passage();
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
    texture = atlas.garbageworm();
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
    texture = atlas.scav();
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
    texture = atlas.den();
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
    texture = atlas.wack();
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

void draw_geo_entrances(Matrix<GeoCell> const &matrix, GE_Textures &atlas, Color color, float scale) {
  uint8_t holes, dots;
  size_t connx, conny;
  const auto &loose_texture = atlas.entry_loose();

  for (size_t x = 0; x < matrix.get_width(); x++) {
    for (size_t y = 0; y < matrix.get_height(); y++) {
      const auto &cell = matrix.get_const(x, y, 0);

      if (!cell.has_feature(GeoFeature::shortcut_entrance)) continue;
    
      // Cell must only have the entrance feature.
      if (cell.features != GeoFeature::shortcut_entrance) goto disconnected;
      // Cell ID must be 7.
      if (cell.type != GeoType::shortcut_entrance) goto disconnected;

      // Cell must only have one opening and connect to one path.
      holes = dots = connx = conny = 0;
      for (int xx = -1; xx < 2; xx++) {
        for (int yy = -1; yy < 2; yy++) {

          // Ignore the middle cell.
          if (xx == 0 && yy == 0) continue;

          // The cell must not be out of bounds.
          const auto *neighbor = matrix.get_const_ptr(x + xx, y + yy, 0);
          if (neighbor == nullptr) goto disconnected;
        
          // The cell must have exactly one hole.
          if (neighbor->type != GeoType::solid && ++holes > 1) goto disconnected;

          if ( 
              neighbor->has_feature(GeoFeature::shortcut_path) ||
              neighbor->has_feature(GeoFeature::dragon_den) ||
              neighbor->has_feature(GeoFeature::scavenger_hole) ||
              neighbor->has_feature(GeoFeature::room_entrance) ||
              neighbor->has_feature(GeoFeature::wack_a_mole_hole)
          ) {
            if (
              ( 
                (xx ==  0 && yy == -1) || 
                (xx == -1 && yy ==  0) ||
                (xx ==  1 && yy ==  0) ||
                (xx ==  0 && yy ==  1)
              )
            ) {
              if (++dots > 1) goto disconnected;
            
              // Track the direction of the connection.
              connx = xx;
              conny = yy;            
            } else goto disconnected;
          }
        }
      }

      if (holes <= 0) goto disconnected;

      if (dots == 1) {

        // Success
        if (connx ==  0 && conny == -1) { // top
          const auto &texture = atlas.entry_top();
          DrawTexturePro(
            texture,
            Rectangle{0, 0, static_cast<float>(loose_texture.width), static_cast<float>(loose_texture.height)},
            Rectangle{x * scale, y * scale, scale, scale},
            Vector2{0, 0},
            0,
            color
          );
        } else if (connx == -1 && conny ==  0) { // left
          const auto &texture = atlas.entry_left();
          DrawTexturePro(
            texture,
            Rectangle{0, 0, static_cast<float>(loose_texture.width), static_cast<float>(loose_texture.height)},
            Rectangle{x * scale, y * scale, scale, scale},
            Vector2{0, 0},
            0,
            color
          );
        } else if (connx ==  1 && conny ==  0) { // right
          const auto &texture = atlas.entry_right();
          DrawTexturePro(
            texture,
            Rectangle{0, 0, static_cast<float>(loose_texture.width), static_cast<float>(loose_texture.height)},
            Rectangle{x * scale, y * scale, scale, scale},
            Vector2{0, 0},
            0,
            color
          );
        } else if (connx ==  0 && conny ==  1) { // bottom
          const auto &texture = atlas.entry_bottom();
          DrawTexturePro(
            texture,
            Rectangle{0, 0, static_cast<float>(loose_texture.width), static_cast<float>(loose_texture.height)},
            Rectangle{x * scale, y * scale, scale, scale},
            Vector2{0, 0},
            0,
            color
          );
        } else goto disconnected;

        continue;
      }

      disconnected:
      {
        DrawTexturePro(
          loose_texture,
          Rectangle{0, 0, static_cast<float>(loose_texture.width), static_cast<float>(loose_texture.height)},
          Rectangle{x * scale, y * scale, scale, scale},
          Vector2{0, 0},
          0,
          color
        );

        continue;
      }
    }
  }
}

};

};
