#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <regex>
#include <unordered_map>
#include <bitset>

const int TotalMinutes = 30;

struct Valve
{
	std::string Name;
	int FlowRate;
	std::string TunnelsString;
	std::vector<Valve*> Tunnels;
	bool Opened = false;

	Valve(std::string Name, int FlowRate, std::string TunnelsString) : 
		Name(Name), FlowRate(FlowRate), TunnelsString(TunnelsString)
	{
	}
};

struct SimulationState
{
	int FlowRate = 0;
	int Pressure = 0;
	int ConsecutiveMoves = 0;
	int Minutes = 0;
	std::bitset<64> OpenedValves;
	Valve* Current = nullptr;
	std::string FullTrail;
	std::string PartialTrail;
};
bool operator==(const SimulationState& lhs, const SimulationState& rhs)
{
	return
		lhs.OpenedValves == rhs.OpenedValves &&
		lhs.Current == rhs.Current &&
		lhs.Minutes == rhs.Minutes;
}

namespace std
{
	template <>
	struct hash<SimulationState>
	{
		size_t operator()(const SimulationState& k) const
		{
			size_t res = 17;
			res = res * 31 + hash<uint64_t>()(k.OpenedValves.to_ullong());
			res = res * 31 + hash<Valve*>()(k.Current);
			res = res * 31 + hash<int>()(k.Minutes);
			return res;
		}
	};
}

int NonZeroValves = 0;
std::vector<Valve*> Valves;
std::unordered_map<SimulationState, SimulationState> PressureCache;

size_t ValveBit(Valve* Valve)
{
	return std::distance(Valves.begin(), std::find(Valves.begin(), Valves.end(), Valve));
}

SimulationState Simulate(SimulationState State);

Valve* Simulate(Valve* From, std::bitset<64> OpenedValves, int Minutes)
{
	SimulationState State;
	State.Minutes = Minutes;
	State.Current = From;
	State.OpenedValves = OpenedValves;
	State.FullTrail = From->Name;
	SimulationState Result = Simulate(State);

	//std::cout << Result.FullTrail << " = " << Result.Pressure << '\n';

	return Result.Current;
}

SimulationState Simulate(SimulationState State)
{
	// 30 Steps? We're outta time
	if (State.Minutes == TotalMinutes)
		return State;

	// Moving for more than 30 steps makes no sense (TODO: calculate max distance between valves from source data)
	if (State.ConsecutiveMoves > TotalMinutes)
		return State;

	// All valves open? Nothing to do
	if (State.OpenedValves.count() == NonZeroValves)
	{
		SimulationState IdleState = State;
		IdleState.Minutes++;
		IdleState.FullTrail += ">_";

		// We still need to keep the simulation going so that we account for remaining minutes
		SimulationState ResultState = Simulate(IdleState);

		ResultState.PartialTrail = ">_" + ResultState.PartialTrail;
		ResultState.Pressure += IdleState.FlowRate;

		return ResultState;
	}

	// Look for a prior cached result
	auto CachedResult = PressureCache.find(State);
	if (CachedResult != PressureCache.end())
		return CachedResult->second;

	SimulationState BestResult;
	bool HasResult = false;

	// Open
	if (!State.OpenedValves.test(ValveBit(State.Current)) && State.Current->FlowRate > 0)
	{
		SimulationState OptionResult = State;
		OptionResult.FlowRate += State.Current->FlowRate;
		OptionResult.OpenedValves.set(ValveBit(State.Current));
		OptionResult.ConsecutiveMoves = 0;
		OptionResult.Minutes++;
		OptionResult.FullTrail += ">+";

		BestResult = Simulate(OptionResult);
		
		BestResult.PartialTrail = ">+" + OptionResult.PartialTrail;
		BestResult.Current = State.Current;
		BestResult.Pressure += OptionResult.FlowRate;

		HasResult = true;
	}

	// Move
	for (Valve* Destination : State.Current->Tunnels)
	{
		SimulationState OptionInput(State);
		OptionInput.Current = Destination;
		OptionInput.ConsecutiveMoves++;
		OptionInput.Minutes++;
		OptionInput.FullTrail += ">" + Destination->Name;

		SimulationState OptionResult = Simulate(OptionInput);

		OptionResult.PartialTrail = ">" + Destination->Name + OptionResult.PartialTrail;
		OptionResult.Pressure += State.FlowRate;

		if (OptionResult.Pressure > BestResult.Pressure)
		{
			BestResult = OptionResult;
			BestResult.Current = Destination;
			HasResult = true;
		}
	}

	if (HasResult)
	{
		//std::cout << "ML=" << (TotalMinutes - State.Minutes) /*<< " FR=" << State.FlowRate*/ << " OV=[";
		//bool first = true;
		//for (int i=0; i<Valves.size(); i++)
		//	if (State.OpenedValves.test(ValveBit(Valves[i])))
		//	{
		//		if (!first) std::cout << "-";
		//		std::cout << Valves[i]->Name;
		//		first = false;
		//	}
		//std::cout << "] " << State.Current->Name << " => " << BestResult.PartialTrail << " = " << BestResult.Pressure << '\n';

		PressureCache[State] = BestResult;
		return BestResult;
	}

	return State;
}

