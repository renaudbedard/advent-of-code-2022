#include <iostream>
#include <fstream>
#include <string>
#include <cassert>
#include <list>
#include <numeric>
#include <iterator>

int main()
{
    std::ifstream InputStream;
    InputStream.open("day1input.txt", std::ios::in);
    assert(InputStream);

    std::list<uint32_t> TopMaxes = { 0, 0, 0 };

    uint32_t CurrentMax = 0;

    while (!InputStream.eof())
    {
        std::string LineString;
        std::getline(InputStream, LineString);

        if (LineString.length() == 0)
        {
            for (auto MaxesIterator = TopMaxes.begin(); MaxesIterator != TopMaxes.end(); ++MaxesIterator)
            {
                if (CurrentMax > *MaxesIterator)
                {
                    uint32_t ReplacedRank = std::distance(TopMaxes.begin(), MaxesIterator);
                    TopMaxes.insert(MaxesIterator, CurrentMax);
                    TopMaxes.pop_back();
                    std::cout << "Found a new max " << CurrentMax << " at rank " << ReplacedRank <<
                        " (new top 3 : { " << TopMaxes.front() << ", " << *std::next(TopMaxes.begin(), 1) << ", " << TopMaxes.back() << " }\n";
                    break;
                }
            }
            CurrentMax = 0;
        }
        else
        {
            uint32_t CurrentValue = std::stoi(LineString);
            CurrentMax += CurrentValue;
        }
    }

    std::cout << "All-Time Max : " << TopMaxes.front() << "\n";
    std::cout << "Sum Of Top Three : " << std::reduce(TopMaxes.cbegin(), TopMaxes.cend());
}