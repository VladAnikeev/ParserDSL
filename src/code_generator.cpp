#include "code_generator.h"
#include "block_manager.h"
#include "block.h"

#include <fstream>

std::string CodeGenerator::stateRef(const std::string &name)
{
    return std::string(STATE) + '.' + name;
}

std::string CodeGenerator::generateStruct(const std::unordered_map<int, Block> &blocksBySID)
{
    std::stringstream structCode;

    structCode << "#include \"nwocg_run.h\"\n";
    structCode << "#include <math.h>\n\n";

    structCode << "static struct\n";
    structCode << "{\n";

    for (const auto &[sid, block] : blocksBySID)
    {
        if (block.type == BlockType::Outport)
        {
            continue;
        }
        structCode << TAB << "double " << block.name << ";\n";
    }

    structCode << "} " << STATE << ";\n\n";

    return structCode.str();
}

std::string CodeGenerator::generateFunctionInit(const std::unordered_map<int, Block> &blocksBySID)
{
    std::stringstream functionInit;

    functionInit << "void generated_init()\n";
    functionInit << "{\n";
    for (const auto &[sid, block] : blocksBySID)
    {
        if (block.type == BlockType::UnitDelay)
        {
            functionInit << TAB << stateRef(block.name) << " = 0;\n";
        }
    }
    functionInit << "}\n\n";

    return functionInit.str();
}

std::string CodeGenerator::generateExtPort(
    const std::unordered_map<int, Block> &blocksBySID,
    const BlockManager &blockManager,
    const std::unordered_map<int, std::map<int, int>> &deps)
{
    std::stringstream extPort;

    extPort << "static const ExtPort\n";
    extPort << TAB << "ext_ports[] =\n";
    extPort << "{\n";

    for (const auto &[sid, block] : blocksBySID)
    {
        if (block.type == BlockType::Inport)
        {
            extPort << TAB << "{ \"" << block.name << "\", &" << stateRef(block.name) << ", 1 },\n";
        }
        else if (block.type == BlockType::Outport)
        {
            auto depsIt = deps.find(sid);
            if (depsIt == deps.end())
            {
                continue;
            }

            const std::map<int, int> &currentDeps = depsIt->second;
            if (currentDeps.empty())
            {
                continue;
            }
            int depSID = currentDeps.begin()->second;

            const Block *blockDep = blockManager.getBlockBySID(depSID);

            if (blockDep == nullptr)
            {
                continue;
            }

            extPort << TAB << "{ \"" << block.name << "\", &" << stateRef(blockDep->name) << ", 0 },\n";
        }
    }
    extPort << TAB << "{ 0, 0, 0 },\n";
    extPort << "};\n\n";

    return extPort.str();
}

/**
 * Первый плюс пропускается, а в остальном пишутся знаки
 */
std::string CodeGenerator::formatSummand(int index, char sign, const std::string &name)
{
    if (index == 0)
    {
        return sign == '-' ? "-" + name : name;
    }
    return std::string(" ") + sign + " " + name;
}

void CodeGenerator::generateSumAssignment(
    const Block &block, const std::map<int, int> &currentDeps,
    const BlockManager &blockManager, std::stringstream &out)
{
    std::string signs = block.getParam(ParamKeys::Inputs);

    out << TAB << stateRef(block.name) << " = ";

    int signIndex = 0;
    int printedCount = 0;
    for (const auto &[inputPort, depSID] : currentDeps)
    {
        const Block *blockDep = blockManager.getBlockBySID(depSID);
        if (blockDep == nullptr)
        {
            signIndex++;
            continue;
        }

        char sign = (signIndex < static_cast<int>(signs.size())) ? signs[signIndex] : '+';
        out << formatSummand(printedCount, sign, stateRef(blockDep->name));

        signIndex++;
        printedCount++;
    }
    out << ";\n";
}

