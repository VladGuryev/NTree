#ifndef NTREE_H
#define NTREE_H

#include <any>
#include <list>

namespace ntree {

/*
 * N-ary tree node with unlimited amount of children
 */
struct TreeNode
{
    std::any            value;
    std::list<TreeNode> childList;
};

} // ntree

#endif // NTREE_H
