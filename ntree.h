#ifndef NTREE_H
#define NTREE_H

#include <any>
#include <list>
#include <vector>

namespace ntree {


struct TreeNode
{
    std::any            value;
    std::list<TreeNode> childList;
};

class NTree
{
public:
    NTree() = default;
    TreeNode root;
};

void printTree(const TreeNode &node);
void serializeAny(const std::any& a, std::vector<char>& buffer);

} // ntree

#endif // NTREE_H
