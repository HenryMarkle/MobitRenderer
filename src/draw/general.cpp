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


  float ttrx = flipx ? 0 : 1.0f;
  float ttry = flipy ? 1.0f : 0;

  float ttlx = flipx ? 1.0f : 0;
  float ttly = flipy ? 1.0f : 0;

  float tblx = flipx ? 1.0f : 0;
  float tbly = flipy ? 0 : 1.0f;

  float tbrx = flipx ? 0 : 1.0f;
  float tbry = flipy ? 0 : 1.0f;


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

  rlEnd();

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


  float ttrx = flipx ? 0 : 1.0f;
  float ttry = flipy ? 1.0f : 0;

  float ttlx = flipx ? 1.0f : 0;
  float ttly = flipy ? 1.0f : 0;

  float tblx = flipx ? 1.0f : 0;
  float tbly = flipy ? 0 : 1.0f;

  float tbrx = flipx ? 0 : 1.0f;
  float tbry = flipy ? 0 : 1.0f;


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

  rlEnd();

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


  float ttrx = flipx ? 0 : 1.0f;
  float ttry = flipy ? 1.0f : 0;

  float ttlx = flipx ? 1.0f : 0;
  float ttly = flipy ? 1.0f : 0;

  float tblx = flipx ? 1.0f : 0;
  float tbly = flipy ? 0 : 1.0f;

  float tbrx = flipx ? 0 : 1.0f;
  float tbry = flipy ? 0 : 1.0f;


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

  rlEnd();

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


  float ttrx = flipx ? 0 : 1.0f;
  float ttry = flipy ? 1.0f : 0;

  float ttlx = flipx ? 1.0f : 0;
  float ttly = flipy ? 1.0f : 0;

  float tblx = flipx ? 1.0f : 0;
  float tbly = flipy ? 0 : 1.0f;

  float tbrx = flipx ? 0 : 1.0f;
  float tbry = flipy ? 0 : 1.0f;


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

  rlEnd();

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


    float ttrx = flipx ? 0 : 1.0f;
    float ttry = flipy ? 1.0f : 0;

    float ttlx = flipx ? 1.0f : 0;
    float ttly = flipy ? 1.0f : 0;

    float tblx = flipx ? 1.0f : 0;
    float tbly = flipy ? 0 : 1.0f;

    float tbrx = flipx ? 0 : 1.0f;
    float tbry = flipy ? 0 : 1.0f;


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

    rlEnd();

    rlSetTexture(0);
}

void draw_texture(
  const Texture2D &texture, 
  const Rectangle &src, 
  const Quad &quad
) noexcept {
    bool flipx = quad.topleft.x > quad.topright.x && quad.bottomleft.x > quad.bottomright.x;
    bool flipy = quad.topleft.y > quad.bottomleft.y && quad.topright.y > quad.bottomright.y;
    

    // int vtrx = flipx ? quad.topleft.x : quad.topright.x;
    // int vtry = flipy ? quad.bottomright.y : quad.topright.y;

    // int vtlx = flipx ? quad.topright.x : quad.topleft.x;
    // int vtly = flipy ? quad.bottomleft.y : quad.topleft.y;

    // int vblx = flipx ? quad.bottomright.x : quad.bottomleft.x;
    // int vbly = flipy ? quad.topleft.y : quad.bottomleft.y;

    // int vbrx = flipx ? quad.bottomleft.x : quad.bottomright.x;
    // int vbry = flipy ? quad.topright.y : quad.bottomright.y;

    int vtrx = quad.topright.x;
    int vtry = quad.topright.y;

    int vtlx = quad.topleft.x;
    int vtly = quad.topleft.y;

    int vblx = quad.bottomleft.x;
    int vbly = quad.bottomleft.y;

    int vbrx = quad.bottomright.x;
    int vbry = quad.bottomright.y;

    float left_v   =  src.x                 / texture.width;
    float right_v  = (src.x + src.width)    / texture.width;
    float top_v    =  src.y                 / texture.height;
    float bottom_v = (src.y + src.height)   / texture.height;

    if (flipx) std::swap(left_v, right_v);
    if (flipy) std::swap(top_v, bottom_v);


    rlSetTexture(texture.id);

    rlBegin(RL_QUADS);

    rlColor4ub(255, 255, 255, 255);

    // top right
    rlTexCoord2f(right_v, top_v);
    rlVertex2f(vtrx, vtry);

    // top left
    rlTexCoord2f(left_v, top_v);
    rlVertex2f(vtlx, vtly);
    
    // bottom left
    rlTexCoord2f(left_v, bottom_v);
    rlVertex2f(vblx, vbly);

    // bottom right
    rlTexCoord2f(right_v, bottom_v);
    rlVertex2f(vbrx, vbry);

    rlEnd();
    
    rlSetTexture(0);
}

