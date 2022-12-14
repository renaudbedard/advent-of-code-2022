#include <iostream>
#include <fstream>
#include <string>
#include <unordered_set>

#include "int2.h"

void Day9()
{
    const int RopeLength = 10; // 2 for Part 1, 10 for Part 2
    int2 Knots[RopeLength];

    std::unordered_set<int2> TailVisitedPositions;

    std::ifstream InputStream;
    InputStream.open("day9input.txt", std::ios::in);

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
                    Knots[j] += std::min(AbsHeadDistance, int2(1, 1)) * std::sgn(HeadDistance); // Move towards head at most 1 unit on each axis
            }

            TailVisitedPositions.insert(Knots[RopeLength - 1]);
        }
    }

    std::cout << TailVisitedPositions.size() << " positions were visited by the tail.\n";
}