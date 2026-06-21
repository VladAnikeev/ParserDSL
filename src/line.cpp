#include "line.h"

#include <regex>
#include <iostream>

Signal Signal::parse(const std::string& str)
{
    // для варианта "17#in:1"
    std::regex patternSIDPort(R"((\d+)#(in|out):(\d+))");
    std::smatch matches;

    if (std::regex_search(str, matches, patternSIDPort))
    {
        if (matches.size() == 4)
        {
            int sid = std::stoi(matches[1]);
            int port = std::stoi(matches[3]);
            return Signal(sid, port);
        }
    }

    // для варианта "17#out"
    std::regex patternSID(R"((\d+)#(in|out))");

    if (std::regex_search(str, matches, patternSID))
    {
        if(matches.size() == 3)
        {
            int sid = std::stoi(matches[1]);
            return Signal(sid, 1);
        }
    }

    std::cout << "Error: Invalid signal format" << std::endl;
    return Signal(0, 0);
}

