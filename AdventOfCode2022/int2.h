#pragma once

#include <functional>

#include "math.h"

struct int2
{
    int X = 0, Y = 0;

    int2() = default;
    constexpr int2(int v) : X(v), Y(v) { }
    constexpr int2(int x, int y) : X(x), Y(y) { }

    int2& operator+=(const int2& rhs)
    {
        X += rhs.X;
        Y += rhs.Y;
        return *this;
    }

    friend int2 operator+(int2 lhs, const int2& rhs)
    {
        lhs.X += rhs.X;
        lhs.Y += rhs.Y;
        return lhs;
    }

    friend int2 operator-(int2 lhs, const int2& rhs)
    {
        lhs.X -= rhs.X;
        lhs.Y -= rhs.Y;
        return lhs;
    }

    friend int2 operator*(int2 lhs, const int2& rhs)
    {
        lhs.X *= rhs.X;
        lhs.Y *= rhs.Y;
        return lhs;
    }

    friend int2 operator*(int2 lhs, int rhs)
    {
        lhs.X *= rhs;
        lhs.Y *= rhs;
        return lhs;
    }
};

namespace std
{
    template <>
    struct hash<int2>
    {
        // Hash function template from https://stackoverflow.com/a/17017281/1721329
        size_t operator()(const int2& k) const
        {
            size_t res = 17;
            res = res * 31 + hash<int>()(k.X);
            res = res * 31 + hash<int>()(k.Y);
            return res;
        }
    };

    inline int2 abs(const int2& v) { return int2(abs(v.X), abs(v.Y)); }
    inline int2 sgn(const int2& v) { return int2(sgn(v.X), sgn(v.Y)); }
    inline int cmax(const int2& v) { return max(v.X, v.Y); }
    inline int2 min(const int2& lhs, const int2& rhs) { return int2(min(lhs.X, rhs.X), min(lhs.Y, rhs.Y)); }
    inline int2 max(const int2& lhs, const int2& rhs) { return int2(max(lhs.X, rhs.X), max(lhs.Y, rhs.Y)); }
}

inline bool operator==(const int2& lhs, const int2& rhs) { return lhs.X == rhs.X && lhs.Y == rhs.Y; }
inline bool operator!=(const int2& lhs, const int2& rhs) { return lhs.X != rhs.X || lhs.Y != rhs.Y; }
