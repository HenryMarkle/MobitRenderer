#include <math.h>

#include <MobitRenderer/quad.h>
#include <MobitRenderer/vec.h>

#define MINF(a, b) a > b ? b : a 
#define MAXF(a, b) a < b ? b : a 

namespace mr {

IQuad::IQuad() : 
    topleft({0, 0}), 
    topright({0, 0}), 
    bottomright({0, 0}), 
    bottomleft({0, 0}) 
{}

IQuad::IQuad(
    IVector2 topleft, 
    IVector2 topright, 
    IVector2 bottomright, 
    IVector2 bottomleft
) :
    topleft(topleft), 
    topright(topright), 
    bottomright(bottomright), 
    bottomleft(bottomleft) 
{}

IQuad::IQuad(Rectangle r) :
    topleft(IVector2{(int)r.x, (int)r.y}),
    topright(IVector2{(int)(r.x + r.width), (int)r.y}),
    bottomright(IVector2{(int)(r.x + r.width), (int)(r.y + r.height)}),
    bottomleft(IVector2{(int)r.x, (int)(r.y + r.height)})
{}


Quad::Quad() : 
    topleft({0, 0}), 
    topright({0, 0}), 
    bottomright({0, 0}), 
    bottomleft({0, 0}) 
{}

Quad::Quad(
    Vector2 topleft, 
    Vector2 topright, 
    Vector2 bottomright, 
    Vector2 bottomleft
) :
    topleft(topleft), 
    topright(topright), 
    bottomright(bottomright), 
    bottomleft(bottomleft) 
{}

Quad::Quad(Rectangle r) :
    topleft({r.x, r.y}),
    topright({r.x + r.width, r.y}),
    bottomright({r.x + r.width, r.y + r.height}),
    bottomleft({r.x, r.y + r.height})
{}

IQuad IQuad::rotated_around(float degrees, IVector2 ctr) const noexcept {
    float radian = degrees * PI / 180.0f;
        
    float sin_rotation = (float)sin(radian);
    float cos_rotation = (float)cos(radian);
    
    IQuad new_quad;

    { // Rotate the top left corner
        const float &x = topleft.x;
        const float &y = topleft.y;

        float dx = x - ctr.x;
        float dy = y - ctr.y;

        new_quad.topleft = IVector2{
            (int)(ctr.x + dx * cos_rotation - dy * sin_rotation), 
            (int)(ctr.y + dx * sin_rotation + dy * cos_rotation)
        };
    }
    
    { // Rotate the top right corner
        const float &x = topright.x;
        const float &y = topright.y;

        float dx = x - ctr.x;
        float dy = y - ctr.y;

        new_quad.topright = IVector2{
            (int)(ctr.x + dx * cos_rotation - dy * sin_rotation), 
            (int)(ctr.y + dx * sin_rotation + dy * cos_rotation)
        };
    }
    
    { // Rotate the bottom right corner
        const float &x = bottomright.x;
        const float &y = bottomright.y;

        float dx = x - ctr.x;
        float dy = y - ctr.y;

        new_quad.bottomright = IVector2{
            (int)(ctr.x + dx * cos_rotation - dy * sin_rotation), 
            (int)(ctr.y + dx * sin_rotation + dy * cos_rotation)
        };
    }
    
    { // Rotate the bottom left corner
        const float &x = bottomleft.x;
        const float &y = bottomleft.y;

        float dx = x - ctr.x;
        float dy = y - ctr.y;

        new_quad.bottomleft = IVector2{
            (int)(ctr.x + dx * cos_rotation - dy * sin_rotation), 
            (int)(ctr.y + dx * sin_rotation + dy * cos_rotation)
        };
    }

    return new_quad;
}
void IQuad::rotate_around(float degrees, IVector2 ctr) noexcept {
    float radian = degrees * PI / 180.0f;
        
    float sin_rotation = (float)sin(radian);
    float cos_rotation = (float)cos(radian);
    
    { // Rotate the top left corner
        const float &x = topleft.x;
        const float &y = topleft.y;

        float dx = x - ctr.x;
        float dy = y - ctr.y;

        topleft = IVector2{
            (int)(ctr.x + dx * cos_rotation - dy * sin_rotation), 
            (int)(ctr.y + dx * sin_rotation + dy * cos_rotation)
        };
    }
    
    { // Rotate the top right corner
        const float &x = topright.x;
        const float &y = topright.y;

        float dx = x - ctr.x;
        float dy = y - ctr.y;

        topright = IVector2{
            (int)(ctr.x + dx * cos_rotation - dy * sin_rotation), 
            (int)(ctr.y + dx * sin_rotation + dy * cos_rotation)
        };
    }
    
    { // Rotate the bottom right corner
        const float &x = bottomright.x;
        const float &y = bottomright.y;

        float dx = x - ctr.x;
        float dy = y - ctr.y;

        bottomright = IVector2{
            (int)(ctr.x + dx * cos_rotation - dy * sin_rotation), 
            (int)(ctr.y + dx * sin_rotation + dy * cos_rotation)
        };
    }
    
    { // Rotate the bottom left corner
        const float &x = bottomleft.x;
        const float &y = bottomleft.y;

        float dx = x - ctr.x;
        float dy = y - ctr.y;

        bottomleft = IVector2{
            (int)(ctr.x + dx * cos_rotation - dy * sin_rotation), 
            (int)(ctr.y + dx * sin_rotation + dy * cos_rotation)
        };
    }
}

Rectangle IQuad::enclose() const noexcept {
    float min_x = MINF(MINF(topleft.x, topright.x), MINF(bottomleft.x, bottomright.x));
    float min_y = MINF(MINF(topleft.y, topright.y), MINF(bottomleft.y, bottomright.y));
    
    float max_x = MAXF(MAXF(topleft.x, topright.x), MAXF(bottomleft.x, bottomright.x));
    float max_y = MAXF(MAXF(topleft.y, topright.y), MAXF(bottomleft.y, bottomright.y));

    return Rectangle { min_x, min_y, (max_x - min_x), (max_y - min_y) };
}

Quad Quad::rotated_around(float degrees, Vector2 ctr) const noexcept {
    float radian = degrees * PI / 180.0f;
        
    float sin_rotation = (float)sin(radian);
    float cos_rotation = (float)cos(radian);
    
    Quad new_quad;

    { // Rotate the top left corner
        const float &x = topleft.x;
        const float &y = topleft.y;

        float dx = x - ctr.x;
        float dy = y - ctr.y;

        new_quad.topleft = Vector2{
            ctr.x + dx * cos_rotation - dy * sin_rotation, 
            ctr.y + dx * sin_rotation + dy * cos_rotation
        };
    }
    
    { // Rotate the top right corner
        const float &x = topright.x;
        const float &y = topright.y;

        float dx = x - ctr.x;
        float dy = y - ctr.y;

        new_quad.topright = Vector2{
            ctr.x + dx * cos_rotation - dy * sin_rotation, 
            ctr.y + dx * sin_rotation + dy * cos_rotation
        };
    }
    
    { // Rotate the bottom right corner
        const float &x = bottomright.x;
        const float &y = bottomright.y;

        float dx = x - ctr.x;
        float dy = y - ctr.y;

        new_quad.bottomright = Vector2{
            ctr.x + dx * cos_rotation - dy * sin_rotation, 
            ctr.y + dx * sin_rotation + dy * cos_rotation
        };
    }
    
    { // Rotate the bottom left corner
        const float &x = bottomleft.x;
        const float &y = bottomleft.y;

        float dx = x - ctr.x;
        float dy = y - ctr.y;

        new_quad.bottomleft = Vector2{
            ctr.x + dx * cos_rotation - dy * sin_rotation, 
            ctr.y + dx * sin_rotation + dy * cos_rotation
        };
    }

    return new_quad;
}
void Quad::rotate_around(float degrees, Vector2 ctr) noexcept {
    float radian = degrees * PI / 180.0f;
        
    float sin_rotation = (float)sin(radian);
    float cos_rotation = (float)cos(radian);
    
    { // Rotate the top left corner
        const float &x = topleft.x;
        const float &y = topleft.y;

        float dx = x - ctr.x;
        float dy = y - ctr.y;

        topleft = Vector2{
            ctr.x + dx * cos_rotation - dy * sin_rotation, 
            ctr.y + dx * sin_rotation + dy * cos_rotation
        };
    }
    
    { // Rotate the top right corner
        const float &x = topright.x;
        const float &y = topright.y;

        float dx = x - ctr.x;
        float dy = y - ctr.y;

        topright = Vector2{
            ctr.x + dx * cos_rotation - dy * sin_rotation, 
            ctr.y + dx * sin_rotation + dy * cos_rotation
        };
    }
    
    { // Rotate the bottom right corner
        const float &x = bottomright.x;
        const float &y = bottomright.y;

        float dx = x - ctr.x;
        float dy = y - ctr.y;

        bottomright = Vector2{
            ctr.x + dx * cos_rotation - dy * sin_rotation, 
            ctr.y + dx * sin_rotation + dy * cos_rotation
        };
    }
    
    { // Rotate the bottom left corner
        const float &x = bottomleft.x;
        const float &y = bottomleft.y;

        float dx = x - ctr.x;
        float dy = y - ctr.y;

        bottomleft = Vector2{
            ctr.x + dx * cos_rotation - dy * sin_rotation, 
            ctr.y + dx * sin_rotation + dy * cos_rotation
        };
    }
}

Rectangle Quad::enclose() const noexcept {
    float min_x = MINF(MINF(topleft.x, topright.x), MINF(bottomleft.x, bottomright.x));
    float min_y = MINF(MINF(topleft.y, bottomleft.y), MINF(topright.y, bottomright.y));
    
    float max_x = MAXF(MAXF(topleft.x, topright.x), MAXF(bottomleft.x, bottomright.x));
    float max_y = MAXF(MAXF(topleft.y, bottomleft.y), MAXF(topright.y, bottomright.y));

    return Rectangle { min_x, min_y, (max_x - min_x), (max_y - min_y) };
}

};