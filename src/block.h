#pragma once
#include <string>
#include <unordered_map>

enum class BlockType
{
    None,
    Inport,
    Outport,
    Sum,
    Gain,
    UnitDelay
};

namespace ParamKeys
{
    constexpr const char *Inputs = "Inputs";
    constexpr const char *Gain = "Gain";
}

struct Block
{
    std::string name;
    BlockType type;
    int sid;

    std::unordered_map<std::string, std::string> parameters;

    int getInt(const std::string &key, int defaultVal = 0) const;
    double getDouble(const std::string &key, double defaultVal = 0.0) const;
    std::string getString(const std::string &key, const std::string &defaultVal = "") const;

    std::string getParam(const std::string &nameParam) const;

    static BlockType stringToBlockType(const std::string &str);
    static std::string blockTypeToString(const BlockType &type);
};
