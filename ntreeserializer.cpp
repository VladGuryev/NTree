#include "ntreeserializer.h"

#include <vector>
#include <sstream>

namespace ntree {


void NTreeSerializer::serialize(const TreeNode &root,
                                 std::vector<std::string> &str)
{
    if(root.key == -1)
    {
        str.push_back("*");
    }
    else
    {
        char key = root.key;
        std::string key_str;
        key_str.push_back(key);
        str.push_back(key_str);

        auto size = std::to_string(root.childList.size());

        str.push_back(size);

        for (auto& child: root.childList) {
            serialize(child, str);
        }
    }
}

TreeNode NTreeSerializer::deserialize(const std::vector<std::string>& vec)
{
    std::queue<std::string> queue;

    for(const auto& tmp: vec)
    {
        queue.push(tmp);
    }
    return buildTree(queue);
}

TreeNode NTreeSerializer::buildTree(std::queue<std::string>& queue)
{
    std::string val = queue.front();
    queue.pop();
    if (val == std::string("*"))
    {
        return TreeNode{};
    }

    TreeNode root = TreeNode{val[0],{}};

    std::istringstream ss(queue.front());
    queue.pop();

    int childrenCount = 0;
    ss >> childrenCount;

    auto& childList = root.childList;
    for (int i = 0; i <  childrenCount; i++) {
        childList.push_back(buildTree(queue));
    }
    return root;
}

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

    cout << (char)node.key;
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
