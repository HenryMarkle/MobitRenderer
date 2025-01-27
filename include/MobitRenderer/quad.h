#pragma once

#include <cstdint>

#include <raylib.h>
#include <raymath.h>

#include <MobitRenderer/vec.h>

namespace mr {

struct iquad {
    IVector2 topright, topleft, bottomleft, bottomright;

    iquad();
    iquad(IVector2, IVector2, IVector2, IVector2);
  
    inline iquad operator+(IVector2 v) const noexcept {
        return iquad(
            topleft + v, 
            topright + v, 
            bottomright + v, 
            bottomleft + v
        );
    }
    inline iquad operator+(Vector2 v) const noexcept {
        return iquad(
            topleft + v, 
            topright + v, 
            bottomright + v, 
            bottomleft + v
        );
    }
  
    inline iquad operator-(IVector2 v) const noexcept {
        return iquad(
            topleft - v, 
            topright - v, 
            bottomright - v, 
            bottomleft - v
        );
    }
    inline iquad operator-(Vector2 v) const noexcept {
        return iquad(
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

        throw std::out_of_range("iquad index cannot be larger than 3");
    }

    /// @brief Returns a vertex depends on an index, which is
    /// given the order: top left, top right, bottom right, bottom left.
    /// @throws std::out_of_range if the index is negative or larger than 3.
    inline IVector2 &operator[](int i) {
        if (i == 0) return topleft;
        if (i == 1) return topright;
        if (i == 2) return bottomright;
        if (i == 3) return bottomleft;

        throw std::out_of_range("iquad index cannot be negative or larger than 3");
    }

    /// @brief Returns the center point of the quad.
    inline IVector2 center() const noexcept { return (topleft + topright + bottomright + bottomleft) / 4; }

    /// @brief Returns a rotated quad by a given angle degree, 
    /// without modifying the current struct.
    iquad rotated(float degrees) const noexcept;
    
    /// @brief Rotates the current quad 
    /// by a given angle degree.
    void rotate(float degrees) noexcept;

    /// @brief Returns a rectangle that encloses
    /// all the vertices.
    Rectangle enclose() const noexcept;
};

struct quad {
    Vector2 topright, topleft, bottomleft, bottomright;

    quad();
    quad(Vector2, Vector2, Vector2, Vector2);

    inline quad operator+(IVector2 v) const noexcept {
        return quad(
            Vector2 { topleft.x + v.x, topleft.y + v.y }, 
            Vector2 { topright.x + v.x, topright.y + v.y }, 
            Vector2 { bottomright.x + v.x, bottomright.y + v.y }, 
            Vector2 { bottomleft.x + v.x, bottomleft.y + v.y }
        );
    }
    inline quad operator+(Vector2 v) const noexcept {
        return quad(
            Vector2Add(topleft, v),
            Vector2Add(topright, v),
            Vector2Add(bottomright, v),
            Vector2Add(bottomleft, v)
        );
    }
  
    inline quad operator-(IVector2 v) const noexcept {
        return quad(
            Vector2 { topleft.x - v.x, topleft.y - v.y }, 
            Vector2 { topright.x - v.x, topright.y - v.y }, 
            Vector2 { bottomright.x - v.x, bottomright.y - v.y }, 
            Vector2 { bottomleft.x - v.x, bottomleft.y - v.y }
        );
    }
    inline quad operator-(Vector2 v) const noexcept {
        return quad(
            Vector2Subtract(topleft, v),
            Vector2Subtract(topright, v),
            Vector2Subtract(bottomright, v),
            Vector2Subtract(bottomleft, v)
        );
    }

    /// @brief Returns a vertex depends on an index, which is
    /// given the order: top left, top right, bottom right, bottom left.
    /// @throws std::out_of_range if the index is larger than 3.
    inline Vector2 &operator[](uint8_t i) {
        if (i == 0) return topleft;
        if (i == 1) return topright;
        if (i == 2) return bottomright;
        if (i == 3) return bottomleft;

        throw std::out_of_range("quad index cannot be larger than 3");
    }

    /// @brief Returns a vertex depends on an index, which is
    /// given the order: top left, top right, bottom right, bottom left.
    /// @throws std::out_of_range if the index is negative or larger than 3.
    inline Vector2 &operator[](int i) {
        if (i == 0) return topleft;
        if (i == 1) return topright;
        if (i == 2) return bottomright;
        if (i == 3) return bottomleft;

        throw std::out_of_range("quad index cannot be larger than 3");
    }

    /// @brief Returns the center point of the quad.
    inline Vector2 center() const noexcept { return (topleft + topright + bottomright + bottomleft) / 4.0f; }

    /// @brief Returns a quad rotated around 
    /// a point by a given angle degree, 
    /// without modifying the current struct.
    quad rotated_around(float degrees, Vector2 point) const noexcept;

    /// @brief Rotates the current quad around 
    /// a point by a given angle degree, 
    /// without modifying the current struct.
    void rotate_around(float degrees, Vector2 point) noexcept;

    /// @brief Returns a quad rotated around 
    /// its center by a given angle degree, 
    /// without modifying the current struct.
    inline quad rotated(float degrees) const noexcept { return rotated_around(degrees, center()); }
    
    /// @brief Rotates the current quad 
    /// by a given angle degree.
    void rotate(float degrees) noexcept;

    /// @brief Returns a rectangle that encloses
    /// all the vertices.
    Rectangle enclose() const noexcept;
};

};