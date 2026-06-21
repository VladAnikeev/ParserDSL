#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <map>
#include <sstream>

// Forward declarations
enum class BlockType;
struct Block;
class BlockManager;
class CodeGenerator

{
private:
    static constexpr char TAB = '\t';
    static constexpr const char *STATE = "state";

    static std::string stateRef(const std::string &name);

    static std::string assembleFunctionBody(const std::string &body, const std::string &delays);

    static void generateSumAssignment(const Block &block, const std::map<int, int> &currentDeps,
                                      const BlockManager &blockManager, std::stringstream &out);

    static void generateGainAssignment(const Block &block, const std::map<int, int> &currentDeps,
                                       const BlockManager &blockManager, std::stringstream &out);

    static void generateUnitDelayAssignment(const Block &block, const std::map<int, int> &currentDeps,
                                            const BlockManager &blockManager, std::stringstream &out);
    static std::string formatSummand(int index, char sign, const std::string &name);

public:
    static std::string generateStruct(const std::unordered_map<int, Block> &blocksBySID);

    static std::string generateFunctionInit(const std::unordered_map<int, Block> &blocksBySID);

    static std::string generateExtPort(
        const std::unordered_map<int, Block> &blocksBySID,
        const BlockManager &blockManager,
        const std::unordered_map<int, std::map<int, int>> &deps);

    static std::string generateFunctionStep(
        const std::vector<int> &sortedOrder,
        const BlockManager &blockManager,
        const std::unordered_map<int, std::map<int, int>> &deps);

    static std::string generateExtPortFooter();

    static std::string generate(
        const std::vector<int> &sortedOrder,
        const BlockManager &blockManager,
        const std::unordered_map<int, std::map<int, int>> &deps,
        const std::string &outputPath);
};