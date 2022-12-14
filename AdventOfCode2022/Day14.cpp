#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

#include <iostream>
#include <fstream>
#include <string>
#include <unordered_set>
#include <regex>
#include <thread>
#include <chrono>

#include "int2.h"

#define PART_TWO

void WriteAt(std::string str, int2 pos) 
{
	std::wstring wideStr(str.begin(), str.end());

	COORD coord = { (SHORT) pos.X, (SHORT) pos.Y };
	HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD dwBytesWritten = 0;
	WriteConsoleOutputCharacter(output, wideStr.c_str(), (DWORD) str.size(), coord, &dwBytesWritten);
}

void Day14()
{
	std::unordered_set<int2> Obstacles;
	std::unordered_set<int2> RestingSand;

	std::ifstream InputStream;
	std::regex PathRegex("(\\d+),(\\d+)( ->)?");

	InputStream.open("day14input.txt", std::ios::in);
	//InputStream.open("day14testinput.txt", std::ios::in);

	int2 Min(std::numeric_limits<int>::max());
	int2 Max(std::numeric_limits<int>::min()); 

	int2 SandEntryPoint(500, 0);

	for (std::string Line; std::getline(InputStream, Line); )
	{
		std::vector<int2> Points;

		std::smatch Matches;
		while (std::regex_search(Line, Matches, PathRegex))
		{
			Points.emplace_back(std::stoi(Matches[1]), std::stoi(Matches[2]));
			Line = Matches.suffix();

			Min = std::min(Min, Points.back());
			Max = std::max(Max, Points.back());
		}

		int2 Origin = Points[0];
		for (int i = 1; i < Points.size(); i++)
		{
			int2 Destination = Points[i];
			int Length = std::cmax(std::abs(Destination - Origin));
			int2 Delta = std::sgn(Destination - Origin);

			for (int j = 0; j <= Length; j++)
				Obstacles.insert(Origin + Delta * j);

			Origin = Destination;
		}
	}

	// Consider sand entry point as well
	Min = std::min(Min, SandEntryPoint);
	Max = std::max(Max, SandEntryPoint);

#ifdef PART_TWO
	// Consider the infinite floor
	Max.Y += 2;
#endif

	int2 FallingSand = SandEntryPoint;

	auto IsBlocked = [&](int2 Coord)
	{
#ifdef PART_TWO
		return Coord.Y == Max.Y || Obstacles.find(FallingSand) != Obstacles.end();
#else
		return Obstacles.find(FallingSand) != Obstacles.end();
#endif
	};

	auto RefreshVisual = [&](int2 Coord)
	{
		int2 ScreenCoord = Coord - Min + int2(4, 0);

		if (ScreenCoord.X == 0)
			WriteAt(std::to_string(Coord.Y), ScreenCoord);
		else if (ScreenCoord.X < 3)
			return;
		else if (ScreenCoord.X == 3)
			WriteAt(" ", ScreenCoord);
		else if (Coord == SandEntryPoint)
			WriteAt("+", ScreenCoord);
		else if (Coord == FallingSand || RestingSand.find(Coord) != RestingSand.end())
			WriteAt("o", ScreenCoord);
		else if (IsBlocked(Coord))
			WriteAt("#", ScreenCoord);
		else
			WriteAt(".", ScreenCoord);
	};

	// Draw caves
	for (int y = Min.Y; y <= Max.Y; y++)
		for (int x = Min.X - 4; x <= Max.X; x++)
			RefreshVisual(int2(x, y));

	int UnitsSpawned = 0;
	bool ReachedAbyss = false;
	bool EntryBlocked = false;
	int2 LastPos = SandEntryPoint;

	while (!EntryBlocked && !ReachedAbyss)
	{
		// Spawn sand
		FallingSand = SandEntryPoint;
		UnitsSpawned++;
		bool AtRest = false;

		while (!AtRest && !(EntryBlocked || ReachedAbyss))
		{
			// Fall
			FallingSand += int2(0, 1);

			if (FallingSand.Y > Max.Y)
			{
				UnitsSpawned--; // Don't count the one that reached the abyss
				ReachedAbyss = true;
			}

			if (IsBlocked(FallingSand))
			{
				// Try moving left
				FallingSand.X -= 1;
				if (IsBlocked(FallingSand))
				{
					// Try moving right
					FallingSand.X += 2;
					if (IsBlocked(FallingSand))
					{
						// Recover position
						FallingSand += int2(-1, -1);
						AtRest = true;
						RestingSand.insert(FallingSand);
						Obstacles.insert(FallingSand); // Count resting sand as obstacles to avoid two collision lookups

						if (FallingSand == SandEntryPoint)
							EntryBlocked = true;
					}
				}
			}
		}

		// Refresh viz
		RefreshVisual(LastPos);
		RefreshVisual(FallingSand);
		LastPos = FallingSand;

		// For visualization purposes
		//std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	// Move past the viz
	for (int i = Min.Y; i <= Max.Y + 1; i++)
		std::cout << '\n';
	std::cout << "It took " << UnitsSpawned << " units of sand to reach the abyss.\n";
}