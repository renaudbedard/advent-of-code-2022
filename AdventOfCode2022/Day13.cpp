#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

// Generic signum function from https://stackoverflow.com/a/4609795/1721329
template <typename T> int sgn(T v) { return (T(0) < v) - (v < T(0)); }

struct Packet
{
	std::vector<Packet> SubPackets;
	int Value = -1;

	Packet() = default;
	Packet(int Value) : Value(Value) { }

	Packet GetWrappedPacket() const
	{
		Packet Promoted(-1);
		Promoted.SubPackets.emplace_back(Value);
		return Promoted;
	}

	void Print() const
	{
		if (Value != -1)
		{
			std::cout << Value;
			return;
		}
		std::cout << "[";
		for (int i = 0; i<SubPackets.size(); i++)
		{
			SubPackets[i].Print();
			if (i < SubPackets.size() - 1)
				std::cout << ",";
		}
		std::cout << "]";
	}
};

int ComparePackets(const Packet& lhs, const Packet& rhs, std::string Prefix = "")
{
	std::cout << Prefix << "- Compare ";
	lhs.Print();
	std::cout << " vs ";
	rhs.Print();
	std::cout << "\n";
	Prefix = "  " + Prefix;

	if (lhs.Value != -1 && rhs.Value != -1)
	{
		// Both values are integers
		int result = sgn(lhs.Value - rhs.Value);
		if (result == -1)
			std::cout << Prefix << "- Left side is smaller, so inputs are in the right order\n";
		else if (result == 1)
			std::cout << Prefix << "- Right side is smaller, so inputs are NOT in the right order\n";
		return result;
	}
	else if (lhs.Value == -1 && rhs.Value == -1)
	{
		// Both values are lists
		for (int i = 0; i < lhs.SubPackets.size(); i++)
		{
			if (rhs.SubPackets.size() == i)
			{
				std::cout << Prefix << "- Right side ran out of items, so input are NOT in the right order\n";
				return 1;
			}
			int result = ComparePackets(lhs.SubPackets[i], rhs.SubPackets[i], Prefix);
			if (result != 0)
				return result;
		}
		if (rhs.SubPackets.size() > lhs.SubPackets.size())
		{
			std::cout << Prefix << "- Left side ran out of items, so input are in the right order\n";
			return -1;
		}
		return 0;
	}
	else
	{
		// Mismatch
		std::cout << Prefix << "- Mixed types; convert ";
		if (lhs.Value != -1)
		{
			std::cout << "left to [" << lhs.Value << "] and retry comparison\n";
			return ComparePackets(lhs.GetWrappedPacket(), rhs, Prefix);
		}
		else if (rhs.Value != -1)
		{
			std::cout << "right to [" << rhs.Value << "] and retry comparison\n";
			return ComparePackets(lhs, rhs.GetWrappedPacket(), Prefix);
		}
	}
}

Packet ReadPacket(std::string PacketString, size_t& Cursor)
{
	Packet p;
	if (std::isdigit(PacketString[Cursor]))
	{
		size_t NextMarker = PacketString.find_first_of(",]", Cursor);
		p.Value = std::stoi(PacketString.substr(Cursor, NextMarker - Cursor));
		Cursor = NextMarker;
	}
	else if (PacketString[Cursor] == '[') 
	{
		do
		{
			Cursor++;
			if (PacketString[Cursor] != ']')
				p.SubPackets.push_back(ReadPacket(PacketString, Cursor));
		} while (PacketString[Cursor] == ',');
		Cursor++; // Eat the ]
	}
	return p;
}

void Day13()
{
	std::ifstream InputStream;

	InputStream.open("day13input.txt", std::ios::in);
	//InputStream.open("day13testinput.txt", std::ios::in);

	size_t Cursor = 0;
	Packet* FirstDivider = new Packet(ReadPacket("[[2]]", Cursor));
	Cursor = 0;
	Packet* SecondDivider = new Packet(ReadPacket("[[6]]", Cursor));

	std::vector<Packet*> AllPackets = { FirstDivider, SecondDivider };

	int SumOfOrderedPairIndices = 0;
	int PairIndex = 1;
	std::string Line;
	while (true)
	{
		if (!std::getline(InputStream, Line))
			break;

		std::cout << "== Pair " << PairIndex << " ==\n";

		Cursor = 0;
		Packet FirstPacket = ReadPacket(Line, Cursor);
		AllPackets.push_back(new Packet(FirstPacket));

		std::getline(InputStream, Line);
		Cursor = 0;
		Packet SecondPacket = ReadPacket(Line, Cursor);
		AllPackets.push_back(new Packet(SecondPacket));

		int ComparisonResult = ComparePackets(FirstPacket, SecondPacket, "  ");
		if (ComparisonResult == 0)
			throw new std::runtime_error("Could not determine order!");
		if (ComparisonResult == -1)
			SumOfOrderedPairIndices += PairIndex;

		std::getline(InputStream, Line); // Read empty line

		std::cout << "\n";
		PairIndex++;
	}

	std::cout << "Sum of ordered pair indices is : " << SumOfOrderedPairIndices << "\n";

	// Mute cout while we do a massive amount of comparisons
	std::cout.setstate(std::ios_base::badbit);

	std::sort(AllPackets.begin(), AllPackets.end(), [](const Packet* lhs, const Packet* rhs) {
		return ComparePackets(*lhs, *rhs) < 0;
	});

	// Recover cout
	std::cout.clear();

	std::cout << "\n";
	for (Packet* Packet : AllPackets)
	{
		Packet->Print();
		std::cout << "\n";
	}

	size_t FirstDividerPosition = std::distance(AllPackets.begin(), std::find(AllPackets.begin(), AllPackets.end(), FirstDivider)) + 1;
	size_t SecondDividerPosition = std::distance(AllPackets.begin(), std::find(AllPackets.begin(), AllPackets.end(), SecondDivider)) + 1;

	std::cout << "\nThe decoder key is : " << FirstDividerPosition * SecondDividerPosition << "\n";
}