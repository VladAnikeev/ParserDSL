#pragma once
#include "block_manager.h"

#include <iostream>


void BlockManager::addBlock(const Block& block)
{
    blocksBySID[block.sid] = block;
    nameToSID[block.name] = block.sid;
}

const Block* BlockManager::getBlockBySID(int sid) const
{
    auto it = blocksBySID.find(sid);
    if (it != blocksBySID.end())
    {
        return &it->second;
    }
    return nullptr;
}

const Block* BlockManager::getBlockByName(const std::string& name) const
{
    auto it = nameToSID.find(name);
    if (it != nameToSID.end())
    {
        return getBlockBySID(it->second);
    }
    return nullptr;
}

void BlockManager::printAll() const
{
    std::cout << "\n=== BLOCKS ===" << std::endl;
    for (const auto& [sid, block] : blocksBySID)
    {
        std::cout << "SID: " << sid << ", Name: " << block.name << std::endl;
        std::cout << "  " << "type: " << Block::blockTypeToString(block.type) << std::endl;
        for (const auto& [key, value] : block.parameters)
        {
            std::cout << "  " << key << ": " << value << std::endl;
        }
        std::cout << std::endl;
    }
}


Block BlockManager::parseBlock(pugi::xml_node blockNode)
{
    Block block;
    const std::string POSITION = "Position";

    // удаление пробелов из имени блока
    std::string name = blockNode.attribute("Name").as_string();
    name.erase(std::remove(name.begin(), name.end(), ' '), name.end());

    block.name = name;
    block.sid = blockNode.attribute("SID").as_int();
    block.type = Block::stringToBlockType(blockNode.attribute("BlockType").as_string());

    for (pugi::xml_node paramNode : blockNode.children("P"))
    {   
        std::string key = paramNode.attribute("Name").as_string();
        if(key != POSITION)
        {
            std::string value = paramNode.text().as_string();
            block.parameters[key] = value;
        }        
    }

    for (pugi::xml_node portNode : blockNode.children("Port"))
    {
        for (pugi::xml_node pNode : portNode.children("P")) {
            std::string key = "Port_" + std::string(pNode.attribute("Name").as_string());
            std::string value = pNode.text().as_string();
            block.parameters[key] = value;
        }
    }

    return block;
}

const std::unordered_map<int, Block>& BlockManager::getBlocksBySID() const
{
    return blocksBySID;
}

const std::unordered_map<std::string, int>& BlockManager::getNameToSID() const
{
    return nameToSID;
}