#include "ntreeserializer.h"

#include <vector>
#include <sstream>
#include <cstddef>
#include <iomanip>
#include <typeindex>
#include <functional>

namespace ntree {

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
inline void register_any_serialize_visitor(const F& f)
{
    std::cout << "Register visitor for type: " << std::quoted(typeid(T).name()) << std::endl;
    serializeAnyVisitors.insert(to_any_serialize_visitor<T>(f));
}


void NTreeSerializer::serialize(const TreeNode &node,
                                std::vector<char> &buffer)
{
    // tree|subtree is empty
    if(!node.value.has_value())
    {
        return;
    }

    serializeAny(node.value, buffer);

    //serialize childList count!
    auto childListSize = node.childList.size();
    buffer.push_back(childListSize);

    for(auto& child: node.childList)
    {
        serialize(child, buffer);
    }
}

TreeNode NTreeSerializer::deserialize(const std::vector<char>& buffer)
{
    if(buffer.empty())
    {
        return TreeNode{};
    }
    int index = 0;
    return buildTree(buffer, index);
}

TreeNode NTreeSerializer::buildTree(const std::vector<char>& buffer, int& index)
{
    char val = poll(buffer,index);

    TreeNode node = TreeNode{ val, {} };

    int childrenCount = poll(buffer,index);
    auto& childList = node.childList;

    for (int i = 0; i < childrenCount; i++)
    {
        childList.push_back(buildTree(buffer,index));
    }

    return node;
}



} // ntree
