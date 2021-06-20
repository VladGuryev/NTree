#include "utility.h"

std::streamsize fileSize(std::fstream &fs)
{
    if ( !fs.is_open() ) {
        //acout() << name() << "size():"<< "Error while opening file!\n";
        return false;
    }
    fs.ignore( std::numeric_limits<std::streamsize>::max() );
    std::streamsize length = fs.gcount();
    fs.clear();   //  Since ignore will have set eof.
    fs.seekg( 0, std::ios_base::beg );
    return length;
}

ntree::TreeNode createTestTree()
{
    using namespace ntree;
    TreeNode root = TreeNode{'B', {}};
    auto& list = root.childList;
    list.push_back({'W', {}});
    list.push_back({'S', {}});
    list.push_back({'R', {}});
    list.push_back({'T', {}});

    auto& front = list.front();
    auto& children2 = front.childList;
    children2.push_back({'H', {}});
    children2.push_back({'G', {}});
    children2.push_back({'X', {}});

    auto& last = list.back();
    auto& children = last.childList;
    children.push_back({12, {}});
    children.push_back({13, {}});
    children.push_back({14, {}});

    auto& last2 = children.back();
    auto& children3 = last2.childList;
    children3.push_back({'E',{}});
    children3.push_back({'U',{}});
    children3.push_back({'A',{}});

    return root;
}

void testNTree()
{
    using namespace std;
    using namespace ntree;

    cout << "************************** Test **********************\n";

    auto root = createTestTree();
    cout << "Programmatically generated NTree:\n\n";
    printTree(root);
    cout << endl;

    NTreeSerializer s;
    std::vector<char> buffer;

    s.serialize(root, buffer);

    std::string fileName = "output.data";
    std::fstream output(fileName, std::ios::out | std::ios::binary);
    output.write(buffer.data(), buffer.size());
    output.close();
    cout << "NTree serialized to file " << fileName <<".\n";

    std::fstream input(fileName, std::ios::in | std::ios::binary);

    buffer.clear();
    auto fz = fileSize(input);
    buffer.resize(fz);

    input.read(buffer.data(), fz);

    auto root2 = s.deserialize(buffer);

    cout << endl;
    cout << "Deserialized NTree from file " << fileName << ":\n\n";
    printTree(root2);
}
