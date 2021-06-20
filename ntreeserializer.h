#ifndef NTREESERIALIZER_H
#define NTREESERIALIZER_H

#include <typeindex>
#include <functional>

#include "ntree.h"

namespace ntree {

class NTreeSerializer
{

public:

    NTreeSerializer();

    void serialize(const TreeNode &node, std::vector<char> &str);
    TreeNode deserialize(const std::vector<char> &buffer);

private:

    struct TypeInfo
    {
        int index      = 0;
        int objectSize = 0;
        int typeNum    = 0;
    };

    using serializer   = std::function<void(const std::any&, std::vector<char>&)>;
    using deserializer = std::function<std::any(const std::vector<char>&, const TypeInfo&)>;

private:

    void serializeTree(const TreeNode &node, std::vector<char> &buffer);
    TreeNode deserializeTree(const std::vector<char>& buffer, int& index);

    void serializeAny(const std::any& a, std::vector<char>& buffer);
    std::any deserializeAny(const std::vector<char> &buffer, const TypeInfo &typeInfo);

    int registerType(const std::string &typeName);
    int loadHeader(const std::vector<char>& buffer);

private:

    int typeNumber = 1;
    std::unordered_map<std::string, int> typeInfo;          //typename to its index
    std::unordered_map<int, std::string> typeInfoReversed;  // typename index to its name

    std::unordered_map<std::type_index, serializer> serializeAnyVisitors;
    std::unordered_map<std::string, deserializer>   deserializeAnyVisitors;
};



} // ntree

#endif // NTREESERIALIZER_H