void draw_texture(
  const Texture2D &texture, 
  const Rectangle &src, 
  const Quad &quad, 
  Color color
) noexcept {
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


    float topright_vx = (src.x + src.width) / texture.width;
    float topright_vy = (src.y)             / texture.height;

    float topleft_vx = (src.x) / texture.width;
    float topleft_vy = (src.y) / texture.height;

    float bottomleft_vx = (src.x)              / texture.width;
    float bottomleft_vy = (src.y + src.height) / texture.height;

    float bottomright_vx = (src.x + src.width)  / texture.width;
    float bottomright_vy = (src.y + src.height) / texture.height;


    float ttrx = flipx ? topleft_vx     : topright_vx;
    float ttry = flipy ? bottomright_vy : topright_vy;

    float ttlx = flipx ? topright_vx   : topleft_vx;
    float ttly = flipy ? bottomleft_vy : topleft_vy;

    float tblx = flipx ? bottomright_vx : bottomleft_vx;
    float tbly = flipy ? topleft_vy     : bottomleft_vy;

    float tbrx = flipx ? bottomleft_vx : bottomright_vx;
    float tbry = flipy ? topright_vy   : bottomright_vy;


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

    rlEnd();

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

void draw_double_frame(int x, int y, levelpixelsize width, levelpixelsize height) noexcept {
    DrawRectangleLinesEx(
        Rectangle{ x - 2.0f, y - 2.0f, width + 2.0f, height + 2.0f},
        6,
        WHITE
    );
    DrawRectangleLinesEx(
        Rectangle{x - 2.0f, y - 2.0f, width + 2.0f, height + 2.0f},
        4,
        BLACK
    );
}

void draw_ruler(int x, int y, int width, int height, Color color, float scale) noexcept {
    DrawLineEx(
        Vector2 { 0, y * scale },
        Vector2 { width * scale, y * scale },
        3,
        color
    );

    DrawLineEx(
        Vector2 { 0, y * scale + scale },
        Vector2 { width * scale, y * scale + scale },
        3,
        color
    );

    DrawLineEx(
        Vector2 { x * scale, 0 },
        Vector2 { x * scale, height * scale },
        3,
        color
    );

    DrawLineEx(
        Vector2 { x * scale + scale, 0 },
        Vector2 { x * scale + scale, height * scale },
        3,
        color
    );
}

void draw_texture_triangle(
    const Texture2D &texture,
    const Vector2 &p1,
    const Vector2 &p2,
    const Vector2 &p3
) noexcept {
    Vector2 uv1 = { 0.0f, 0.0f };
    Vector2 uv2 = { 0.0f, 1.0f };
    Vector2 uv3 = { 1.0f, 1.0f };

    rlSetTexture(texture.id);

    rlBegin(RL_TRIANGLES);
  
    rlColor4ub(255, 255, 255, 255);

    rlTexCoord2f(uv1.x, uv1.y); rlVertex2f(p1.x, p1.y);
    rlTexCoord2f(uv2.x, uv2.y); rlVertex2f(p2.x, p2.y);
    rlTexCoord2f(uv3.x, uv3.y); rlVertex2f(p3.x, p3.y);

    rlEnd();

    rlSetTexture(0);
}

};
    
};