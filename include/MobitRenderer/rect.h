#pragma once

#include <raylib.h>

namespace mr {

struct IRect {

int x, y, width, height;

inline Vector2 position() const noexcept { 
    return Vector2 { 
        static_cast<float>(x), 
        static_cast<float>(y) 
    }; 
}

inline Rectangle operator=(IRect r) const noexcept { 
    return Rectangle {
        static_cast<float>(x), 
        static_cast<float>(y),
        static_cast<float>(width),
        static_cast<float>(height),
    }; 
}

inline IRect operator+(Vector2 v) const noexcept {
    IRect rect = IRect(x, y, width, height);

    rect.x += static_cast<int>(v.x);
    rect.y += static_cast<int>(v.y);

    return rect;
}

inline IRect operator-(Vector2 v) const noexcept {
    IRect rect = IRect(x, y, width, height);

    rect.x -= static_cast<int>(v.x);
    rect.y -= static_cast<int>(v.y);

    return rect;
}

inline IRect(int x, int y, int width, int height)
    : x(x), y(y), width(width), height(height) 
{}

};

};