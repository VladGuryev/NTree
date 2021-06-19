#ifndef NTREESERIALIZER_H
#define NTREESERIALIZER_H

#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <string>
//#include "ntree.h"

#include <list>

namespace ntree {


struct TreeNode
{
    char                value;
    std::list<TreeNode> childList;
};

class NTreeSerializer
{
public:
    NTreeSerializer() = default;

    void serialize(const TreeNode &node, std::vector<char> &str);

    TreeNode deserialize(const std::vector<char> &buffer);

private:
    TreeNode buildTree(const std::vector<char>& buffer, int& index);

    template<typename T>
    auto poll(const std::vector<T>& vec, int& index)
    {
        return vec[index++];
    }

};

void printTree(const TreeNode &root);

} // ntree

#endif // NTREESERIALIZER_H
