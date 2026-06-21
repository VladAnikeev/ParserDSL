#include "block.h"
#include <stdexcept>

int Block::getInt(const std::string &key, int defaultVal) const
{
    auto it = parameters.find(key);
    if (it != parameters.end())
    {
        try
        {
            return std::stoi(it->second);
        }
        catch (const std::exception &)
        {
            return defaultVal;
        }
    }
    return defaultVal;
}

double Block::getDouble(const std::string &key, double defaultVal) const
{
    auto it = parameters.find(key);
    if (it != parameters.end())
    {
        try
        {
            return std::stod(it->second);
        }
        catch (const std::exception &)
        {
            return defaultVal;
        }
    }
    return defaultVal;
}

std::string Block::getString(const std::string &key, const std::string &defaultVal) const
{
    auto it = parameters.find(key);
    if (it != parameters.end())
    {
        return it->second;
    }
    return defaultVal;
}

std::string Block::getParam(const std::string &key) const
{
    auto it = parameters.find(key);

    if (it != parameters.end())
    {
        return it->second;
    }
    return "";
}

BlockType Block::stringToBlockType(const std::string &str)
{
    static const std::unordered_map<std::string, BlockType> map =
        {
            {"None", BlockType::None},
            {"Inport", BlockType::Inport},
            {"Outport", BlockType::Outport},
            {"Sum", BlockType::Sum},
            {"Gain", BlockType::Gain},
            {"UnitDelay", BlockType::UnitDelay}};

    auto it = map.find(str);
    if (it != map.end())
    {
        return it->second;
    }
    return BlockType::None;
}

std::string Block::blockTypeToString(const BlockType &type)
{
    static const std::unordered_map<BlockType, std::string> map =
        {
            {BlockType::None, "None"},
            {BlockType::Inport, "Inport"},
            {BlockType::Outport, "Outport"},
            {BlockType::Sum, "Sum"},
            {BlockType::Gain, "Gain"},
            {BlockType::UnitDelay, "UnitDelay"}};

    auto it = map.find(type);
    if (it != map.end())
    {
        return it->second;
    }
    return "None";
}
