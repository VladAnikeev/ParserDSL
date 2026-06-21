#include "line_manager.h"
#include <iostream>

void LineManager::addLine(const Line& line)
{
    lines.push_back(line);
    int idx = static_cast<int>(lines.size()) - 1;
    sourceToLines[line.source.blockSID].push_back(idx);
}


Line LineManager::parseLine(pugi::xml_node lineNode)
{
    Line line;
    
    for (pugi::xml_node paramNode : lineNode.children("P"))
    {
        std::string paramName = paramNode.attribute("Name").as_string();
        std::string paramValue = paramNode.child_value();
        
        if (paramName == "Src")
        {
            if (!paramValue.empty())
            {
                line.source = Signal::parse(paramValue);
            }
        }
    }
    
    for (pugi::xml_node branchNode : lineNode.children("Branch"))
    {
        Signal branch;
        
        for (pugi::xml_node paramNode : branchNode.children("P"))
        {
            std::string paramName = paramNode.attribute("Name").as_string();
            std::string paramValue = paramNode.child_value();
            
            if (paramName == "Dst")
            {
                if (!paramValue.empty())
                {
                    branch = Signal::parse(paramValue);
                }
            }
        }
        
        if (branch.blockSID != 0)
        {
            line.branches.push_back(branch);
        }
    }
    
    // Если нет ветвлений, но есть Dst на уровне Line - создаем одну ветку
    if (line.branches.empty())
    {
        for (pugi::xml_node paramNode : lineNode.children("P"))
        {
            std::string paramName = paramNode.attribute("Name").as_string();
            std::string paramValue = paramNode.child_value();
            
            if (paramName == "Dst" && !paramValue.empty())
            {
                Signal branch;
                branch = Signal::parse(paramValue);
                line.branches.push_back(branch);
                break;
            }
        }
    }
    
    return line;
}

const std::vector<Line>& LineManager::getLines() const
{
    return lines;
}

void LineManager::printAll() const 
{
    std::cout << "=== LINES (" << lines.size() << ") ===" << std::endl;
    for (size_t i = 0; i < lines.size(); ++i)
    {
        const auto& line = lines[i];
        std::cout << "Line " << i << ": ";
        std::cout << line.source.blockSID << "#out:" << line.source.port;
        
        for (const auto& branch : line.branches)
        {
            std::cout << " -> " << branch.blockSID 
                      << "#in:" << branch.port;
        }
        std::cout << std::endl;
    }
}