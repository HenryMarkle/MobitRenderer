#include <cstdint>
#include <raylib.h>
#include <rlgl.h>

#include <MobitRenderer/level.h>
#include <MobitRenderer/quad.h>

namespace mr {

namespace draw {

void draw_texture(const Texture2D *texture, const IQuad *quad) {
  rlSetTexture(texture->id);

  rlBegin(RL_QUADS);

  rlColor4ub(255, 255, 255, 255);

  bool flipx = quad->topleft.x > quad->topright.x && quad->bottomleft.x > quad->bottomright.x;
  bool flipy = quad->topleft.y > quad->bottomleft.y && quad->topright.y > quad->bottomright.y;
  

  int vtrx = flipx ? quad->topleft.x : quad->topright.x;
  int vtry = flipy ? quad->bottomright.y : quad->topright.y;

  int vtlx = flipx ? quad->topright.x : quad->topleft.x;
  int vtly = flipy ? quad->bottomleft.y : quad->topleft.y;

  int vblx = flipx ? quad->bottomright.x : quad->bottomleft.x;
  int vbly = flipy ? quad->topleft.y : quad->bottomleft.y;

  int vbrx = flipx ? quad->bottomleft.x : quad->bottomright.x;
  int vbry = flipy ? quad->topright.y : quad->bottomright.y;


  int ttrx = flipx ? 0 : 1.0f;
  int ttry = flipy ? 1.0f : 0;

  int ttlx = flipx ? 1.0f : 0;
  int ttly = flipy ? 1.0f : 0;

  int tblx = flipx ? 1.0f : 0;
  int tbly = flipy ? 0 : 1.0f;

  int tbrx = flipx ? 0 : 1.0f;
  int tbry = flipy ? 0 : 1.0f;


  // top right
  rlTexCoord2f(ttrx, ttry);
  rlVertex2i(vtrx, vtry);

  // top left
  rlTexCoord2f(ttlx, ttly);
  rlVertex2i(vtlx, vtly);
  
  // bottom left
  rlTexCoord2f(tblx, tbly);
  rlVertex2i(vblx, vbly);

  // bottom right
  rlTexCoord2f(tbrx, tbry);
  rlVertex2i(vbrx, vbry);


  // top right
  rlTexCoord2f(ttrx, ttry);
  rlVertex2i(vtrx, vtry);


  rlSetTexture(0);
}

void draw_texture(const Texture2D &texture, const IQuad &quad) {
  rlSetTexture(texture.id);

  rlBegin(RL_QUADS);

  rlColor4ub(255, 255, 255, 255);

  bool flipx = quad.topleft.x > quad.topright.x && quad.bottomleft.x > quad.bottomright.x;
  bool flipy = quad.topleft.y > quad.bottomleft.y && quad.topright.y > quad.bottomright.y;
  

  int vtrx = flipx ? quad.topleft.x : quad.topright.x;
  int vtry = flipy ? quad.bottomright.y : quad.topright.y;

  int vtlx = flipx ? quad.topright.x : quad.topleft.x;
  int vtly = flipy ? quad.bottomleft.y : quad.topleft.y;

  int vblx = flipx ? quad.bottomright.x : quad.bottomleft.x;
  int vbly = flipy ? quad.topleft.y : quad.bottomleft.y;

  int vbrx = flipx ? quad.bottomleft.x : quad.bottomright.x;
  int vbry = flipy ? quad.topright.y : quad.bottomright.y;


  int ttrx = flipx ? 0 : 1.0f;
  int ttry = flipy ? 1.0f : 0;

  int ttlx = flipx ? 1.0f : 0;
  int ttly = flipy ? 1.0f : 0;

  int tblx = flipx ? 1.0f : 0;
  int tbly = flipy ? 0 : 1.0f;

  int tbrx = flipx ? 0 : 1.0f;
  int tbry = flipy ? 0 : 1.0f;


  // top right
  rlTexCoord2f(ttrx, ttry);
  rlVertex2i(vtrx, vtry);

  // top left
  rlTexCoord2f(ttlx, ttly);
  rlVertex2i(vtlx, vtly);
  
  // bottom left
  rlTexCoord2f(tblx, tbly);
  rlVertex2i(vblx, vbly);

  // bottom right
  rlTexCoord2f(tbrx, tbry);
  rlVertex2i(vbrx, vbry);


  // top right
  rlTexCoord2f(ttrx, ttry);
  rlVertex2i(vtrx, vtry);


  rlSetTexture(0);
}

void draw_texture(const Texture2D *texture, const Quad *quad) {
  rlSetTexture(texture->id);

  rlBegin(RL_QUADS);

  rlColor4ub(255, 255, 255, 255);

  bool flipx = quad->topleft.x > quad->topright.x && quad->bottomleft.x > quad->bottomright.x;
  bool flipy = quad->topleft.y > quad->bottomleft.y && quad->topright.y > quad->bottomright.y;
  

  int vtrx = flipx ? quad->topleft.x : quad->topright.x;
  int vtry = flipy ? quad->bottomright.y : quad->topright.y;

  int vtlx = flipx ? quad->topright.x : quad->topleft.x;
  int vtly = flipy ? quad->bottomleft.y : quad->topleft.y;

  int vblx = flipx ? quad->bottomright.x : quad->bottomleft.x;
  int vbly = flipy ? quad->topleft.y : quad->bottomleft.y;

  int vbrx = flipx ? quad->bottomleft.x : quad->bottomright.x;
  int vbry = flipy ? quad->topright.y : quad->bottomright.y;


  int ttrx = flipx ? 0 : 1.0f;
  int ttry = flipy ? 1.0f : 0;

  int ttlx = flipx ? 1.0f : 0;
  int ttly = flipy ? 1.0f : 0;

  int tblx = flipx ? 1.0f : 0;
  int tbly = flipy ? 0 : 1.0f;

  int tbrx = flipx ? 0 : 1.0f;
  int tbry = flipy ? 0 : 1.0f;


  // top right
  rlTexCoord2f(ttrx, ttry);
  rlVertex2i(vtrx, vtry);

  // top left
  rlTexCoord2f(ttlx, ttly);
  rlVertex2i(vtlx, vtly);
  
  // bottom left
  rlTexCoord2f(tblx, tbly);
  rlVertex2i(vblx, vbly);

  // bottom right
  rlTexCoord2f(tbrx, tbry);
  rlVertex2i(vbrx, vbry);


  // top right
  rlTexCoord2f(ttrx, ttry);
  rlVertex2i(vtrx, vtry);


  rlSetTexture(0);
}

void draw_texture(const Texture2D &texture, const Quad &quad) {
  rlSetTexture(texture.id);

  rlBegin(RL_QUADS);

  rlColor4ub(255, 255, 255, 255);

  bool flipx = quad.topleft.x > quad.topright.x && quad.bottomleft.x > quad.bottomright.x;
  bool flipy = quad.topleft.y > quad.bottomleft.y && quad.topright.y > quad.bottomright.y;
  

  int vtrx = flipx ? quad.topleft.x : quad.topright.x;
  int vtry = flipy ? quad.bottomright.y : quad.topright.y;

  int vtlx = flipx ? quad.topright.x : quad.topleft.x;
  int vtly = flipy ? quad.bottomleft.y : quad.topleft.y;

  int vblx = flipx ? quad.bottomright.x : quad.bottomleft.x;
  int vbly = flipy ? quad.topleft.y : quad.bottomleft.y;

  int vbrx = flipx ? quad.bottomleft.x : quad.bottomright.x;
  int vbry = flipy ? quad.topright.y : quad.bottomright.y;


  int ttrx = flipx ? 0 : 1.0f;
  int ttry = flipy ? 1.0f : 0;

  int ttlx = flipx ? 1.0f : 0;
  int ttly = flipy ? 1.0f : 0;

  int tblx = flipx ? 1.0f : 0;
  int tbly = flipy ? 0 : 1.0f;

  int tbrx = flipx ? 0 : 1.0f;
  int tbry = flipy ? 0 : 1.0f;


  // top right
  rlTexCoord2f(ttrx, ttry);
  rlVertex2i(vtrx, vtry);

  // top left
  rlTexCoord2f(ttlx, ttly);
  rlVertex2i(vtlx, vtly);
  
  // bottom left
  rlTexCoord2f(tblx, tbly);
  rlVertex2i(vblx, vbly);

  // bottom right
  rlTexCoord2f(tbrx, tbry);
  rlVertex2i(vbrx, vbry);


  // top right
  rlTexCoord2f(ttrx, ttry);
  rlVertex2i(vtrx, vtry);


  rlSetTexture(0);
}

void draw_texture(const Texture2D &texture, const Quad &quad, Color color) {
    rlSetTexture(texture.id);

    rlBegin(RL_QUADS);

    rlColor4ub(color.r, color.g, color.b, color.a);

    bool flipx = quad.topleft.x > quad.topright.x && quad.bottomleft.x > quad.bottomright.x;
    bool flipy = quad.topleft.y > quad.bottomleft.y && quad.topright.y > quad.bottomright.y;
    

    int vtrx = flipx ? quad.topleft.x : quad.topright.x;
    int vtry = flipy ? quad.bottomright.y : quad.topright.y;

    int vtlx = flipx ? quad.topright.x : quad.topleft.x;
    int vtly = flipy ? quad.bottomleft.y : quad.topleft.y;

    int vblx = flipx ? quad.bottomright.x : quad.bottomleft.x;
    int vbly = flipy ? quad.topleft.y : quad.bottomleft.y;

    int vbrx = flipx ? quad.bottomleft.x : quad.bottomright.x;
    int vbry = flipy ? quad.topright.y : quad.bottomright.y;


    int ttrx = flipx ? 0 : 1.0f;
    int ttry = flipy ? 1.0f : 0;

    int ttlx = flipx ? 1.0f : 0;
    int ttly = flipy ? 1.0f : 0;

    int tblx = flipx ? 1.0f : 0;
    int tbly = flipy ? 0 : 1.0f;

    int tbrx = flipx ? 0 : 1.0f;
    int tbry = flipy ? 0 : 1.0f;


    // top right
    rlTexCoord2f(ttrx, ttry);
    rlVertex2i(vtrx, vtry);

    // top left
    rlTexCoord2f(ttlx, ttly);
    rlVertex2i(vtlx, vtly);
    
    // bottom left
    rlTexCoord2f(tblx, tbly);
    rlVertex2i(vblx, vbly);

    // bottom right
    rlTexCoord2f(tbrx, tbry);
    rlVertex2i(vbrx, vbry);


    // top right
    rlTexCoord2f(ttrx, ttry);
    rlVertex2i(vtrx, vtry);


    rlSetTexture(0);
}

void draw_grid(levelsize width, levelsize height, Color color, int scale, int thickness) noexcept {
    for (levelsize x = 1; x < width; x++) {
        float scaled = x * scale;
        DrawLineEx({scaled, 0}, {scaled, (float)height * scale}, thickness, color);
    }

    for (levelsize y = 1; y < height; y++) {
        float scaled = y * scale;
        DrawLineEx({0, scaled}, {(float)width * scale, scaled}, thickness, color);
    }
}

void draw_nested_grid(levelsize width, levelsize height, Color color, int scale, int thickness) noexcept {
    float thickened = thickness + 0.5f;
    
    for (levelsize x = 1; x < width; x++) {
        float scaled = x * scale;

        if (x % 2 == 0) {
            DrawLineEx({scaled, 0}, {scaled, (float)height * scale}, thickened, color);

        } else {
            DrawLineEx({scaled, 0}, {scaled, (float)height * scale}, thickness, color);
        }
    }

    for (levelsize y = 1; y < height; y++) {
        float scaled = y * scale;

        if (y % 2 == 0) {
            DrawLineEx({0, scaled}, {(float)width * scale, scaled}, thickened, color);
        } else {
            DrawLineEx({0, scaled}, {(float)width * scale, scaled}, thickness, color);
        }
    }
}

void draw_frame(levelpixelsize width, levelpixelsize height, Color color) noexcept {
    DrawRectangleLinesEx(
        Rectangle{-2, -2, width + 2.0f, height + 2.0f},
        6,
        color
    );
    DrawRectangleLinesEx(
        Rectangle{-2, -2, width + 2.0f, height + 2.0f},
        4,
        color
    );
}

void draw_double_frame(levelpixelsize width, levelpixelsize height) noexcept {
    DrawRectangleLinesEx(
        Rectangle{-2, -2, width + 2.0f, height + 2.0f},
        6,
        WHITE
    );
    DrawRectangleLinesEx(
        Rectangle{-2, -2, width + 2.0f, height + 2.0f},
        4,
        BLACK
    );
}

};
    
};