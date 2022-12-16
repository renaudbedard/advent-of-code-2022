#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <regex>
#include <unordered_set>

#include "int2.h"

const int TestRow = 2000000;
const int MaxCoordinate = 4000000;

//const int TestRow = 10;
//const int MaxCoordinate = 20;

struct Sensor
{
	int2 Position;
	int Range;
	int2 ClosestBeacon;

	Sensor(const int2& Position, const int2& ClosestBeacon) : Position(Position), ClosestBeacon(ClosestBeacon)
	{
		Range = std::manhattan_distance(Position, ClosestBeacon);
	}
};

void Day15()
{
	auto start = std::chrono::high_resolution_clock::now();

	std::ifstream InputStream;

	InputStream.open("day15input.txt", std::ios::in);
	//InputStream.open("day15testinput.txt", std::ios::in);

	std::regex LineRegex("Sensor at x=(-?\\d+), y=(-?\\d+): closest beacon is at x=(-?\\d+), y=(-?\\d+)");

	std::vector<Sensor> Sensors;

	for (std::string Line; std::getline(InputStream, Line); )
	{
		std::smatch Matches;
		std::regex_search(Line, Matches, LineRegex);
		int2 Position(std::stoi(Matches[1]), std::stoi(Matches[2]));
		int2 ClosestBeacon(std::stoi(Matches[3]), std::stoi(Matches[4]));

		Sensors.emplace_back(Position, ClosestBeacon);
	}

	int MinColumn = std::numeric_limits<int>().max();
	int MaxColumn = std::numeric_limits<int>().min();

	for (const Sensor& Sensor : Sensors)
	{
		int2 TentativePosition(Sensor.Position.X, TestRow);
		int AlignedDistance = std::manhattan_distance(TentativePosition, Sensor.Position);

		if (AlignedDistance > Sensor.Range)
			continue;

		int HorizontalRange = Sensor.Range - AlignedDistance;
		MinColumn = std::min(MinColumn, TentativePosition.X - HorizontalRange);
		MaxColumn = std::max(MaxColumn, TentativePosition.X + HorizontalRange);
	}

	int NoBeaconPositions = 0;

	for (int x = MinColumn; x <= MaxColumn; x++)
	{
		int2 Position(x, TestRow);

		for (const Sensor& Sensor : Sensors)
		{
			if (Sensor.ClosestBeacon == Position)
				break;

			int Distance = std::manhattan_distance(Position, Sensor.Position);
			if (Distance <= Sensor.Range)
			{
				NoBeaconPositions++;
				break;
			}
		}
	}

	auto finish = std::chrono::high_resolution_clock::now();

	std::cout << "In row " << TestRow << ", " << NoBeaconPositions << " positions cannot contain a beacon. (took "
		<< std::chrono::duration_cast<std::chrono::microseconds>(finish - start).count()
		<< " microseconds)\n";

	start = std::chrono::high_resolution_clock::now();

	std::vector<int2> Candidates;

	for (const Sensor& s : Sensors)
	{
		for (int x = s.Position.X, y = s.Position.Y - s.Range - 1;
			x <= s.Position.X + s.Range + 1;
			x++, y++)
		{
			Candidates.emplace_back(x, y);
		}

		for (int x = s.Position.X - s.Range - 1, y = s.Position.Y;
			x <= s.Position.X;
			x++, y--)
		{
			Candidates.emplace_back(x, y);
		}

		for (int x = s.Position.X - s.Range - 1, y = s.Position.Y;
			x <= s.Position.X;
			x++, y++)
		{
			Candidates.emplace_back(x, y);
		}

		for (int x = s.Position.X, y = s.Position.Y + s.Range + 1;
			x <= s.Position.X + s.Range + 1;
			x++, y--)
		{
			Candidates.emplace_back(x, y);
		}
	}

	for (const int2& Candidate : Candidates)
	{
		if (Candidate.X < 0 || Candidate.X > MaxCoordinate ||
			Candidate.Y < 0 || Candidate.Y > MaxCoordinate)
		{
			continue;
		}

		bool Eliminated = false;
		for (const Sensor& Sensor : Sensors)
		{
			int Distance = std::manhattan_distance(Candidate, Sensor.Position);
			if (Distance <= Sensor.Range)
			{
				Eliminated = true;
				break;
			}
		}
		if (!Eliminated)
		{
			uint64_t TuningFrequency = (uint64_t)Candidate.X * 4000000 + Candidate.Y;

			auto finish = std::chrono::high_resolution_clock::now();

			std::cout << "Found it! At : (" << Candidate.X << ", " << Candidate.Y << 
				") -- Tuning Frequency : " << TuningFrequency << " (took "
				<< std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count()
				<< " ms)\n";
			break;
		}
	}
}