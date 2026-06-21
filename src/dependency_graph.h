#pragma once

#include "line.h"
#include "block.h"

#include <unordered_map>
#include <unordered_set>
#include <map>
#include <vector>
#include <queue>

class DependencyGraph
{
public:
    // inputs[17] = {1: 16, 2: 18} - порт 1 = источник 16, порт 2 = источник 18
    std::unordered_map<int, std::map<int, int>> deps;
    std::vector<int> order;

    void buildDependencies(const std::vector<Line> &lines);
    bool topologicalSort(const std::unordered_map<int, Block> &blocksBySID);
};