#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>

//#define PART_ONE
#define PART_TWO

// Enum values are the base score
enum class Shape
{
    Rock = 1,
    Paper = 2,
    Scissors = 3
};
constexpr const char* ShapeToName(Shape Shape)
{
    switch (Shape)
    {
    case Shape::Rock: return "Rock";
    case Shape::Paper: return "Paper";
    case Shape::Scissors: return "Scissors";
    }
    throw new std::invalid_argument("Shape has invalid argument");
}

enum class Outcome
{
    Loss,
    Draw,
    Win
};
constexpr const char* OutcomeToName(Outcome Outcome)
{
    switch (Outcome)
    {
    case Outcome::Draw: return "Draw";
    case Outcome::Win: return "Win";
    case Outcome::Loss: return "Loss";
    }
    throw new std::invalid_argument("Outcome has invalid argument");
}

constexpr Shape WhatWinsAgainst(Shape Shape)
{
    switch (Shape)
    {
    case Shape::Rock: return Shape::Paper;
    case Shape::Paper: return Shape::Scissors;
    case Shape::Scissors: return Shape::Rock;
    }
    throw new std::invalid_argument("Shape has invalid argument");
}

constexpr Shape WhatLosesAgainst(Shape Shape)
{
    switch (Shape)
    {
    case Shape::Rock: return Shape::Scissors;
    case Shape::Paper: return Shape::Rock;
    case Shape::Scissors: return Shape::Paper;
    }
    throw new std::invalid_argument("Shape has invalid argument");
}

constexpr Outcome GetOutcome(Shape Ours, Shape Theirs)
{
    if (Ours == Theirs) return Outcome::Draw;
    if (Ours == WhatWinsAgainst(Theirs)) return Outcome::Win;
    return Outcome::Loss;
}

constexpr Shape ParseShape(char Character)
{
    switch (Character)
    {
    case 'A':
    case 'X':
        return Shape::Rock;
    case 'B':
    case 'Y':
        return Shape::Paper;
    case 'C':
    case 'Z':
        return Shape::Scissors;
    }
    throw new std::invalid_argument("Character has invalid argument");
}

constexpr Outcome ParseOutcome(char Character)
{
    switch (Character)
    {
    case 'X': return Outcome::Loss;
    case 'Y': return Outcome::Draw;
    case 'Z': return Outcome::Win;
    }
    throw new std::invalid_argument("Character has invalid argument");
}

constexpr uint32_t GetOutcomeScore(Outcome Outcome)
{
    switch (Outcome)
    {
    case Outcome::Win: return 6;
    case Outcome::Draw: return 3;
    case Outcome::Loss: return 0;
    }
    throw new std::invalid_argument("Outcome has invalid argument");
}

void Day2()
{
    std::ifstream InputStream;
    InputStream.open("day2input.txt", std::ios::in);
    if (!InputStream) throw new std::runtime_error("Can't find Day 2 input text file");

    uint32_t TotalScore = 0;
    
    while (!InputStream.eof())
    {
        std::string LineString;
        std::getline(InputStream, LineString);

        if (LineString.length() == 0)
            continue;

        Shape Theirs = ParseShape(LineString[0]);
        Shape Ours;
        Outcome RoundOutcome;

#ifdef PART_ONE
        Ours = ParseShape(LineString[2]);
        RoundOutcome = GetOutcome(Ours, Theirs);
#elif defined(PART_TWO)
        RoundOutcome = ParseOutcome(LineString[2]);
        switch (RoundOutcome)
        {
        case Outcome::Win: Ours = WhatWinsAgainst(Theirs); break;
        case Outcome::Draw: Ours = Theirs; break;
        case Outcome::Loss: Ours = WhatLosesAgainst(Theirs); break;
        default: throw new std::invalid_argument("Invalid round outcome");
        }
#endif

        uint32_t BaseScore = (uint32_t) Ours;
        uint32_t RoundScore = BaseScore + GetOutcomeScore(RoundOutcome);
        TotalScore += RoundScore;

#ifdef PART_ONE
        std::cout << LineString[0] << " (" << ShapeToName(Theirs) << ") vs. " << LineString[2] << " (" << ShapeToName(Ours) << ") => " << RoundScore << "\n";
#elif defined(PART_TWO)
        std::cout << LineString[0] << " (" << ShapeToName(Theirs) << ") vs. " << LineString[2] << " (" << OutcomeToName(RoundOutcome) << " ~ " << ShapeToName(Ours) << ") => " << RoundScore << "\n";
#endif
    }

    std::cout << "Total Score : " << TotalScore;
}