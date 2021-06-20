#ifndef NTREESERIALIZER_H
#define NTREESERIALIZER_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <typeindex>
#include <functional>
#include <unordered_map>

#include "ntree.h"

namespace ntree {


class NTreeSerializer
{
public:
    using serializer = std::function<void(const std::any&, std::vector<char>&)>;

    NTreeSerializer();

    void serialize(const TreeNode &node, std::vector<char> &str);

    TreeNode deserialize(const std::vector<char> &buffer);

private:

    void serializeTree(const TreeNode &node, std::vector<char> &buffer);

    TreeNode deserializeTree(const std::vector<char>& buffer, int& index);

    template<typename T>
    auto poll(const std::vector<T>& vec, int& index, int objectSize)
    {
        int curIndex = index;
        index += objectSize;
        return vec[curIndex];
    }

    void serializeAny(const std::any& a, std::vector<char>& buffer);
    int addType(const std::string &typeName);

    int loadHeader(const std::vector<char>& buffer);

private:

    int typeNumber = 1;
    std::unordered_map<std::string, int> typeInfo;

    std::unordered_map<std::type_index, serializer> serializeAnyVisitors;

    //std::unordered_map<std::string, deserializer> deserializeAnyVisitors;

};



} // ntree

#endif // NTREESERIALIZER_H
