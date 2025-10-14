#ifndef NGE_VEC2_H
#define NGE_VEC2_H

class Vec2 {
public:
    float x, y;
    Vec2() : x(0), y(0) {}
    Vec2(float x, float y) : x(x), y(y) {}
    Vec2 operator+(const Vec2& other) const { return Vec2(x + other.x, y + other.y); }
    Vec2 operator-(const Vec2& other) const { return Vec2(x - other.x, y - other.y); }
    Vec2 operator*(float scalar) const { return Vec2(x * scalar, y * scalar); }
    Vec2& operator+=(const Vec2& other) { x += other.x; y += other.y; return *this; }
    Vec2& operator-=(const Vec2& other) { x -= other.x; y -= other.y; return *this; }
    Vec2& operator*=(float scalar) { x *= scalar; y *= scalar; return *this; }
    float length() const { return sqrtf(x * x + y * y); }
    Vec2 normalized() const { float len = length(); return len > 0 ? Vec2(x / len, y / len) : Vec2(0, 0); }
    ~Vec2() = default;
};

#endif