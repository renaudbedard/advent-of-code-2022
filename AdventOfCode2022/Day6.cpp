#include <iostream>
#include <fstream>
#include <string>
#include <queue>

//#define PART_TWO

void Day6()
{
    std::ifstream InputStream;
    InputStream.open("day6input.txt", std::ios::in);

    uint32_t CharOccurences[26] = { 0 };
    uint32_t UniqueChars = 0;
    std::queue<char> LastNChars;
    int CharactersProcessed = 0;

#ifndef PART_TWO
    const uint32_t MessageLength = 4;
#else
    const uint32_t MessageLength = 14;
#endif

    auto KeyIndex = [](char Key) { return Key - 'a'; };

    for (char ThisChar; InputStream.get(ThisChar); )
    {
        if (LastNChars.size() == MessageLength)
        {
            char PoppedChar = LastNChars.front();
            int8_t PoppedIndex = KeyIndex(PoppedChar);
            CharOccurences[PoppedIndex]--;
            if (CharOccurences[PoppedIndex] == 0)
                UniqueChars--;
            LastNChars.pop();
        }

        CharactersProcessed++;

        int8_t PushedIndex = KeyIndex(ThisChar);
        CharOccurences[PushedIndex]++;
        if (CharOccurences[PushedIndex] == 1)
        {
            UniqueChars++;
            if (UniqueChars == MessageLength)
            {
                std::cout << CharactersProcessed << " characters were processed before the marker was detected.";
                return;
            }
        }
        LastNChars.push(ThisChar);
    }
}