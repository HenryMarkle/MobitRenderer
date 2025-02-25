#pragma once

#include <cstdint>
#include <stdexcept>

#include <raylib.h>
#include <raymath.h>

#include <MobitRenderer/vec.h>

namespace mr {

struct IQuad {
    IVector2 topright, topleft, bottomleft, bottomright;

    IQuad();
    IQuad(IVector2, IVector2, IVector2, IVector2);
    IQuad(Rectangle);
  
    inline IQuad operator+(IVector2 v) const noexcept {
        return IQuad(
            topleft + v, 
            topright + v, 
            bottomright + v, 
            bottomleft + v
        );
    }
    inline IQuad operator+(Vector2 v) const noexcept {
        return IQuad(
            topleft + v, 
            topright + v, 
            bottomright + v, 
            bottomleft + v
        );
    }
  
    inline IQuad operator-(IVector2 v) const noexcept {
        return IQuad(
            topleft - v, 
            topright - v, 
            bottomright - v, 
            bottomleft - v
        );
    }
    inline IQuad operator-(Vector2 v) const noexcept {
        return IQuad(
            topleft - v, 
            topright - v, 
            bottomright - v, 
            bottomleft - v
        );
    }

    /// @brief Returns a vertex depends on an index, which is
    /// given the order: top left, top right, bottom right, bottom left.
    /// @throws std::out_of_range if the index is larger than 3.
    inline IVector2 &operator[](uint8_t i) {
        if (i == 0) return topleft;
        if (i == 1) return topright;
        if (i == 2) return bottomright;
        if (i == 3) return bottomleft;

        throw std::out_of_range("IQuad index cannot be larger than 3");
    }

    /// @brief Returns a vertex depends on an index, which is
    /// given the order: top left, top right, bottom right, bottom left.
    /// @throws std::out_of_range if the index is negative or larger than 3.
    inline IVector2 &operator[](int i) {
        if (i == 0) return topleft;
        if (i == 1) return topright;
        if (i == 2) return bottomright;
        if (i == 3) return bottomleft;

        throw std::out_of_range("IQuad index cannot be negative or larger than 3");
    }

    /// @brief Returns the center point of the quad.
    inline IVector2 center() const noexcept { return (topleft + topright + bottomright + bottomleft) / 4; }

    /// @brief Returns a quad rotated around 
    /// a point by a given angle degree, 
    /// without modifying the current struct.
    IQuad rotated_around(float degrees, IVector2 point) const noexcept;

    /// @brief Rotates the current quad around 
    /// a point by a given angle degree, 
    /// without modifying the current struct.
    void rotate_around(float degrees, IVector2 point) noexcept;

    /// @brief Returns a rotated quad by a given angle degree, 
    /// without modifying the current struct.
    inline IQuad rotated(float degrees) const noexcept { return rotated_around(degrees, center()); }
    
    /// @brief Rotates the current quad 
    /// by a given angle degree.
    void rotate(float degrees) noexcept { rotate_around(degrees, center()); }

    /// @brief Returns a rectangle that encloses
    /// all the vertices.
    Rectangle enclose() const noexcept;
};

struct Quad {
    Vector2 topright, topleft, bottomleft, bottomright;

    Quad();
    Quad(Vector2, Vector2, Vector2, Vector2);
    Quad(Rectangle);

    inline Quad operator+(IVector2 v) const noexcept {
        return Quad(
            Vector2 { topleft.x + v.x, topleft.y + v.y }, 
            Vector2 { topright.x + v.x, topright.y + v.y }, 
            Vector2 { bottomright.x + v.x, bottomright.y + v.y }, 
            Vector2 { bottomleft.x + v.x, bottomleft.y + v.y }
        );
    }
    inline Quad &operator+=(IVector2 v) noexcept {
        topleft.x += v.x; topleft.y += v.y;
        topright.x += v.x; topright.y += v.y;
        bottomright.x += v.x; bottomright.y += v.y;
        bottomleft.x += v.x; bottomleft.y += v.y;

        return *this;
    }

    inline Quad operator+(Vector2 v) const noexcept {
        return Quad(
            Vector2Add(topleft, v),
            Vector2Add(topright, v),
            Vector2Add(bottomright, v),
            Vector2Add(bottomleft, v)
        );
    }
    inline Quad &operator+=(Vector2 v) noexcept {
        topleft += v;
        topright += v;
        bottomright += v;
        bottomleft += v;

        return *this;
    }
  
