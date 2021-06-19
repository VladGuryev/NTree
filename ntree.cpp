#include "ntree.h"

#include <type_traits>
#include <functional>
#include <iomanip>
#include <iostream>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <fstream>

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

template<class T, class F>
inline std::pair<const std::type_index, std::function<void(const std::any&, std::vector<char>&)>>
to_any_serialize_visitor(const F& f)
{
    return std::make_pair(
        std::type_index(typeid(T)),
        [g = f](const std::any &a, std::vector<char>& buffer)
        {
            if constexpr (std::is_void_v<T>)
                g();
            else
                g(std::any_cast<const T&>(a), buffer);
        }
        );
}

static std::unordered_map<std::type_index,
                          std::function<void(const std::any&, std::vector<char>&)>>
serializeAnyVisitors
{
    to_any_serialize_visitor<char>([](char value,
                                      std::vector<char>& buffer)
                                      {
//                                      std::cout << "test\n";
                                        //add typeinfo
                                        buffer.push_back(value);
                                      })
};

void serializeAny(const std::any& a, std::vector<char>& buffer)
{
    if (const auto it = serializeAnyVisitors.find(std::type_index(a.type()));
        it != serializeAnyVisitors.cend())
    {
        it->second(a, buffer);
    }
    else
    {
        std::cout << "Unregistered type for serialization "<< std::quoted(a.type().name());
        throw std::exception();
    }
}

//нигде не используется!
template<class T, class F>
inline void register_any_visitor(const F& f)
{
    std::cout << "Register visitor for type: " << std::quoted(typeid(T).name()) << std::endl;
    printAnyVisitors.insert(to_any_visitor<T>(f));
}

//нигде не используется!
template<class T, class F>
inline void register_any_serialize_visitor(const F& f)
{
    std::cout << "Register visitor for type: " << std::quoted(typeid(T).name()) << std::endl;
    serializeAnyVisitors.insert(to_any_serialize_visitor<T>(f));
}

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

//struct TypeInfo
//{
//    unsigned short typeNumber;  // up to 65535 possible types
//    std::string    typeName;    // typeName
//};


//void serialize(const Header& header, std::string_view fileName)
//{
//    std::ofstream output_file(fileName.data(), std::ios::binary);
//    std::size_t headerSize = sizeof(header.type) + sizeof (header.typeNameSize)
//                             + sizeof(header.dataSize) + header.dataSize;
//    output_file.write((char*)&header, headerSize);
//    output_file.close();
//}

} // ntree
