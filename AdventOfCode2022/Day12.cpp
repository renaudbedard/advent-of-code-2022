#include <iostream>
#include <fstream>
#include <string>
#include <array>
#include <queue>

//#define PART_TWO

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
std::array<int2, GridSize> Previous;
std::array<char, GridSize> Visualization;

constexpr int CoordToIndex(const int2& coord) { return coord.Y * GridWidth + coord.X; };
constexpr int2 IndexToCoord(int i) { return int2(i % GridWidth, i / GridWidth); };

void Day12()
{
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

    Distance.fill(std::numeric_limits<int>().max());

    auto Comparator = [](const int2& lhs, const int2& rhs) { return Distance[CoordToIndex(lhs)] > Distance[CoordToIndex(rhs)]; };
    std::priority_queue<int2, std::vector<int2>, decltype(Comparator)> Queue(Comparator);
    for (int2 PossibleOrigin : PossibleOrigins)
    {
        Distance[CoordToIndex(PossibleOrigin)] = 0;
        Queue.push(PossibleOrigin);
    }

    Previous.fill(int2(-1, -1));

    std::array<int2, 4> PossibleSteps = { int2(-1, 0), int2(1, 0), int2(0, -1), int2(0, 1) };
    bool FoundPath = false;

    while (!Queue.empty() && !FoundPath)
    {
        int2 Current = Queue.top();
        Queue.pop();

        int CurrentIndex = CoordToIndex(Current);
        int CurrentHeight = Heightmap[CurrentIndex];

        for (int2 Step : PossibleSteps)
        {
            int2 CandidateCoord = Current + Step;
            int CandidateIndex = CoordToIndex(CandidateCoord);

            if (CandidateCoord.X >= 0 && CandidateCoord.X < GridWidth &&
                CandidateCoord.Y >= 0 && CandidateCoord.Y < GridHeight &&
                (Heightmap[CandidateIndex] - CurrentHeight) <= 1)
            {
                int CandidateDistance = Distance[CurrentIndex] + 1;
                if (CandidateDistance < Distance[CandidateIndex])
                {
                    Previous[CandidateIndex] = Current;
                    if (CandidateCoord == Goal)
                    {
                        FoundPath = true;
                        break;
                    }

                    Distance[CandidateIndex] = CandidateDistance;
                    Queue.push(CandidateCoord);
                }
            }
        }
    }

    int TotalSteps = 0;
    int2 Current = Goal;
    Visualization.fill('.');
    Visualization[CoordToIndex(Goal)] = 'E';

    while (std::find(PossibleOrigins.cbegin(), PossibleOrigins.cend(), Current) == PossibleOrigins.cend())
    {
        int2 Preceding = Current;
        Current = Previous[CoordToIndex(Current)];
        int2 Delta = Preceding - Current;
        TotalSteps++;

        Visualization[CoordToIndex(Current)] = Delta.X == -1 ? '<' : Delta.X == 1 ? '>' : Delta.Y == -1 ? '^' : 'v';
    }

    std::cout << "Found goal in " << TotalSteps << " steps.\n";

    Cursor = 0;
    for (int i = 0; i < GridHeight; i++)
    {
        for (int j = 0; j < GridWidth; j++)
            std::cout << Visualization[Cursor++];
        std::cout << '\n';
    }
}