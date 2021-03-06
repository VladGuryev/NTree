#include "ntreeserializer.h"

#include <map>
#include <stdexcept>
#include <sstream>
#include <stack>

namespace ntree {

class SerializerException : public std::runtime_error
{
public:
    SerializerException(const std::string& error, const std::string& type = "")
        : std::runtime_error(error), m_error(error), m_type(type) {}

    std::string type() const
    {
        if(m_type.empty())
        {
            return "";
        }
        m_ss << std::quoted(m_type);
        return m_ss.str();
    }

    std::string error() const
    {
        return m_error;
    }

private:
    std::string m_error = "";
    std::string m_type = "";
    mutable std::stringstream m_ss;
};

std::unordered_map<std::string, int> NTreeSerializer::m_typeInfo = {};

/*
 * Predefined serializers for POD types
 */
std::unordered_map<std::type_index, NTreeSerializer::serializer>
    NTreeSerializer::s_serializeAnyVisitors =
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
    NTreeSerializer::s_deserializeAnyVisitors =
{
    addDeserializer(charDeserializer, "char"),
    addDeserializer(intDeserializer, "int"),
    addDeserializer(floatDeserializer, "float"),
    addDeserializer(doubleDeserializer, "double")
};

void NTreeSerializer::serialize(const TreeNode &node, std::vector<char> &buffer)
{
    std::vector<char> treeBuffer;

    try
    {
        serializeTree(node,treeBuffer);
    }
    catch (const SerializerException& se)
    {
        std::cout << se.error() << " " << se.type() << std::endl;
        return;
    }
    catch (...)
    {
       std::cout << "Unregistered exception thrown while serializing tree" << std::endl;
       return;
    }

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

    int headerShift = loadHeader(buffer);

    TreeNode root;

    try
    {
        //root = deserializeTreeRecursive(buffer,headerShift);
        root = deserializeTreeIterative(buffer, headerShift);
    }
    catch (const SerializerException& se)
    {
        std::cout << se.error() << " " << se.type() << std::endl;
        return root;
    }
    catch (...)
    {
        std::cout << "Unregistered exception thrown while deserializing tree" << std::endl;
        return root;
    }

    return root;
}

void NTreeSerializer::serializeTree(const TreeNode &node, std::vector<char> &buffer)
{
    std::stack<std::reference_wrapper<const TreeNode>> stack;
    stack.push(std::cref(node));

    while(!stack.empty())
    {
        auto curNode = stack.top();
        serializeAny(curNode.get().value, buffer);

        int childListSize = curNode.get().childList.size();
        saveToBinary(&childListSize, childCountSize, buffer);

        stack.pop();

        for(auto it = curNode.get().childList.rbegin();
            it != curNode.get().childList.rend(); it++)
        {
            stack.push(*it);
        }
    }
}

[[maybe_unused]]
TreeNode NTreeSerializer::deserializeTreeRecursive(const std::vector<char>& buffer, int& index)
{
    if(index < headerSize + objectWidthSize)
    {
        throw SerializerException("Invalid header format while deserializing");
    }

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
        childList.push_back(deserializeTreeRecursive(buffer,index));
    }

    return node;
}

