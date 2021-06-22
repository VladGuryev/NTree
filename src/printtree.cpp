#include "printtree.h"

#include <functional>

#include <iostream>
#include <typeindex>
#include <stack>

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
        addPrinter<const char*>([](const char* s){ std::cout << std::quoted(s); }),
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
        throw std::exception();
    }
    std::cout << std::endl;
}

static void ntreeTraverseRecursive(const TreeNode &node, int& depth);
static void ntreeTraverseIterative(const TreeNode &node);

void printTree(const TreeNode &node)
{
    if(!node.value.has_value())
    {
        return;
    }
    std::cout << "\nNTree:\n";

    //int recursiveDepth = 0;
    //ntreeTraverseRecursive(node, recursiveDepth);

    ntreeTraverseIterative(node);

    std::cout << "\n";
}

[[maybe_unused]]
void ntreeTraverseRecursive(const TreeNode &node, int& depth)
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
            ntreeTraverseRecursive(node, depth);
        }
        depth--;
    }
}

void ntreeTraverseIterative(const TreeNode &node)
{
    using namespace std;

    std::stack<std::reference_wrapper<const TreeNode>> stack;
    stack.push(std::cref(node));

    int depth = 0;
    std::stack<int> childCounts;

    const int firstNodeAsChildSize = 1;    // root node is a single child for hypothetical parent
    childCounts.push(firstNodeAsChildSize);

    while(!stack.empty())
    {
        --childCounts.top();
        if(childCounts.top() < 0)
        {
            childCounts.pop();
            depth--;
        }

        for(int i = 0; i < depth; i++)
        {
            std::cout  << "    | ";
        }

        auto n = stack.top();
        PrintInfo::printAny(n.get().value);
        stack.pop();

        for(auto it = n.get().childList.rbegin(); it != n.get().childList.rend(); it++)
        {
            stack.push(*it);
        }
        int childListCount = n.get().childList.size(); // O(1) since C++11
        if(childListCount)
        {
            depth++;
            childCounts.push(childListCount);
        }
    }
}

} // ntree
