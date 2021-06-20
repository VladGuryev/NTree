#include "ntreeserializer.h"

#include <iomanip>
#include <iostream>
#include <map>

namespace ntree {


// FormatInfo
namespace
{
    const int headerSize      = 4; // 4 bytes for header with types
    const int typeNumberSize  = 4; // 4 bytes for typeNumber
    const int objectWidthSize = 4; // 4 bytes for objectWidthSize
    const int childCountSize  = 4; // 4 bytes for childCountSize
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

static void saveToBinary(const void* addr, std::size_t size, std::vector<char>& buffer)
{
    const char* ptr = reinterpret_cast<const char*>(addr);
    for(std::size_t i = 0; i < size; i++)
    {
        char ch = *ptr;
        buffer.push_back(ch);
        ptr++;
    }
}

static void loadFromBinary(void* addr, std::size_t size, const char* buffer)
{
    memcpy(reinterpret_cast<char*>(addr), buffer, size);
}

////нигде не используется!
//template<class T, class F>
//inline void register_any_serialize_visitor(NTreeSerializer& serializer,const F& f)
//{
//    std::cout << "Register visitor for type: " << std::quoted(typeid(T).name()) << std::endl;
//    serializer.serializeAnyVisitors.insert(to_any_serialize_visitor<T>(f));
//}

NTreeSerializer::NTreeSerializer()
{
    auto charSerializer = [&](char value,
                              std::vector<char>& buffer)
    {
        std::string typeName = "char";
        int typeNum = registerType(typeName);
        saveToBinary(&typeNum, typeNumberSize, buffer);        // номер типа

        int sizeOfObject = sizeof(char);
        saveToBinary(&sizeOfObject, objectWidthSize, buffer);  // размер объекта

        buffer.push_back(value);                               // объект
    };

    serializeAnyVisitors.insert( to_any_serialize_visitor<char>(charSerializer) );

    auto intSerializer = [&](int value,
                             std::vector<char>& buffer)
    {
        std::string typeName = "int";
        int typeNum = registerType(typeName);
        saveToBinary(&typeNum, typeNumberSize, buffer);           // номер типа
        saveToBinary(&objectWidthSize, objectWidthSize, buffer);  // размер объекта
        saveToBinary(&value, objectWidthSize, buffer);            // объект
    };

    serializeAnyVisitors.insert( to_any_serialize_visitor<int>(intSerializer) );

    auto charDeserializer = [&](const std::vector<char> &buffer,
                                const TypeInfo &typeInfo) ->std::any
    {
        char value;
        if(typeInfo.objectSize == 1)
            value = buffer[typeInfo.index];

       // std::cout << "char deserializer called!\n";

        return value;
    };
    deserializeAnyVisitors["char"] = charDeserializer;

    auto intDeserializer = [&](const std::vector<char> &buffer,
                               const TypeInfo &typeInfo) ->std::any
    {
        int value;
        if(typeInfo.objectSize == 4)
        {
            loadFromBinary(&value, typeInfo.objectSize, &buffer[typeInfo.index]);
        }

       // std::cout << "int deserializer called!\n";

        return value;
    };
    deserializeAnyVisitors["int"] = intDeserializer;

   // std::cout << "***********************\n";
//    for(auto& [type, i] : deserializeAnyVisitors)
//    {
       // std::cout << "deserialized type: " << type << std::endl;
 //   }
   // std::cout << "***********************\n";
}

void NTreeSerializer::serialize(const TreeNode &node,
                                std::vector<char> &buffer)
{
    std::vector<char> treeBuffer;
    serializeTree(node,treeBuffer);

    int headerByteCount = headerSize;
    std::vector<char> headerBuffer;

    //std::cout << "-------------------------------\n";

    std::map<int, std::string> typeInfoSorted;
    // Чтобы перечень типов в хедере файла
    // был отсортирован по возрастанию номера типа и не было необходимости хранить
    // в хедере еще и номер типа рядом с его именем
    for(const auto &[typeName, index] : typeInfo)
    {
        typeInfoSorted[index] = typeName;
    }

    for(const auto &[index, typeName] : typeInfoSorted)
    {
        int typeNameLength = typeName.length();
        saveToBinary(&typeNameLength, objectWidthSize, headerBuffer);

        saveToBinary(typeName.data(), typeNameLength, headerBuffer);

        headerByteCount += objectWidthSize + typeNameLength;

        //std::cout << typeName << "," << index << std::endl;
    }

    saveToBinary(&headerByteCount, headerSize, buffer);

    buffer.insert(std::end(buffer), std::begin(headerBuffer), std::end(headerBuffer));
    buffer.insert(std::end(buffer), std::begin(treeBuffer), std::end(treeBuffer));

   // std::cout << "-------------------------------\n";
}

TreeNode NTreeSerializer::deserialize(const std::vector<char>& buffer)
{
    if(buffer.empty())
    {
        return TreeNode{};
    }

    int index = loadHeader(buffer);
    return deserializeTree(buffer, index);
}

void NTreeSerializer::serializeTree(const TreeNode &node, std::vector<char> &buffer)
{
    // tree|subtree is empty
    if(!node.value.has_value())
    {
        return;
    }

    serializeAny(node.value, buffer);

    int childListSize = node.childList.size();
    saveToBinary(&childListSize, childCountSize, buffer);

    for(auto& child: node.childList)
    {
        serializeTree(child, buffer);
    }
}

TreeNode NTreeSerializer::deserializeTree(const std::vector<char>& buffer, int& index)
{
    TypeInfo typeInfo;
    loadFromBinary(&typeInfo.typeNum, typeNumberSize, &buffer[index]);
    index += typeNumberSize;

    int objectSize = 0;
    loadFromBinary(&objectSize, objectWidthSize, &buffer[index]);
    index += objectWidthSize;

    typeInfo.objectSize = objectSize;
    typeInfo.index = index;

   // std::cout << "typeNumber: " << typeInfo.typeNum << " " << "objectSize: " << typeInfo.objectSize << std::endl;

    std::any value = deserializeAny(buffer, typeInfo);

    index += objectSize;
    int childrenCount = 0;
    loadFromBinary(&childrenCount, childCountSize, &buffer[index]);

    index += childCountSize;

    TreeNode node = TreeNode{ value, {} };
    auto& childList = node.childList;

    for (int i = 0; i < childrenCount; i++)
    {
        childList.push_back(deserializeTree(buffer,index));
    }

    return node;
}

void NTreeSerializer::serializeAny(const std::any& a, std::vector<char>& buffer)
{
    if (const auto it = serializeAnyVisitors.find(std::type_index(a.type()));
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

std::any NTreeSerializer::deserializeAny(const std::vector<char> &buffer, const TypeInfo &typeInfo)
{
    std::string typeName(typeInfoReversed[typeInfo.typeNum]);

    if (const auto it = deserializeAnyVisitors.find(typeName);
        it !=  deserializeAnyVisitors.cend())
    {
        return it->second(buffer, typeInfo);
    }
    else
    {
        std::cout << "Unregistered type for deserialization "<< std::quoted(typeName);
        throw std::exception();
    }
}

int NTreeSerializer::registerType(const std::string &typeName)
{
    auto it = typeInfo.find(typeName);
    if(it == typeInfo.end())
    {
        typeInfo[typeName] = typeNumber;
        return typeNumber++;
    }
    return typeInfo[typeName];
}

int NTreeSerializer::loadHeader(const std::vector<char> &buffer)
{
    if(buffer.empty())
    {
        return 0;
    }

    typeNumber = 1;
    typeInfo.clear();

    int headerByteCount = 0;
    loadFromBinary(&headerByteCount, headerSize, buffer.data());
    //  std::cout << "\nheaderByteCount: " << headerByteCount << std::endl;

    int index = headerSize;
    while(index < headerByteCount)
    {
        int length = 0;
        loadFromBinary(&length, objectWidthSize, &buffer[index]);
        index += objectWidthSize;

        std::string typeName;
        typeName.resize(length);
        loadFromBinary(typeName.data(), length, &buffer[index]);
        index += length;

        int typeNumber = registerType(typeName);
        typeInfoReversed[typeNumber] = typeName;
    }

    // for(const auto &[typeName, index] : typeInfo)
    // {
    //   std::cout << "deserialized typeName: " << typeName << "," << index << std::endl;
    // }
    return headerByteCount;
}

} // ntree
