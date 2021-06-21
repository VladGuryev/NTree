#include "printtree.h"

#include <functional>

#include <iostream>
#include <typeindex>

namespace ntree {

/*
 * Predefined printers for POD types
 */
std::unordered_map<std::type_index, PrintInfo::printFunc>
    PrintInfo::printAnyVisitors
    {
        addPrinter<void>([]{ std::cout << "{}"; }),
        addPrinter<int>([](int x){ std::cout << x; }),
        addPrinter<unsigned>([](unsigned x){ std::cout << x; }),
        addPrinter<float>([](float x){ std::cout << std::setprecision(7) << x; }),
        addPrinter<double>([](double x){ std::cout << std::setprecision(13) << x; }),
        addPrinter<char>([](char x){ std::cout << x; }),
        addPrinter<char const*>([](char const *s){ std::cout << std::quoted(s); }),
    };

void PrintInfo::printAny(const std::any& a)
{
    if (const auto it =  printAnyVisitors.find(std::type_index(a.type()));
        it !=  printAnyVisitors.cend())
    {
        it->second(a);
    }
    else
    {
        std::cout << "Unregistered type for print: "<< std::quoted(a.type().name());
    }
    std::cout << std::endl;
}

static void ntreeTraverse(const TreeNode &node, int& depth);

void printTree(const TreeNode &node)
{
    if(!node.value.has_value())
    {
        return;
    }
    std::cout << "\nNTree:\n";
    int recursiveDepth = 0;
    ntreeTraverse(node, recursiveDepth);
    std::cout << "\n";
}

void ntreeTraverse(const TreeNode &node, int& depth)
{
    using namespace std;

    for(int i = 0; i < depth; i++)
    {
        std::cout  << "    | ";
    }

    PrintInfo::printAny(node.value);

    depth++;

    if(node.childList.empty())
    {
        depth--;
        return;
    }
    else
    {
        for(const auto& node : node.childList)
        {
            ntreeTraverse(node, depth);
        }
        depth--;
    }
}

} // ntree
