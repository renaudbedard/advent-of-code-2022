#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <array>

enum class Operation
{
    Add,
    Multiply,
    Square
};

struct Monkey
{
    std::queue<int64_t> Items;
    Operation Operation;
    int Operand;
    int Divisor;
    int TrueTarget;
    int FalseTarget;
    int64_t InspectionCount;

    int64_t Transform(int64_t Item) const
    {
        switch (Operation)
        {
            case Operation::Add: Item += Operand; break;
            case Operation::Multiply: Item *= Operand; break;
            case Operation::Square: Item *= Item; break;
        }
        return Item;
    }
};

void Day11()
{
    std::ifstream InputStream;

    std::array<Monkey, 8> Monkeys;
    InputStream.open("day11input.txt", std::ios::in);

    //std::array<Monkey, 4> Monkeys;
    //InputStream.open("day11testinput.txt", std::ios::in);

    int64_t ProductOfDivisors = 1;
    int MonkeyIndex = 0;
    std::string Line;
    while (std::getline(InputStream, Line))
    {
        Monkey m = {};

        std::getline(InputStream, Line);
        size_t Offset = 18;
        while (Offset < Line.size())
        {
            std::string ItemPart = Line.substr(Offset, 2);
            m.Items.push(std::stoi(ItemPart));
            Offset += 4;
        }

        std::getline(InputStream, Line);
        switch (Line[23])
        {
        case '*':
            if (Line[25] == 'o')
                m.Operation = Operation::Square;
            else
            {
                m.Operation = Operation::Multiply;
                m.Operand = std::stoi(Line.substr(25));
            }
            break;
        case '+':
            m.Operation = Operation::Add;
            m.Operand = std::stoi(Line.substr(25));
            break;
        }

        std::getline(InputStream, Line);
        m.Divisor = std::stoi(Line.substr(21));
        ProductOfDivisors *= m.Divisor;

        std::getline(InputStream, Line);
        m.TrueTarget = std::stoi(Line.substr(29));

        std::getline(InputStream, Line);
        m.FalseTarget = std::stoi(Line.substr(30));

        Monkeys[MonkeyIndex++] = m;

        std::getline(InputStream, Line); // Read empty line
    }

    for (int Round = 0; Round < 10000; Round++)
    {
        for (MonkeyIndex = 0; MonkeyIndex < Monkeys.size(); MonkeyIndex++)
        {
            Monkey& m = Monkeys[MonkeyIndex];
            while (!m.Items.empty())
            {
                int64_t Item = m.Items.front();
                m.Items.pop();
                m.InspectionCount++;

                Item = m.Transform(Item) % ProductOfDivisors;

                if (Item % m.Divisor == 0)  Monkeys[m.TrueTarget].Items.push(Item);
                else                        Monkeys[m.FalseTarget].Items.push(Item);
            }
        }

        if (Round == 0 || Round == 19 || (Round + 1) % 1000 == 0)
        {
            std::cout << "== After round " << (Round + 1) << " ==\n";
            for (int i = 0; i < Monkeys.size(); i++)
                std::cout << "Monkey " << i << " inspected items " << Monkeys[i].InspectionCount << " times.\n";
            std::cout << '\n';
        }
    }

    std::sort(Monkeys.begin(), Monkeys.end(), 
        [](const Monkey& lhs, const Monkey& rhs) { return lhs.InspectionCount > rhs.InspectionCount; });

    std::cout << "Monkey business level of two most active monkeys : " << (Monkeys[0].InspectionCount * Monkeys[1].InspectionCount) << '\n';
}