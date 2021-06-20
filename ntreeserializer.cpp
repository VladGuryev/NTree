#include "ntreeserializer.h"

#include <vector>
#include <sstream>
#include <cstddef>
#include <iomanip>


namespace ntree {


// FormatInfo
namespace
{
    const int typeNumberSize  = 4; // 4 bytes for typeNumber       in binary file
    const int objectWidthSize = 4; // 4 bytes for objectWidthSize  in binary file
    const int childCountSize  = 4; // 4 bytes for childCountSize   in binary file
};


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

NTreeSerializer::NTreeSerializer()
{
    auto charSerializer = [&](char value,
                              std::vector<char>& buffer)
    {
        std::string typeName = "char";
        int typeNum = addType(typeName);
        SaveToBinary(&typeNum, typeNumberSize, buffer);        // номер типа (4 байта)

        int sizeOfObject = sizeof(char);
        SaveToBinary(&sizeOfObject, objectWidthSize, buffer);  // размер объекта (4байта)

        buffer.push_back(value);                               // объект (неограничено кол-во байт)
    };

    serializeAnyVisitors =
    {
        to_any_serialize_visitor<char>(charSerializer)
    };
}

void NTreeSerializer::serializeAny(const std::any& a, std::vector<char>& buffer)
{
    if (const auto it =  serializeAnyVisitors.find(std::type_index(a.type()));
        it !=  serializeAnyVisitors.cend())
    {
        it->second(a, buffer);
    }
    else
    {
        std::cout << "Unregistered type for serialization "<< std::quoted(a.type().name());
        throw std::exception();
    }
}

////нигде не используется!
//template<class T, class F>
//inline void register_any_serialize_visitor(NTreeSerializer& serializer,const F& f)
//{
//    std::cout << "Register visitor for type: " << std::quoted(typeid(T).name()) << std::endl;
//    serializer.serializeAnyVisitors.insert(to_any_serialize_visitor<T>(f));
//}

int NTreeSerializer::addType(const std::string &typeName)
{
    auto it = typeInfo.find(typeName);
    if(it == typeInfo.end())
    {
        typeInfo[typeName] = typeNumber;
        return typeNumber++;
    }
    return typeInfo[typeName];
}

void NTreeSerializer::serialize(const TreeNode &node,
                                std::vector<char> &buffer)
{
    serializeTree(node,buffer);

    std::cout << "-------------------------------\n";
    for(auto &pair : typeInfo)
    {
        std::cout << pair.first << "," << pair.second << std::endl;
    }
    std::cout << "-------------------------------\n";
}

void NTreeSerializer::serializeTree(const TreeNode &node, std::vector<char> &buffer)
{
    // tree|subtree is empty
    if(!node.value.has_value())
    {
        return;
    }

    serializeAny(node.value, buffer);

    //serialize childList count!
    int childListSize = node.childList.size();
    SaveToBinary(&childListSize, childCountSize, buffer);

    for(auto& child: node.childList)
    {
        serializeTree(child, buffer);
    }
}

TreeNode NTreeSerializer::deserialize(const std::vector<char>& buffer)
{
    if(buffer.empty())
    {
        return TreeNode{};
    }
    int index = 0;
    return deserializeTree(buffer, index);
}

TreeNode NTreeSerializer::deserializeTree(const std::vector<char>& buffer, int& index)
{
    char val          = poll(buffer,index);
    int childrenCount = poll(buffer,index);

    TreeNode node = TreeNode{ val, {} };
    auto& childList = node.childList;


    for (int i = 0; i < childrenCount; i++)
    {
        childList.push_back(deserializeTree(buffer,index));
    }

    return node;
}





} // ntree
