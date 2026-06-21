#pragma once
#include <vector>
#include <string>

struct Signal
{
    int blockSID;
    int port;

    Signal(int _blockSID, int _port) : blockSID(_blockSID), port(_port) {}
    Signal()
    { 
        blockSID = 0; 
        port = 0;
    }
    static Signal parse(const std::string& str);
};

struct Line
{
    Signal source;
    std::vector<Signal> branches;

    bool hasBranches() const {return !branches.empty();}
};

