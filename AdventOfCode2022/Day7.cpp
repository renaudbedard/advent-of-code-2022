#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <regex>
#include <vector>
#include <functional>

struct File
{
    std::string Name {};
    File* Parent {};
    File** Children {};
    size_t ChildCount {};
    uint64_t Size {};
    uint64_t TotalSize {};

    File* FindChild(std::string ChildName) const
    {
        for (int i = 0; i < ChildCount; i++) 
            if (Children[i]->Name == ChildName)
                return Children[i];

        throw new std::runtime_error("Child not found");
    }

    uint64_t CalculateTotalSizeRecursively()
    {
        TotalSize = Size;

        for (int i = 0; i < ChildCount; i++)
        {
            Children[i]->CalculateTotalSizeRecursively();
            TotalSize += Children[i]->TotalSize;
        }

        return TotalSize;
    }
};

void Day7()
{
    std::ifstream InputStream;
    InputStream.open("day7input.txt", std::ios::in);

    File Root = {};
    Root.Name = "/";
    File* CurrentDirectory = &Root;

    std::regex FileRegex("(\\d+) ([a-z\\.]+)");
    std::string Statement;
    std::vector<File*> ChildrenWorkingList;

#define READ_STATEMENT() \
    if (!getline(InputStream, Statement) || Statement.length() == 0) \
        goto StopParsing;

    READ_STATEMENT();

    while (true)
    {
        if (Statement[0] != '$') 
            throw new std::runtime_error("Command expected");

        if (Statement[2] == 'c' && Statement[3] == 'd')
        {
            std::string NewDirectory = Statement.substr(5);
            if (NewDirectory == "/")
                CurrentDirectory = &Root;
            else if (NewDirectory == "..")
                CurrentDirectory = CurrentDirectory->Parent;
            else
                CurrentDirectory = CurrentDirectory->FindChild(NewDirectory);

            READ_STATEMENT();
        }
        else if (Statement[2] == 'l' && Statement[3] == 's')
        {
            ChildrenWorkingList.clear();

            while (true)
            {
                READ_STATEMENT();

                if (Statement[0] == '$')
                    break;

                File* Child = new File();
                Child->Parent = CurrentDirectory;

                std::smatch Matches;
                if (std::regex_search(Statement, Matches, FileRegex))
                {
                    Child->Size = std::stoi(Matches[1]);
                    Child->Name = Matches[2];
                    ChildrenWorkingList.push_back(Child);
                }
                else
                {
                    Child->Name = Statement.substr(4);
                    ChildrenWorkingList.push_back(Child);
                }
            }

            size_t ChildCount = ChildrenWorkingList.size();
            CurrentDirectory->Children = new File*[ChildCount];
            memcpy(CurrentDirectory->Children, ChildrenWorkingList.data(), sizeof(File*) * ChildCount);
            CurrentDirectory->ChildCount = ChildCount;
        }
    }

StopParsing:
    Root.CalculateTotalSizeRecursively();

    // Part 1
    uint64_t WorkingSum = 0;

    std::function<void(File*)> SumRecursively;
    SumRecursively = [&](File* DirCursor)
    {
        if (DirCursor->TotalSize <= 100000)
            WorkingSum += DirCursor->TotalSize;

        for (int i = 0; i < DirCursor->ChildCount; i++)
            if (DirCursor->Children[i]->ChildCount > 0)
                SumRecursively(DirCursor->Children[i]);
    };

    SumRecursively(&Root);
    std::cout << "The sum of the total size of all directories with a total size of at least 100000 is : " << WorkingSum;

    // Part 2
    const uint64_t DiskSize = 70000000;
    const uint64_t NeededSpace = 30000000;
    uint64_t SpaceToFree = NeededSpace - (DiskSize - Root.TotalSize);
    uint64_t ClosestSize = UINT64_MAX;

    std::function<void(File*)> FindClosestRecursively;
    FindClosestRecursively = [&](File* DirCursor)
    {
        if (DirCursor->TotalSize >= SpaceToFree && DirCursor->TotalSize < ClosestSize)
            ClosestSize = DirCursor->TotalSize;

        for (int i = 0; i < DirCursor->ChildCount; i++)
            if (DirCursor->Children[i]->ChildCount > 0)
                FindClosestRecursively(DirCursor->Children[i]);
    };

    FindClosestRecursively(&Root);
    std::cout << "\nThe size of the smallest drectory that would give us enough space is : " << ClosestSize;
}