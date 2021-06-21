#ifndef PRINTTREE_H
#define PRINTTREE_H

#include <iostream>
#include <functional>
#include <unordered_map>
#include <typeindex>
#include <iomanip>

#include "ntree.h"

namespace ntree {

void printTree(const TreeNode &node);

class PrintInfo
{
public:
    using printFunc = std::function<void(const std::any&)>;

    static void printAny(const std::any& a);

private:

    template<class T, class F>
    static inline std::pair<const std::type_index, printFunc>
    addPrinter(const F& f)
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

    template<class T, class F>
    friend inline void registerPrinter(const F &f, const std::string& typeName);

private:
    static std::unordered_map<std::type_index, printFunc> printAnyVisitors;
};

template<class T, class F>
inline void registerPrinter(const F &f, const std::string& typeName)
{
    //std::cout << "Registered printer for type: " << typeName << std::endl;
    PrintInfo::printAnyVisitors.insert(PrintInfo::addPrinter<T>(f));
}

} // ntree

#endif // PRINTTREE_H
