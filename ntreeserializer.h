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
    char                key = -1;
    std::list<TreeNode> childList;
};

class NTreeSerializer
{
public:
    NTreeSerializer() = default;

    void serialize(const TreeNode &root, std::vector<std::string> &str);

    TreeNode deserialize(const std::vector<std::string> &str);

private:
    TreeNode buildTree(std::queue<std::string>& queue);

};

void printTree(const TreeNode &root);

} // ntree

#endif // NTREESERIALIZER_H