void CodeGenerator::generateGainAssignment(
    const Block &block, const std::map<int, int> &currentDeps,
    const BlockManager &blockManager, std::stringstream &out)
{
    const Block *blockDep = blockManager.getBlockBySID(currentDeps.begin()->second);
    if (blockDep == nullptr)
    {
        return;
    }

    const std::string &gain = block.getParam(ParamKeys::Gain);
    if (gain.empty())
    {
        return;
    }

    out << TAB << stateRef(block.name) << " = "
        << stateRef(blockDep->name) << " * " << gain << ";\n";
}

void CodeGenerator::generateUnitDelayAssignment(
    const Block &block, const std::map<int, int> &currentDeps,
    const BlockManager &blockManager, std::stringstream &out)
{
    const Block *blockDep = blockManager.getBlockBySID(currentDeps.begin()->second);
    if (blockDep == nullptr)
    {
        return;
    }

    out << TAB << stateRef(block.name) << " = "
        << stateRef(blockDep->name) << ";\n";
}

std::string CodeGenerator::assembleFunctionBody(const std::string &body, const std::string &delays)
{
    std::stringstream out;
    out << "void generated_step()\n{\n";
    out << body;
    out << "\n";
    out << delays;
    out << "}\n";
    return out.str();
}

std::string CodeGenerator::generateFunctionStep(
    const std::vector<int> &sortedOrder,
    const BlockManager &blockManager,
    const std::unordered_map<int, std::map<int, int>> &deps)
{
    const auto &blocksBySID = blockManager.getBlocksBySID();

    std::stringstream functionStep;
    std::stringstream delayAssignments;

    for (int sid : sortedOrder)
    {
        auto blockIt = blocksBySID.find(sid);
        if (blockIt == blocksBySID.end())
        {
            continue;
        }

        const Block &currentBlock = blockIt->second;
        auto depsIt = deps.find(sid);

        if (currentBlock.type == BlockType::Inport ||
            currentBlock.type == BlockType::Outport ||
            depsIt == deps.end() ||
            depsIt->second.empty())
        {
            continue;
        }

        const std::map<int, int> &currentDeps = depsIt->second;

        switch (currentBlock.type)
        {
        case BlockType::Sum:
            generateSumAssignment(currentBlock, currentDeps, blockManager, functionStep);
            break;
        case BlockType::Gain:
            generateGainAssignment(currentBlock, currentDeps, blockManager, functionStep);
            break;
        case BlockType::UnitDelay:
            generateUnitDelayAssignment(currentBlock, currentDeps, blockManager, delayAssignments);
            break;
        default:
            break;
        }
    }

    return assembleFunctionBody(functionStep.str(), delayAssignments.str());
}

std::string CodeGenerator::generateExtPortFooter()
{
    std::stringstream footer;

    footer << "const ExtPort * const\n";
    footer << TAB << "nwocg_generated_ext_ports = ext_ports;\n";
    footer << "const size_t\n";
    footer << TAB << "generated_ext_ports_size = sizeof(ext_ports);\n";

    return footer.str();
}

std::string CodeGenerator::generate(
    const std::vector<int> &sortedOrder,
    const BlockManager &blockManager,
    const std::unordered_map<int, std::map<int, int>> &deps,
    const std::string &outputPath)
{
    const std::unordered_map<int, Block> &blocksBySID = blockManager.getBlocksBySID();
    std::stringstream result;
    result << generateStruct(blocksBySID)
           << generateFunctionInit(blocksBySID)
           << generateFunctionStep(sortedOrder, blockManager, deps)
           << generateExtPort(blocksBySID, blockManager, deps)
           << generateExtPortFooter();

    const std::string code = result.str();

    std::ofstream outFile(outputPath);
    if (!outFile)
    {
        std::cerr << "Error: cannot open output file: " << outputPath << std::endl;
        return code;
    }
    outFile << code;
    outFile.close();

    std::cout << "Code generated: " << outputPath << std::endl;
    return code;
}