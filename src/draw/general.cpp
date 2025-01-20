#include <cstdint>
#include <raylib.h>

#include <MobitRenderer/level.h>

namespace mr {

namespace draw {



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