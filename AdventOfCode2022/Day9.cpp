#include <iostream>
#include <fstream>
#include <string>
#include <unordered_set>

struct int2
{
    int X { 0 };
    int Y { 0 };

    int2() = default;
    int2(const int2& other) = default;
    int2& operator=(const int2& other) = default;
    int2(int2&& other) = default;
    int2& operator=(int2&& other) = default;

    int2(int v) : X(v), Y(v) { }
    int2(int x, int y) : X(x), Y(y) { }

    int2& operator+=(const int2& rhs)
    {
        X += rhs.X;
        Y += rhs.Y;
        return *this;
    }
    friend int2 operator+(int2 lhs, const int2& rhs)
    {
        lhs += rhs;
        return lhs;
    }

    int2& operator-=(const int2& rhs)
    {
        X -= rhs.X;
        Y -= rhs.Y;
        return *this;
    }
    friend int2 operator-(int2 lhs, const int2& rhs)
    {
        lhs -= rhs;
        return lhs;
    }

    int2& operator*=(const int2& rhs)
    {
        X *= rhs.X;
        Y *= rhs.Y;
        return *this;
    }
    friend int2 operator*(int2 lhs, const int2& rhs)
    {
        lhs *= rhs;
        return lhs;
    }
};

namespace std
{
    // Hash function template from https://stackoverflow.com/a/17017281/1721329
    template <>
    struct hash<int2>
    {
        size_t operator()(const int2& k) const
        {
            size_t res = 17;
            res = res * 31 + hash<int>()(k.X);
            res = res * 31 + hash<int>()(k.Y);
            return res;
        }
    };

    // Generic signum function from https://stackoverflow.com/a/4609795/1721329
    template <typename T> int sgn(T v) { return (T(0) < v) - (v < T(0)); }

    int2 abs(const int2& v) { return int2(abs(v.X), abs(v.Y)); }
    int2 sgn(const int2& v) { return int2(sgn(v.X), sgn(v.Y)); }
    int cmax(const int2& v) { return max(v.X, v.Y); }
    int2 min(const int2& lhs, const int2& rhs) { return int2(min(lhs.X, rhs.X), min(lhs.Y, rhs.Y)); }
}

inline bool operator==(const int2& lhs, const int2& rhs) { return lhs.X == rhs.X && lhs.Y == rhs.Y; }
inline bool operator!=(const int2& lhs, const int2& rhs) { return !(lhs == rhs); }

void Day9()
{
    std::ifstream InputStream;
    InputStream.open("day9input.txt", std::ios::in);

    const int RopeLength = 10; // 2 for Part 1, 10 for Part 2
    int2 Knots[RopeLength];

    std::unordered_set<int2> TailVisitedPositions;

    for (std::string ThisLine; std::getline(InputStream, ThisLine) && !ThisLine.empty(); )
    {
        int2 MoveDirection;
        switch (ThisLine[0])
        {
            case 'R': MoveDirection.X = 1; break;
            case 'L': MoveDirection.X = -1; break;
            case 'U': MoveDirection.Y = 1; break;
            case 'D': MoveDirection.Y = -1; break;
        }

        int MoveDistance = std::stoi(ThisLine.substr(2));
        for (int i = 0; i < MoveDistance; i++)
        {
            Knots[0] += MoveDirection;

            for (int j = 1; j < RopeLength; j++)
            {
                int2 HeadDistance = Knots[j - 1] - Knots[j];
                int2 AbsHeadDistance = std::abs(HeadDistance);

                if (std::cmax(AbsHeadDistance) > 1) // If at least 2 units away on one axis,
                    Knots[j] += std::min(AbsHeadDistance, int2(1)) * std::sgn(HeadDistance); // Move towards head at most 1 unit on each axis
            }

            TailVisitedPositions.insert(Knots[RopeLength - 1]);
        }
    }

    std::cout << TailVisitedPositions.size() << " positions were visited by the tail.\n";
}