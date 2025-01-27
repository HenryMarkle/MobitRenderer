#pragma once

#include <raylib.h>

namespace mr {

typedef struct IVector2 {
  int x, y;

  inline IVector2 operator+(IVector2 v) const noexcept { return IVector2{ x + v.x, y + v.y }; }
  inline IVector2 operator+(Vector2  v) const noexcept { return IVector2{ x + (int)v.x, y + (int)v.y }; }
  
  inline IVector2 operator-(IVector2 v) const noexcept { return IVector2{ x - v.x, y - v.y }; }
  inline IVector2 operator-(Vector2  v) const noexcept { return IVector2{ x - (int)v.x, y - (int)v.y }; }
  
  inline IVector2 operator*(int i) const noexcept { return IVector2{ x * i, y * i }; }
  inline IVector2 operator/(int i) const noexcept { return IVector2{ x / i, y / i }; }
  inline IVector2 operator%(int i) const noexcept { return IVector2{ x % i, y % i }; }

} ivec2;

};