TreeNode NTreeSerializer::deserializeTreeIterative(const std::vector<char>& buffer, int index)
{
    if(index < headerSize + objectWidthSize)
    {
        throw SerializerException("Invalid header format while deserialization");
    }

    int dIndex = index;

    struct Frame
    {
        TypeInfo typeInfo;
        int objectSize = 0;
        int childrenCount = 0;
    } frame;

    std::stack<Frame> stack;
    stack.push(frame);

    std::stack<int> childCounts;
    const int firstNodeAsChildSize = 1;
    childCounts.push(firstNodeAsChildSize);

    std::list<TreeNode> subTreeStorage;
    std::stack<TreeNode*> parents;
    parents.push(nullptr);

    while(!stack.empty())
    {
        --childCounts.top();
        if(childCounts.top() < 0)
        {
            childCounts.pop();

            TreeNode* subTree = parents.top();
            parents.pop();

            auto& childsOfSubTree = parents.top()->childList.back().childList;
            childsOfSubTree.splice(childsOfSubTree.end(), subTree->childList);
        }

        auto frame = stack.top();

        loadFromBinary(&frame.typeInfo.typeNum, typeNumberSize, &buffer[dIndex]);
        dIndex += typeNumberSize;

        loadFromBinary(&frame.objectSize, objectWidthSize, &buffer[dIndex]);
        dIndex += objectWidthSize;

        frame.typeInfo.objectSize = frame.objectSize;
        frame.typeInfo.index = dIndex;

        std::any value = deserializeAny(buffer, frame.typeInfo);

        dIndex += frame.objectSize;
        loadFromBinary(&frame.childrenCount, childCountSize, &buffer[dIndex]);

        dIndex += childCountSize;

        TreeNode node = TreeNode{ value, {} };

        if(parents.top())
        {
            parents.top()->childList.push_back(node);
        }

        stack.pop();

        for(int i = 0; i < frame.childrenCount; i++)
        {
            Frame childrenFrames;
            stack.push(childrenFrames);
        }

        if(frame.childrenCount)
        {
            subTreeStorage.push_back(node);
            parents.push(&subTreeStorage.back());
            childCounts.push(frame.childrenCount);
        }
    }

    TreeNode* subTree = nullptr;
    while(parents.top())
    {
        subTree = parents.top();
        parents.pop();
        if(!parents.top())
        {
            break;
        }

        auto& childsOfSubTree = parents.top()->childList.back().childList;
        childsOfSubTree.splice(childsOfSubTree.end(), subTree->childList);
    }

    return subTree ? *subTree : TreeNode{};
}


void NTreeSerializer::serializeAny(const std::any& a, std::vector<char>& buffer)
{
    if (const auto it = s_serializeAnyVisitors.find(std::type_index(a.type()));
        it !=  s_serializeAnyVisitors.cend())
    {
        it->second(a, buffer);
    }
    else
    {
        throw SerializerException("Unregistered type for serialization", a.type().name());
    }
}

std::any NTreeSerializer::deserializeAny(const std::vector<char> &buffer, const TypeInfo &typeInfo)
{
    std::string typeName(m_typeInfoReversed[typeInfo.typeNum]);

    if (const auto it = s_deserializeAnyVisitors.find(typeName);
        it !=  s_deserializeAnyVisitors.cend())
    {
        return it->second(buffer, typeInfo);
    }
    else
    {
        throw SerializerException("Unregistered type for deserialization", typeName);
    }
}

int NTreeSerializer::saveHeader(std::vector<char> &buffer)
{
    int headerByteCount = headerSize;
    std::map<int, std::string> typeInfoSorted;

    // ?????????? ???????????????? ?????????? ?? ???????????? ??????????
    // ?????? ???????????????????????? ???? ?????????????????????? ???????????? ???????? ?? ???? ???????? ?????????????????????????? ??????????????
    // ?? ???????????? ?????? ?? ?????????? ???????? ?????????? ?? ?????? ????????????
    for(const auto &[typeName, index] : m_typeInfo)
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
    auto it = m_typeInfo.find(typeName);
    if(it == m_typeInfo.end())
    {
        m_typeInfo[typeName] = s_typeNumber;
        return s_typeNumber++;
    }
    return m_typeInfo[typeName];
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

    s_typeNumber = 1;
    m_typeInfo.clear();

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
        m_typeInfoReversed[typeNumber] = typeName;
    }

    return headerByteCount;
}

void registerDeserializer(const NTreeSerializer::deserializer &deserializer,
                          const std::string &typeName)
{
    //std::cout << "Registered deserializer for type: " << typeName << std::endl;
    NTreeSerializer::s_deserializeAnyVisitors[typeName] = deserializer;
}

} // ntree
