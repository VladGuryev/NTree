#include "ntree.h"

#include <functional>
#include <iomanip>
#include <iostream>
#include <typeindex>

namespace ntree {


template<class T, class F>
inline std::pair<const std::type_index, std::function<void(const std::any&)>>
to_any_visitor(const F& f)
{
    return std::make_pair(
        std::type_index(typeid(T)),
        [g = f](const std::any &a)
        {
            if constexpr (std::is_void_v<T>)
                g();
            else
                g(std::any_cast<const T&>(a));
        }
    );
}

static std::unordered_map<std::type_index, std::function<void(const std::any&)>>
printAnyVisitors
{
    to_any_visitor<void>([]{ std::cout << "{}"; }),
    to_any_visitor<int>([](int x){ std::cout << x; }),
    to_any_visitor<unsigned>([](unsigned x){ std::cout << x; }),
    to_any_visitor<float>([](float x){ std::cout << x; }),
    to_any_visitor<double>([](double x){ std::cout << x; }),
    to_any_visitor<char>([](char x){ std::cout << x; }),
    to_any_visitor<char const*>([](char const *s){ std::cout << std::quoted(s); }),
    to_any_visitor<std::string>([](const std::string& str){ std::cout << str; })
};

inline void printAny(const std::any& a)
{
    if (const auto it = printAnyVisitors.find(std::type_index(a.type()));
        it != printAnyVisitors.cend())
    {
        it->second(a);
    }
    else
    {
        std::cout << "Unregistered type "<< std::quoted(a.type().name());
    }
    std::cout << std::endl;
}

//нигде не используется!
//template<class T, class F>
//inline void register_any_visitor(const F& f)
//{
//    std::cout << "Register visitor for type: " << std::quoted(typeid(T).name()) << std::endl;
//    printAnyVisitors.insert(to_any_visitor<T>(f));
//}

static void ntreeTraverse(const TreeNode &node, int& depth);

void printTree(const TreeNode &node)
{
    int recursiveDepth = 0;
    ntreeTraverse(node, recursiveDepth);
}

void ntreeTraverse(const TreeNode &node, int& depth)
{
    using namespace std;

    for(int i = 0; i < depth; i++)
    {
        std::cout  << "    | ";
    }

    printAny(node.value);

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
