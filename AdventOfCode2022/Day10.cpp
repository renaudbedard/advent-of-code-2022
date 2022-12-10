#include <iostream>
#include <fstream>
#include <string>

float Frac(int Value, int Base)
{
    float Quotient = (float)Value / Base;
    return Quotient - (int)Quotient;
}

void Day10()
{
    const int CycleOrigin = 20;
    const int CycleStep = 40;
    const int CycleStop = 220;

    const int CrtWidth = 40;

    int SignalStrengthSum = 0;
    int CycleCount = 1;
    int RegisterValue = 1;

    auto ProcessCycle = [&]()
    {
        if (CycleCount <= CycleStop && Frac(CycleCount - CycleOrigin, CycleStep) == 0)
            SignalStrengthSum += RegisterValue * CycleCount;

        int BeamPosition = (int) std::fmod(CycleCount - 1, CrtWidth);
        if (std::abs(BeamPosition - RegisterValue) <= 1)
            std::cout << '#';
        else
            std::cout << '.';

        if (Frac(CycleCount, CrtWidth) == 0)
            std::cout << '\n';
    };

    std::ifstream InputStream;
    InputStream.open("day10input.txt", std::ios::in);

    for (std::string ThisLine; std::getline(InputStream, ThisLine) && !ThisLine.empty(); )
    {
        if (ThisLine == "noop")
        {
            ProcessCycle();
            CycleCount++;
        }
        else // assume addx
        {
            ProcessCycle();
            CycleCount++;
            ProcessCycle();
            RegisterValue += std::stoi(ThisLine.substr(5));
            CycleCount++;
        }
    }

    std::cout << "The sum of signal strengths is : " << SignalStrengthSum;
}