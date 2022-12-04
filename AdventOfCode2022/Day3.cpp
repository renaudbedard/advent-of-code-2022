#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <cctype>

void Day3PartOne()
{
    std::ifstream InputStream;
    InputStream.open("day3input.txt", std::ios::in);
    if (!InputStream) throw new std::runtime_error("Can't find Day 3 input text file");

    uint32_t TotalPriority = 0;

    while (!InputStream.eof())
    {
        std::string LineString;
        std::getline(InputStream, LineString);

        size_t MidPoint = LineString.length() / 2;
        unsigned char CommonItem = '\0';

        for (int i = 0; i < MidPoint; i++)
            if (LineString.find(LineString[i], MidPoint) != std::string::npos)
            {
                if (CommonItem != '\0' && CommonItem != LineString[i]) throw new std::runtime_error("More than one common item!");
                CommonItem = LineString[i];
                // Technically we could break here, but to be safe...
            }

        if (CommonItem == '\0')
            continue;

        int32_t ThisPriority = std::islower(CommonItem) ? CommonItem - 'a' + 1 : CommonItem - 'A' + 27;
        std::cout << CommonItem << " => " << ThisPriority << "\n";
        TotalPriority += ThisPriority;
    }

    std::cout << "[Part 1] Total Priority : " << TotalPriority << "\n\n";
}

void Day3PartTwo()
{
    std::ifstream InputStream;
    InputStream.open("day3input.txt", std::ios::in);
    if (!InputStream) throw new std::runtime_error("Can't find Day 3 input text file");

    uint32_t TotalPriority = 0;
    std::string GroupLines[3];

    while (!InputStream.eof())
    {
        for (int i = 0; i < 3; i++)
            std::getline(InputStream, GroupLines[i]);

        unsigned char CommonItem = '\0';

        for (int i = 0; i < GroupLines[0].length(); i++)
        {
            unsigned char ItemCandidate = GroupLines[0][i];
            if (GroupLines[1].find(ItemCandidate) != std::string::npos &&
                GroupLines[2].find(ItemCandidate) != std::string::npos)
            {
                if (CommonItem != '\0' && CommonItem != ItemCandidate) throw new std::runtime_error("More than one common item!");
                CommonItem = ItemCandidate;
                // Technically we could break here, but to be safe...
            }
        }

        if (CommonItem == '\0')
            continue;

        int32_t ThisPriority = std::islower(CommonItem) ? CommonItem - 'a' + 1 : CommonItem - 'A' + 27;
        std::cout << CommonItem << " => " << ThisPriority << "\n";
        TotalPriority += ThisPriority;
    }

    std::cout << "[Part 2] Total Priority : " << TotalPriority;
}

void Day3()
{
    Day3PartOne();
    Day3PartTwo();
}