void Day16()
{
	auto start = std::chrono::high_resolution_clock::now();

	std::ifstream InputStream;
	//InputStream.open("day16input.txt", std::ios::in);
	//InputStream.open("day16testinput.txt", std::ios::in);
	InputStream.open("day16testinput2.txt", std::ios::in);

	std::regex LineRegex("Valve ([A-Z]{2}) has flow rate=(\\d+); tunnels? leads? to valves? ([A-Z]{2}.*)+");

	for (std::string Line; std::getline(InputStream, Line); )
	{
		std::smatch Matches;
		std::regex_search(Line, Matches, LineRegex);

		Valves.emplace_back(new Valve(Matches[1], std::stoi(Matches[2]), Matches[3]));
	}

	// Connect valves
	for (Valve* SourceValve : Valves)
	{
		if (SourceValve->FlowRate > 0)
			NonZeroValves++;

		for (int i = 0; i < SourceValve->TunnelsString.size(); i += 4)
		{
			std::string ValveName = SourceValve->TunnelsString.substr(i, 2);

			Valve* ConnectedValve = *std::find_if(Valves.begin(), Valves.end(), 
				[&ValveName](const Valve* v) { return v->Name == ValveName; });

			SourceValve->Tunnels.emplace_back(ConnectedValve);
		}
	}

	std::cout << "There are " << NonZeroValves << " valves to open.\n\n";

	Valve* CurrentValve = Valves[0];
	int TotalFlowRate = 0;
	int TotalPressure = 0;
	std::bitset<64> OpenedValves;

	for (int i = 1; i <= TotalMinutes; i++)
	{
		std::cout << "== Minute " << i << " ==\n";
		if (OpenedValves.none())
			std::cout << "No valves are open.\n";
		else
		{
			std::vector<std::string> OpenedValveNames;
			for (int j = 0; j < Valves.size(); j++)
				if (OpenedValves.test(j))
					OpenedValveNames.emplace_back(Valves[j]->Name);

			if (OpenedValveNames.size() == 1)
				std::cout << "Valve " << OpenedValveNames[0] << " is open";
			else
			{
				std::cout << "Valves ";
				for (int j = 0; j < (int)OpenedValveNames.size() - 3; j++)
					std::cout << OpenedValveNames[j] << ", ";
				std::cout << OpenedValveNames[OpenedValveNames.size() - 2] << " and " << OpenedValveNames.back() << " are open";
			}
			std::cout << ", releasing " << TotalFlowRate << " pressure.\n";
		}

		TotalPressure += TotalFlowRate;

		if (OpenedValves.count() != NonZeroValves)
		{
			PressureCache.clear();
			Valve* BestDestination = Simulate(CurrentValve, OpenedValves, i);

			if (BestDestination == CurrentValve)
			{
				CurrentValve->Opened = true;
				TotalFlowRate += CurrentValve->FlowRate;
				std::cout << "You open valve " << CurrentValve->Name << ".\n";
				OpenedValves.set(ValveBit(CurrentValve));
			}
			else
			{
				CurrentValve = BestDestination;
				std::cout << "You move to valve " << CurrentValve->Name << ".\n";
			}
		}

		std::cout << "\n";
	}

	auto finish = std::chrono::high_resolution_clock::now();

	std::cout << "Released a total of " << TotalPressure << " pressure. (took "
		<< std::chrono::duration_cast<std::chrono::seconds>(finish - start).count()
		<< " seconds)\n";
}