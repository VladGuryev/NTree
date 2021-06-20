#ifndef NTREE_H
#define NTREE_H

#include <any>
#include <list>

namespace ntree {


struct TreeNode
{
    std::any            value;
    std::list<TreeNode> childList;
};

void printTree(const TreeNode &node);


} // ntree

#endif // NTREE_H