    inline Quad operator-(IVector2 v) const noexcept {
        return Quad(
            Vector2 { topleft.x - v.x, topleft.y - v.y }, 
            Vector2 { topright.x - v.x, topright.y - v.y }, 
            Vector2 { bottomright.x - v.x, bottomright.y - v.y }, 
            Vector2 { bottomleft.x - v.x, bottomleft.y - v.y }
        );
    }
    inline Quad &operator-=(IVector2 v) noexcept {
        topleft.x -= v.x; topleft.y -= v.y;
        topright.x -= v.x; topright.y -= v.y;
        bottomright.x -= v.x; bottomright.y -= v.y;
        bottomleft.x -= v.x; bottomleft.y -= v.y;

        return *this;
    }

    inline Quad operator-(Vector2 v) const noexcept {
        return Quad(
            Vector2Subtract(topleft, v),
            Vector2Subtract(topright, v),
            Vector2Subtract(bottomright, v),
            Vector2Subtract(bottomleft, v)
        );
    }
    inline Quad &operator-=(Vector2 v) noexcept {
        topleft -= v;
        topright -= v;
        bottomright -= v;
        bottomleft -= v;

        return *this;
    }

    inline Quad operator*(float scale) const noexcept {
        return Quad(
            topleft * scale,
            topright * scale,
            bottomright * scale,
            bottomleft * scale
        );
    }
    inline Quad &operator*=(float scale) noexcept {
        topleft *= scale;
        topright *= scale;
        bottomright *= scale;
        bottomleft *= scale;

        return *this;
    }

    inline Quad operator/(float scale) const noexcept {
        return Quad(
            topleft / scale,
            topright / scale,
            bottomright / scale,
            bottomleft / scale
        );
    }
    inline Quad &operator/=(float scale) noexcept {
        topleft /= scale;
        topright /= scale;
        bottomright /= scale;
        bottomleft /= scale;

        return *this;
    }

    inline Quad operator+(Quad q) noexcept {
        return Quad(
            topleft + q.topleft,
            topright + q.topright,
            bottomright + q.bottomright,
            bottomleft + q.bottomleft
        );
    }
    inline Quad operator-(Quad q) noexcept {
        return Quad(
            topleft - q.topleft,
            topright - q.topright,
            bottomright - q.bottomright,
            bottomleft - q.bottomleft
        );
    }

    inline Quad &operator+=(Quad q) noexcept {
        topleft += q.topleft;
        topright += q.topright;
        bottomright += q.bottomright;
        bottomleft += q.bottomleft;
        
        return *this;
    }
    inline Quad &operator-=(Quad q) noexcept {
        topleft -= q.topleft;
        topright -= q.topright;
        bottomright -= q.bottomright;
        bottomleft -= q.bottomleft;
        
        return *this;
    }

    /// @brief Returns a vertex depends on an index, which is
    /// given the order: top left, top right, bottom right, bottom left.
    /// @throws std::out_of_range if the index is larger than 3.
    inline Vector2 &operator[](uint8_t i) {
        if (i == 0) return topleft;
        if (i == 1) return topright;
        if (i == 2) return bottomright;
        if (i == 3) return bottomleft;

        throw std::out_of_range("Quad index cannot be larger than 3");
    }

    /// @brief Returns a vertex depends on an index, which is
    /// given the order: top left, top right, bottom right, bottom left.
    /// @throws std::out_of_range if the index is negative or larger than 3.
    inline Vector2 &operator[](int i) {
        if (i == 0) return topleft;
        if (i == 1) return topright;
        if (i == 2) return bottomright;
        if (i == 3) return bottomleft;

        throw std::out_of_range("Quad index cannot be larger than 3");
    }

    /// @brief Returns the center point of the Quad.
    inline Vector2 center() const noexcept { return (topleft + topright + bottomright + bottomleft) / 4.0f; }

    /// @brief Returns a Quad rotated around 
    /// a point by a given angle degree, 
    /// without modifying the current struct.
    Quad rotated_around(float degrees, Vector2 point) const noexcept;

    /// @brief Rotates the current Quad around 
    /// a point by a given angle degree, 
    /// without modifying the current struct.
    void rotate_around(float degrees, Vector2 point) noexcept;

    /// @brief Returns a Quad rotated around 
    /// its center by a given angle degree, 
    /// without modifying the current struct.
    inline Quad rotated(float degrees) const noexcept { return rotated_around(degrees, center()); }
    
    /// @brief Rotates the current Quad 
    /// by a given angle degree.
    inline void rotate(float degrees) noexcept { rotate_around(degrees, center()); }

    /// @brief Returns a rectangle that encloses
    /// all the vertices.
    Rectangle enclose() const noexcept;
};

};