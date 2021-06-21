#include "utility.h"

std::streamsize fileSize(std::fstream &fs)
{
    if ( !fs.is_open() ) {
        return false;
    }
    fs.ignore( std::numeric_limits<std::streamsize>::max() );
    std::streamsize length = fs.gcount();
    fs.clear();
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

    cout << "\n************************** Test **********************\n";

    auto root = createTestTree();
    cout << "Programmatically generated NTree:\n";
    printTree(root);
    cout << endl;

    serializeTree(root, "test_tree.data");
    auto deserializedTree = deserializeFile("test_tree.data");
    ntree::printTree(deserializedTree);

    cout << "************************** Test END ********************\n";
}

ntree::TreeNode deserializeFile(const std::string &fileName)
{
    if(fileName.empty())
    {
        return ntree::TreeNode{};
    }

    std::fstream input(fileName, std::ios::in | std::ios::binary);
    auto fz = fileSize(input);

    if(fz < 1)
    {
        std::cout << "Empty file passed for deserialization. Program will be stopped\n";
        return ntree::TreeNode{};
    }

    ntree::NTreeSerializer s;
    std::vector<char> buffer;

    buffer.resize(fz);
    input.read(buffer.data(), fz);

    auto root = s.deserialize(buffer);

    std::cout << "N-ary tree was deserialized from file " << fileName << std::endl;
    return root;
}

void serializeTree(const ntree::TreeNode &node, const std::string& fileName)
{
    if(fileName.empty())
    {
        std::cout << "Empty file name provided for serialization. Program will be stopped.\n";
        return;
    }
    if(!node.value.has_value())
    {
        std::cout << "Empty tree provided for serialization. Program will be stopped.\n";
        return;
    }
    ntree::NTreeSerializer s;
    std::vector<char> buffer;

    s.serialize(node, buffer);

    std::fstream output(fileName, std::ios::out | std::ios::binary);
    output.write(buffer.data(), buffer.size());

    std::cout << "N-ary tree was serialized to file " << fileName << std::endl;
}
