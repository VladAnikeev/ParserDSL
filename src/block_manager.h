#pragma once
#include "pugixml.hpp"
#include "block.h"
#include <unordered_map>
#include <iostream>

//! NAME и SID должны быть уникальными
class BlockManager
{
private:
    // {SID: блок}
    std::unordered_map<int, Block> blocksBySID;
    // {ИМЯ: SID}
    std::unordered_map<std::string, int> nameToSID;
public:

    void            addBlock(const Block& block);
    
    const Block*    getBlockBySID(int sid) const;
    const Block*    getBlockByName(const std::string& name) const;

    const std::unordered_map<int, Block>& getBlocksBySID() const;
    const std::unordered_map<std::string, int>& getNameToSID() const;

    void            printAll() const;
    static Block    parseBlock(pugi::xml_node blockNode);
};


