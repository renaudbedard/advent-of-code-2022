#define PART_TWO
#define VISUALIZATION

#ifdef VISUALIZATION
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#endif

#include <iostream>
#include <fstream>
#include <string>
#include <unordered_set>
#include <regex>
#include <thread>
#include <chrono>

#include "int2.h"

#ifdef VISUALIZATION
void WriteAt(std::wstring str, int2 pos)
{
	COORD coord = { (SHORT) pos.X, (SHORT) pos.Y };
	HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD dwBytesWritten = 0;

	SetConsoleMode(output, ENABLE_VIRTUAL_TERMINAL_PROCESSING | ENABLE_PROCESSED_OUTPUT);
	SetConsoleCursorPosition(output, coord);
	WriteConsole(output, str.c_str(), (DWORD) str.size(), &dwBytesWritten, nullptr);
}
#endif

void Day14()
{
	auto start = std::chrono::high_resolution_clock::now();

	std::unordered_set<int2> Obstacles;
	std::unordered_set<int2> RestingSand;
	std::vector<int2*> FallingSand;

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

	auto IsBlocked = [&](int2 Coord)
	{
#ifdef PART_TWO
		return Coord.Y == Max.Y || Obstacles.find(Coord) != Obstacles.end();
#else
		return Obstacles.find(Coord) != Obstacles.end();
#endif
	};

#ifdef VISUALIZATION
	auto RefreshVisual = [&](int2 Coord, bool IsFallingSand = false)
	{
		int2 ScreenCoord = Coord - Min + int2(4, 0);

		if (ScreenCoord.X == 0)
			WriteAt(std::to_wstring(Coord.Y), ScreenCoord);
		else if (ScreenCoord.X < 3)
			return;
		else if (ScreenCoord.X == 3)
			WriteAt(L" ", ScreenCoord);
		else if (Coord == SandEntryPoint)
			WriteAt(L"\033[33m█\033[0m", ScreenCoord);
		else if (IsFallingSand)
			WriteAt(L"\033[93;1m■\033[0m", ScreenCoord);
		else if (RestingSand.find(Coord) != RestingSand.end())
			WriteAt(L"\033[33m█\033[0m", ScreenCoord);
		else if (IsBlocked(Coord))
			WriteAt(L"\033[94m█\033[0m", ScreenCoord);
		else
			WriteAt(L"\033[90m·\033[0m", ScreenCoord);
	};

	// Draw caves
	for (int y = Min.Y; y <= Max.Y; y++)
		for (int x = Max.X; x >= Min.X - 4; x--)
			RefreshVisual(int2(x, y));
#endif

	int UnitsSpawned = 0;
	bool ReachedAbyss = false;
	bool EntryBlocked = false;
	int SpawnIn = 0;

	std::this_thread::sleep_for(std::chrono::milliseconds(6000));

	while (!EntryBlocked && !ReachedAbyss)
	{
		if (SpawnIn == 0)
		{
			SpawnIn = 1;
			FallingSand.push_back(new int2(SandEntryPoint));
			UnitsSpawned++;
		}
		else
			SpawnIn--;
		
		bool AtRest = false;

		for (auto SandIt = FallingSand.begin(); SandIt != FallingSand.end(); ++SandIt)
		{
			int2* Sand = *SandIt;

			int2 LastPos = *Sand;
			bool Deleted = false;

			// Fall
			Sand->Y++;

			if (Sand->Y > Max.Y)
			{
				UnitsSpawned--; // Don't count the one that reached the abyss
				ReachedAbyss = true;
				break;
			}

			if (IsBlocked(*Sand))
			{
				// Try moving left
				Sand->X -= 1;
				if (IsBlocked(*Sand))
				{
					// Try moving right
					Sand->X += 2;
					if (IsBlocked(*Sand))
					{
						// Recover position
						*Sand += int2(-1, -1);
						AtRest = true;
						RestingSand.insert(*Sand);
						Obstacles.insert(*Sand); // Count resting sand as obstacles to avoid two collision lookups

						if (*Sand == SandEntryPoint)
							EntryBlocked = true;

						SandIt = FallingSand.erase(SandIt);
						delete Sand;
						Deleted = true;
					}
				}
			}

#ifdef VISUALIZATION
			RefreshVisual(LastPos);
			if (!Deleted)
				RefreshVisual(*Sand, true);
#endif
		}
	}

#ifdef VISUALIZATION
	// Move past the viz
	for (int i = Min.Y; i <= Max.Y + 1; i++)
		std::cout << '\n';
#endif

	auto finish = std::chrono::high_resolution_clock::now();

	std::cout << "It took " << UnitsSpawned << " units of sand to reach the abyss/block the entry. (took "
		<< std::chrono::duration_cast<std::chrono::microseconds>(finish - start).count()
		<< " microseconds)\n";
}