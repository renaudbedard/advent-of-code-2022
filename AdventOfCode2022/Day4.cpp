#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <regex>

void Day4()
{
    std::ifstream InputStream;
    InputStream.open("day4input.txt", std::ios::in);
    if (!InputStream) throw new std::runtime_error("Can't find Day 4 input text file");

    uint32_t ContainedRanges = 0;
    uint32_t OverlappingRanges = 0;

    std::regex Regex("(\\d{1,2})-(\\d{1,2}),(\\d{1,2})-(\\d{1,2})");

    for (std::string LineString; std::getline(InputStream, LineString); )
    {
        std::smatch Matches;
        std::regex_search(LineString, Matches, Regex);
        std::pair<uint32_t, uint32_t> Range1(std::stoi(Matches[1]), std::stoi(Matches[2]));
        std::pair<uint32_t, uint32_t> Range2(std::stoi(Matches[3]), std::stoi(Matches[4]));

        auto IsInscribed = [](std::pair<uint32_t, uint32_t> Inner, std::pair<uint32_t, uint32_t> Outer)
        {
            return Inner.first >= Outer.first && Inner.second <= Outer.second;
        };
        auto Overlaps = [](std::pair<uint32_t, uint32_t> A, std::pair<uint32_t, uint32_t> B)
        {
            // Min is contained, or max is contained (order insensitive, both "edges" of both ranges are tested)
            return 
                (A.first >= B.first && A.first <= B.second) || (A.second >= B.first && A.second <= B.second) || 
                (B.first >= A.first && B.first <= A.second) || (B.second >= A.first && B.second <= A.second);
        };

        if (IsInscribed(Range1, Range2))
        {
            std::cout << "[" << Range1.first << ", " << Range1.second << "] is inscribed in [" << Range2.first << ", " << Range2.second << "]\n";
            ContainedRanges++;
        }
        else if (IsInscribed(Range2, Range1))
        {
            std::cout << "[" << Range2.first << ", " << Range2.second << "] is inscribed in [" << Range1.first << ", " << Range1.second << "]\n";
            ContainedRanges++;
        }

        if (Overlaps(Range1, Range2))
        {
            std::cout << "[" << Range1.first << ", " << Range1.second << "] overlaps with [" << Range2.first << ", " << Range2.second << "]\n";
            OverlappingRanges++;
        }
    }

    std::cout << "\n[Part 1] Total fully contained pairs : " << ContainedRanges << "\n";
    std::cout << "[Part 2] Overlapping pairs : " << OverlappingRanges;
}
