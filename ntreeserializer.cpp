#include "ntreeserializer.h"

#include <vector>
#include <sstream>
#include <cstddef>
namespace ntree {


void NTreeSerializer::serialize(const TreeNode &node,
                                std::vector<char> &buffer)
{
    if(node.value == -1)
    {
        buffer.push_back('*');
    }
    else
    {
        char value = node.value; //std::any
        buffer.push_back(value);

        auto childListSize = node.childList.size();
        buffer.push_back(childListSize);

        for (auto& child: node.childList) {
            serialize(child, buffer);
        }
    }
}

TreeNode NTreeSerializer::deserialize(const std::vector<char>& buffer)
{
    std::queue<char> queue;

    for(const auto& tmp: buffer)
    {
        queue.push(tmp);
    }
    return buildTree(queue);
}

TreeNode NTreeSerializer::buildTree(std::queue<char>& queue)
{
    char val = poll(queue);

    if (val == '*')
    {
        return TreeNode{};
    }

    TreeNode node = TreeNode{ val,{} };

    int childrenCount = poll(queue);
    auto& childList = node.childList;

    for (int i = 0; i < childrenCount; i++)
    {
        childList.push_back(buildTree(queue));
    }

    return node;
}

/*
 * удалить позже
 */
void ntreeTraverse(const TreeNode &node, int& depth);

void printTree(const TreeNode &root)
{
    int recursiveDepth = 0;
    ntreeTraverse(root, recursiveDepth);
}

void ntreeTraverse(const TreeNode &node, int& depth)
{
    using namespace std;

    for(int i = 0; i < depth; i++)
    {
        std::cout  << "    | ";
    }

    cout << (char)node.value;
    std::cout << std::endl;

    depth++;

    if(node.childList.empty())
    {
        depth--;
        return;
    }
    else
    {
        for(const auto& node : node.childList)
        {
            ntreeTraverse(node, depth);
        }
        depth--;
    }
}

} // ntree
