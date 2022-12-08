#include <iostream>
#include <fstream>
#include <unordered_set>

#define GRID_SIZE 99

uint16_t Trees[GRID_SIZE][GRID_SIZE];

void Day8()
{
    std::ifstream InputStream;
    InputStream.open("day8input.txt", std::ios::in);

    int RowIndex = 0;
    int ColIndex = 0;
    for (char ThisChar; InputStream.get(ThisChar); )
    {
        if (ThisChar == '\n')
        {
            RowIndex++;
            ColIndex = 0;
            continue;
        }

        Trees[RowIndex][ColIndex] = ThisChar - '0';

        ColIndex++;
    }

    auto HashFunction = [](const std::pair<int, int>& p) { return p.first * GRID_SIZE + p.second; };
    std::unordered_set<std::pair<int, int>, decltype(HashFunction)> InnerVisibleTrees(1, HashFunction);

    uint16_t MaxHeight = 0;

    auto DetermineVisibility = [&InnerVisibleTrees, &MaxHeight](uint16_t i, uint16_t j)
    {
        uint16_t TreeHeight = Trees[i][j];
        if (TreeHeight > MaxHeight)
        {
            InnerVisibleTrees.insert(std::make_pair(i, j));
            MaxHeight = TreeHeight;
        }
    };

    for (RowIndex = 1; RowIndex < GRID_SIZE - 1; RowIndex++)
    {
        for (ColIndex = 1, MaxHeight = Trees[RowIndex][0]; ColIndex < GRID_SIZE - 1 && MaxHeight < 9; ColIndex++)
            DetermineVisibility(RowIndex, ColIndex);
        for (ColIndex = GRID_SIZE - 2, MaxHeight = Trees[RowIndex][GRID_SIZE - 1]; ColIndex > 0 && MaxHeight < 9; ColIndex--)
            DetermineVisibility(RowIndex, ColIndex);
    }

    for (ColIndex = 1; ColIndex < GRID_SIZE - 1; ColIndex++)
    {
        for (RowIndex = 1, MaxHeight = Trees[0][ColIndex]; RowIndex < GRID_SIZE - 1 && MaxHeight < 9; RowIndex++)
            DetermineVisibility(RowIndex, ColIndex);
        for (RowIndex = GRID_SIZE - 2, MaxHeight = Trees[GRID_SIZE - 1][ColIndex]; RowIndex > 0 && MaxHeight < 9; RowIndex--)
            DetermineVisibility(RowIndex, ColIndex);
    }

    const uint32_t ContourVisibleTrees = GRID_SIZE * 2 + (GRID_SIZE - 2) * 2;
    std::cout << (InnerVisibleTrees.size() + ContourVisibleTrees) << " trees are visible from outside the grid.\n";

    uint64_t HighestScenicScore = 0;

    // Since one of the edge score will always be 0, this causes a 0 scenic score and we can skip them
    for (RowIndex = 1; RowIndex < GRID_SIZE - 1; RowIndex++)
    for (ColIndex = 1; ColIndex < GRID_SIZE - 1; ColIndex++)
    {
        uint16_t TreeHeight = Trees[RowIndex][ColIndex];

        uint16_t LeftScore = 0;
        for (int j = ColIndex - 1; j >= 0; j--)
        {
            LeftScore++;
            if (Trees[RowIndex][j] >= TreeHeight) break;
        }

        uint16_t RightScore = 0;
        for (int j = ColIndex + 1; j < GRID_SIZE; j++)
        {
            RightScore++;
            if (Trees[RowIndex][j] >= TreeHeight) break;
        }

        uint16_t UpScore = 0;
        for (int i = RowIndex - 1; i >= 0; i--)
        {
            UpScore++;
            if (Trees[i][ColIndex] >= TreeHeight) break;
        }

        uint16_t DownScore = 0;
        for (int i = RowIndex + 1; i < GRID_SIZE; i++)
        {
            DownScore++;
            if (Trees[i][ColIndex] >= TreeHeight) break;
        }

        uint64_t ScenicScore = (uint64_t) LeftScore * RightScore * UpScore * DownScore;
        HighestScenicScore = std::max(ScenicScore, HighestScenicScore);
    }

    std::cout << "The highest scenic score is : " << HighestScenicScore;
}