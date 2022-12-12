#include <iostream>
#include <fstream>
#include <string>
#include <array>
#include <queue>
#include <chrono>

//#define PART_TWO
//#define VISUALIZATION

struct int2
{
    int X = 0, Y = 0;

    int2() = default;
    constexpr int2(int x, int y) : X(x), Y(y) { }

    friend int2 operator+(int2 lhs, const int2& rhs)
    {
        lhs.X += rhs.X;
        lhs.Y += rhs.Y;
        return lhs;
    }

    friend int2 operator-(int2 lhs, const int2& rhs)
    {
        lhs.X -= rhs.X;
        lhs.Y -= rhs.Y;
        return lhs;
    }
};

inline bool operator==(const int2& lhs, const int2& rhs) { return lhs.X == rhs.X && lhs.Y == rhs.Y; }
inline bool operator!=(const int2& lhs, const int2& rhs) { return lhs.X != rhs.X || lhs.Y != rhs.Y; }

const int GridWidth = 159;
const int GridHeight = 41;
//const int GridWidth = 8;
//const int GridHeight = 5;

const int GridSize = GridWidth * GridHeight;

std::array<int, GridSize> Heightmap;
std::array<int, GridSize> Distance;

#ifdef VISUALIZATION
std::array<int2, GridSize> Previous;
std::array<char, GridSize> Visualization;
#endif

constexpr int CoordToIndex(const int2& coord) { return coord.Y * GridWidth + coord.X; };
constexpr int2 IndexToCoord(int i) { return int2(i % GridWidth, i / GridWidth); };

constexpr std::array<int2, 4> PossibleSteps = { int2(-1, 0), int2(1, 0), int2(0, -1), int2(0, 1) };

void Day12()
{
    auto start = std::chrono::high_resolution_clock::now();

    int2 Goal;
    std::vector<int2> PossibleOrigins;

    std::ifstream InputStream;

    InputStream.open("day12input.txt", std::ios::in);
    //InputStream.open("day12testinput.txt", std::ios::in);

    int Cursor = 0;
    for (char Char; InputStream.get(Char); )
    {
        if (std::isspace(static_cast<unsigned char>(Char)))
            continue;

        if (Char == 'S')
        {
            PossibleOrigins.push_back(IndexToCoord(Cursor));
            Heightmap[Cursor++] = 0;
        }
        else if (Char == 'E')
        {
            Goal = IndexToCoord(Cursor);
            Heightmap[Cursor++] = 'z' - 'a';
        }
        else
        {
#ifdef PART_TWO
            if (Char == 'a')
                PossibleOrigins.push_back(IndexToCoord(Cursor));
#endif
            Heightmap[Cursor++] = Char - 'a';
        }
    }

    Distance.fill(-1);

    std::queue<int2> Queue;
    for (const int2& PossibleOrigin : PossibleOrigins)
    {
        Distance[CoordToIndex(PossibleOrigin)] = 0;
        Queue.push(PossibleOrigin);
    }

    bool FoundPath = false;

    while (!Queue.empty() && !FoundPath)
    {
        int2 Current = Queue.front();
        Queue.pop();

        int CurrentIndex = CoordToIndex(Current);
        int CurrentHeight = Heightmap[CurrentIndex];

        for (const int2& Step : PossibleSteps)
        {
            int2 CandidateCoord = Current + Step;
            int CandidateIndex = CoordToIndex(CandidateCoord);

            if (CandidateCoord.X >= 0 && CandidateCoord.X < GridWidth &&    // In bounds
                CandidateCoord.Y >= 0 && CandidateCoord.Y < GridHeight &&   
                (Heightmap[CandidateIndex] - CurrentHeight) <= 1 &&         // Climb at most 1 height
                Distance[CandidateIndex] == -1)                             // Not yet visited
            {
#ifdef VISUALIZATION
                Previous[CandidateIndex] = Current;
#endif
                Distance[CandidateIndex] = Distance[CurrentIndex] + 1;

                if (CandidateCoord == Goal)
                {
                    FoundPath = true;
                    break;
                }

                Queue.push(CandidateCoord);
            }
        }
    }

    int TotalSteps = Distance[CoordToIndex(Goal)];

    auto finish = std::chrono::high_resolution_clock::now();

    std::cout << "Found goal in " << TotalSteps << " steps. (took "
        << std::chrono::duration_cast<std::chrono::microseconds>(finish - start).count()
        << " microseconds)\n";

#ifdef VISUALIZATION
    Visualization.fill('.');
    Visualization[CoordToIndex(Goal)] = 'E';

    int2 Current = Goal;
    while (std::find(PossibleOrigins.begin(), PossibleOrigins.end(), Current) == PossibleOrigins.end())
    {
        int2 Preceding = Current;
        Current = Previous[CoordToIndex(Current)];
        int2 Delta = Preceding - Current;
        Visualization[CoordToIndex(Current)] = Delta.X == -1 ? '<' : Delta.X == 1 ? '>' : Delta.Y == -1 ? '^' : 'v';
    }

    Cursor = 0;
    for (int i = 0; i < GridHeight; i++)
    {
        for (int j = 0; j < GridWidth; j++)
            std::cout << Visualization[Cursor++];
        std::cout << '\n';
    }
#endif
}