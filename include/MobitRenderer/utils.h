#pragma once

#include <raylib.h>
#include <raymath.h>

inline Rectangle operator+(const Rectangle &r, const Vector2 &v) { return Rectangle{ r.x + v.x, r.y + v.y, r.width, r.height }; }
inline Rectangle operator-(const Rectangle &r, const Vector2 &v) { return Rectangle{ r.x - v.x, r.y - v.y, r.width, r.height }; }
inline Rectangle operator+(const Rectangle &r1, const Rectangle &r2) { return Rectangle{ r1.x + r2.x, r1.y + r2.y, r1.width + r2.width, r1.height + r2.height }; }
inline Rectangle operator-(const Rectangle &r1, const Rectangle &r2) { return Rectangle{ r1.x - r2.x, r1.y - r2.y, r1.width - r2.width, r1.height - r2.height }; }
inline Rectangle operator*(const Rectangle &r, const int &i) { return Rectangle{ r.x * i, r.y * i, r.width * i, r.height * i }; }
inline Rectangle operator/(const Rectangle &r, const int &i) { return Rectangle{ r.x / i, r.y / i, r.width / i, r.height / i }; }
inline Rectangle operator*(const Rectangle &r, const float &i) { return Rectangle{ r.x * i, r.y * i, r.width * i, r.height * i }; }
inline Rectangle operator/(const Rectangle &r, const float &i) { return Rectangle{ r.x / i, r.y / i, r.width / i, r.height / i }; }

inline Vector2 operator+(const Vector2 &v1, const Vector2 &v2) { return Vector2Add(v1, v2); }
inline Vector2 operator-(const Vector2 &v1, const Vector2 &v2) { return Vector2Subtract(v1, v2); }
inline Vector2 operator*(const Vector2 &v, const int &scale) { return Vector2{ v.x * scale, v.y * scale }; }
inline Vector2 operator*(const Vector2 &v, const float &scale) { return Vector2Scale(v, scale); }
inline Vector2 operator/(const Vector2 &v, const int &scale) { return Vector2{ v.x * scale, v.y / scale }; }
inline Vector2 operator/(const Vector2 &v, const float &scale) { return Vector2{ v.x * scale, v.y / scale }; }

namespace mr::utils {

void unload_shader(Shader&);
void unload_texture(Texture2D&);

};