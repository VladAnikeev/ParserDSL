#include "pugixml.hpp"
#include "block.h"
#include "block_manager.h"
#include "line.h"
#include "line_manager.h"
#include "code_generator.h"
#include "dependency_graph.h"
#include <iostream>
#include <filesystem>

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <path-to-xml-file> [output-file]" << std::endl;
        return 1;
    }
    const std::string inputFile = argv[1];

    // Парсинг XML
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(inputFile.c_str());
    if (!result)
    {
        std::cout << "Error: " << result.description() << std::endl;
        return 1;
    }

    BlockManager blockManager;
    pugi::xml_node root = doc.root().first_child();
    for (pugi::xml_node blockNode : root.children("Block"))
    {
        Block block = BlockManager::parseBlock(blockNode);
        blockManager.addBlock(block);
    }

    LineManager lineManager;
    for (pugi::xml_node lineNode : root.children("Line"))
    {
        Line line = LineManager::parseLine(lineNode);
        lineManager.addLine(line);
    }

    // Поиск зависимостей и топологическая сортировка
    DependencyGraph dependencies;
    dependencies.buildDependencies(lineManager.getLines());
    if (!dependencies.topologicalSort(blockManager.getBlocksBySID()))
    {
        std::cout << "Error: Cyclic dependency detected" << std::endl;
        return 1;
    }

    // Имя выходного файла: из аргумента, иначе из входного XML с расширением .c
    std::string outputFile;
    if (argc >= 3)
    {
        outputFile = argv[2];
    }
    else
    {
        std::filesystem::path outputPath(inputFile);
        outputPath.replace_extension(".c");
        outputFile = outputPath.string();
    }

    // Генерация кода
    CodeGenerator codeGenerator;
    codeGenerator.generate(dependencies.order, blockManager, dependencies.deps, outputFile);
    return 0;
}

