#include "ntreeserializer.h"

#include <map>

namespace ntree {


std::unordered_map<std::string, int> NTreeSerializer::typeInfo = {};

/*
 * Predefined serializers for POD types
 */
std::unordered_map<std::type_index, NTreeSerializer::serializer>
    NTreeSerializer::serializeAnyVisitors =
{
    addSerializer<char>(charSerializer, NTreeSerializer::registerType("char")),
    addSerializer<int>(intSerializer, NTreeSerializer::registerType("int")),
    addSerializer<float>(floatSerializer, NTreeSerializer::registerType("float")),
    addSerializer<double>(doubleSerializer, NTreeSerializer::registerType("double"))
};

/*
 * Predefined deserializers for POD types
 */
std::unordered_map<std::string, NTreeSerializer::deserializer>
    NTreeSerializer::deserializeAnyVisitors =
{
    addDeserializer(charDeserializer, "char"),
    addDeserializer(intDeserializer, "int"),
    addDeserializer(floatDeserializer, "float"),
    addDeserializer(doubleDeserializer, "double")
};

void NTreeSerializer::serialize(const TreeNode &node, std::vector<char> &buffer)
{
    std::vector<char> treeBuffer;
    serializeTree(node,treeBuffer);

    std::vector<char> headerBuffer;
    int headerByteCount = saveHeader(headerBuffer);
    saveToBinary(&headerByteCount, headerSize, buffer);

    buffer.insert(std::end(buffer), std::begin(headerBuffer), std::end(headerBuffer));
    buffer.insert(std::end(buffer), std::begin(treeBuffer), std::end(treeBuffer));
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

int NTreeSerializer::saveHeader(std::vector<char> &buffer)
{
    int headerByteCount = headerSize;
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
        saveToBinary(&typeNameLength, objectWidthSize, buffer);
        saveToBinary(typeName.data(), typeNameLength, buffer);
        headerByteCount += objectWidthSize + typeNameLength;
    }

    return headerByteCount;
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

std::pair<std::string, NTreeSerializer::deserializer>
NTreeSerializer::addDeserializer(const NTreeSerializer::deserializer &f,
                                 const std::string &typeName)
{
    return std::make_pair(typeName, f);
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

    return headerByteCount;
}

void registerDeserializer(const NTreeSerializer::deserializer &deserializer,
                          const std::string &typeName)
{
    //std::cout << "Registered deserializer for type: " << typeName << std::endl;
    NTreeSerializer::deserializeAnyVisitors[typeName] = deserializer;
}

} // ntree
