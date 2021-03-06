#include <iostream>
#include <string>
#include <map>

#include "expression-tree-compiler.hpp"

using Tool = int(*)(std::string,std::string);
struct Descriptor
{
    Tool tool;
    std::string description;
};

int tmxToMap(std::string, std::string);
int lvxToLvl(std::string, std::string);
int tsxToTs(std::string, std::string);
int pexToPe(std::string, std::string);
int exportLanguage(std::string, std::string);

const std::map<std::string,Descriptor> toolList =
{
    { "tmxToMap", { tmxToMap, "converts a Tiled map into a form that can be consumed by the engine" } },
    { "lvxToLvl", { lvxToLvl, "converts a XML document describing a level into a form accessible by the engine" } },
    { "tsxToTs", { tsxToTs, "converts a XML document describing a tileset into a form accessible by the engine" } },
    { "pexToPe", { pexToPe, "converts a XML document describing a particle emitter into a form accessible by the engine" } },
    { "exportLanguage", { exportLanguage, "converts a language descriptor file into a binary form" } },
};

void printAllTools()
{
    std::cout << "List of current tools on the toolkit:" << std::endl;
    
    for (const auto& p : toolList)
        std::cout << p.first << ": " << p.second.description << std::endl;
}

int main(int argc, char **argv)
{
    if (argc < 4)
    {
        std::cout << "Usage: " << argv[0] << " <tool> <input> <output>" << std::endl;
        return -1;
    }

    if (std::string(argv[1]) == "help")
    {
        printAllTools();
        return 0;
    }
    
    if (std::string(argv[1]) == "lextest")
    {
        while (true)
        {
            std::cout << "Write your string: ";
            
            std::string str;
            std::getline(std::cin, str);
            
            try
            {
                printTree(*compileExpression(str));
                std::cout << std::endl;
            }
            catch (std::runtime_error err)
            {
                std::cout << "Error: " << err.what() << std::endl;
            }
        }
    }

    auto it = toolList.find(argv[1]);
    if (it == toolList.end())
    {
        std::cout << "Tool " << argv[1] << " not found on the toolkit! ";
        printAllTools();
        return -1;
    }

    return it->second.tool(argv[2], argv[3]);
}
