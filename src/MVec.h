#pragma once

#include <SFML/Graphics.hpp>

#include <cmath>

#ifndef M_PIF
# define M_PIF	3.14159265358979323846f
#endif


struct Vec2
{
    Vec2(float _x = 0, float _y = 0) : x{ _x }, y{ _y } {};
    Vec2(int _x, int _y) : x{ (float)_x}, y{ (float)_y} {};
    //Vec2(int _x, int _y) : x{(float)_x}, y{(float)_y} {};
    Vec2(sf::Vector2f _v) : x{ _v.x }, y{ _v.y } {};
    Vec2(sf::Vector2u _v) : x{ (float)_v.x}, y{ (float)_v.y} {};
    float x;
    float y;

    inline Vec2& set(float _x, float _y) { x = _x; y = _y; return *this; }

    Vec2 operator+ (const Vec2 other) const { return Vec2(x + other.x, y + other.y); }
    Vec2 operator- (const Vec2 other) const { return Vec2(x - other.x, y - other.y); }
    Vec2 operator+= (const Vec2 other) { x += other.x; y += other.y; return *this; }
    Vec2 operator-= (const Vec2 other) { x -= other.x; y -= other.y; return *this; }
    Vec2 operator* (const Vec2 other) const { return Vec2(x * other.x, y * other.y); }
    Vec2 operator*= (const Vec2 other) { x *= other.x; y *= other.y; return *this; }


    Vec2 operator* (const float _m) const { return Vec2(x * _m, y * _m); }
    Vec2 operator/ (const float _m) const { return Vec2(x / _m, y / _m); }

    operator sf::Vector2f() const { return sf::Vector2f(x, y);  }

    float length() const { return sqrt(x*x + y*y); };

    static const Vec2 nullV;
    static const Vec2 UP;
    static const Vec2 DOWN;
    static const Vec2 LEFT;
    static const Vec2 RIGHT;

    static Vec2 random_rad(float max_radius) {
        float angle = rand() * 2. * M_PIF / RAND_MAX;
        return Vec2(sinf(angle), cosf(angle)) * (max_radius * rand() / RAND_MAX);
    }

    static float angleBetween(const Vec2 base, const Vec2 target);
};

std::ostream& operator<< (std::ostream& out, const Vec2& obj);

std::ostream& operator<< (std::ostream& out, const Vec2&& obj);

