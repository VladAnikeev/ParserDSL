#pragma once
#include "pugixml.hpp"
#include "line.h"

#include <vector>
#include <unordered_map>


class LineManager
{
private:
    std::vector<Line> lines;
    std::unordered_map<int, std::vector<int>>  sourceToLines;
public:

    void addLine(const Line& line);

    static Line parseLine(pugi::xml_node lineNode);

    const std::vector<Line>& getLines() const;

    void printAll() const;
};