#include <iostream>
#include <fstream>
#include <string>
#include <deque>
#include <stack>
#include <regex>

//#define PART_TWO

void Day5()
{
    std::ifstream InputStream;
    InputStream.open("day5input.txt", std::ios::in);

    std::deque<char> Stacks[9];

    // Read all lines up to the first empty line (stack state)
    for (std::string LineString; std::getline(InputStream, LineString) && LineString.length() > 0; )
    {
        for (uint8_t i = 0; i < 9; i++)
        {
            char PotentialCrate = LineString[i * 4 + 1]; // Skip brackets & spaces, assuming a regular pattern
            if (std::isupper(PotentialCrate)) // Ignore numbering line
                Stacks[i].push_back(PotentialCrate);
        }
    }

    std::regex Regex("move (\\d{1,2}) from (\\d) to (\\d)");

#ifdef PART_TWO
    std::stack<char> TemporaryStack;
#endif

    // Read & apply rearrangement procedure
    for (std::string LineString; std::getline(InputStream, LineString); )
    {
        std::smatch Matches;
        std::regex_search(LineString, Matches, Regex);

        uint8_t MovedQuantity = std::stoi(Matches[1]);
        uint8_t FromStack = std::stoi(Matches[2]) - 1; // Array is 0-indexed, instructions are 1-indexed
        uint8_t ToStack = std::stoi(Matches[3]) - 1;

        for (uint8_t i = 0; i < MovedQuantity; i++)
        {
            char poppedCrate = Stacks[FromStack].front();
            Stacks[FromStack].pop_front();
#ifdef PART_TWO
            TemporaryStack.push(poppedCrate);
#else
            Stacks[ToStack].push_front(poppedCrate);
#endif
        }

#ifdef PART_TWO
        while (!TemporaryStack.empty())
        {
            char poppedCrate = TemporaryStack.top();
            Stacks[ToStack].push_front(poppedCrate);
            TemporaryStack.pop();
        }
#endif
    }

    std::cout << "After rearrangement, top of stacks : ";
    for (uint8_t i = 0; i < 9; i++)
        std::cout << Stacks[i].front();
}