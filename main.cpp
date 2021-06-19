#include <iostream>
//#include "ntree.h"
#include "ntreeserializer.h"
using namespace std;
using namespace ntree;

struct S
{
    int t = -184;
} s;

TreeNode createDummyNTree()
{
    TreeNode root = TreeNode{'Q', {}};
    auto& list = root.childList;
    list.push_back({'W', {}});
    list.push_back({'S', {}});
    list.push_back({'R', {}});
    list.push_back({'T', {}});

    auto& front = list.front();
    auto& children2 = front.childList;
    children2.push_back({'H', {}});
    children2.push_back({'G', {}});
    children2.push_back({'B', {}});

    auto& last = list.back();
    auto& children = last.childList;
    children.push_back({'P', {}});
    children.push_back({'L', {}});
    children.push_back({'K', {}});

    return root;
}

int main()
{
    auto root = createDummyNTree();
    printTree(root);
    cout << endl;

    /*
    NTreeSerializer s;
    s.serialize(root);
    s.close();

    TreeNode root2;

    FILE* fp = fopen("NTreeSerialized.txt", "r");
    s.deserialize(root2,fp);
    */
    NTreeSerializer s;
    std::vector<std::string> str;


    s.serialize(root, str);
    cout << "serialized: ";
    for(auto s : str)
        cout << s;
    cout << endl;

    auto root2 = s.deserialize(str);

    cout << endl;
    cout << "deserialized tree: \n";
    printTree(root2);

//    NTree t;
//    t.root = TreeNode{"root", {}};
//    auto& list = t.root.childList;
//    list.push_back({1, {}});
//    list.push_back({2, {}});
//    list.push_back({s, {}});
//    list.push_back({"3", {}});

//    auto& last = list.back();
//    auto& children = last.childList;
//    children.push_back({string("31"), {}});
//    children.push_back({32.0, {}});
//    children.push_back({33, {}});

//    auto& front = list.front();
//    auto& children2 = front.childList;
//    children2.push_back({string("12"), {}});
//    children2.push_back({13.0f, {}});
//    children2.push_back({14, {}});

//    printTree(t);
//    cout << endl;

//    bar();
    return 0;
}
