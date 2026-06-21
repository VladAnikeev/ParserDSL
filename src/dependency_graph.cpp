#include "dependency_graph.h"

void DependencyGraph::buildDependencies(const std::vector<Line> &lines)
{
    for (const Line &line : lines)
    {
        for (const Signal &branch : line.branches)
        {
            int targetSID = branch.blockSID;
            int targetPort = branch.port;
            deps[targetSID][targetPort] = line.source.blockSID;
        }
    }
}
// dependency_graph.cpp
bool DependencyGraph::topologicalSort(const std::unordered_map<int, Block> &blocksBySID)
{
    order.clear(); // на случай повторного вызова
    // Создаем set зависимостей для каждого блока (только SID источников)
    std::unordered_map<int, std::unordered_set<int>> depSet;
    for (const auto &[sid, portMap] : deps)
    {
        for (const auto &[port, sourceSID] : portMap)
        {
            depSet[sid].insert(sourceSID);
        }
    }
        
    std::queue<int> ready;
    for (const auto &[sid, block] : blocksBySID)
    {
        if (block.type == BlockType::UnitDelay)
        {
            depSet[sid].clear();
        }
        if (depSet[sid].empty())
        {
            ready.push(sid);
        }
    }

    while (!ready.empty())
    {
        int current = ready.front();
        ready.pop();
        order.push_back(current);

        for (auto &[sid, dependencies] : depSet)
        {
            if (dependencies.erase(current) > 0 && dependencies.empty())
            {
                ready.push(sid);
            }
        }
            
    }

    return order.size() == blocksBySID.size();
}