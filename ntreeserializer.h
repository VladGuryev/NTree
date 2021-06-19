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
    TreeNode buildTree(std::queue<char> &queue);

    template<typename T>
    auto poll(std::queue<T>& queue)
    {
        auto value = queue.front();
        queue.pop();
        return value;
    }

};

void printTree(const TreeNode &root);

} // ntree

#endif // NTREESERIALIZER_H
