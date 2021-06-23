#include "utility.h"

#include "printtree.h"

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

ntree::TreeNode testTree()
{
    using namespace ntree;

    TreeNode root = TreeNode{'A', {}};

    auto& list = root.childList;
    list.push_back({'B', {}});
    list.push_back({'C', {}});

    auto& last = list.back();
    auto& children = last.childList;
    children.push_back({'D', {}});
    children.push_back({'E', {}});

    list.push_back({'F', {}});
    list.push_back({'G', {}});

    return root;
}

struct TestUnregisteredStruct
{
    std::vector<int> m = {1,2};
} testUnregisteredStruct;

ntree::TreeNode createTestTree()
{
    using namespace ntree;
    TreeNode root = TreeNode{'B', {}};
    auto& list = root.childList;
    list.push_back({'W', {}});
    list.push_back({double(45.872), {}});
    list.push_back({'R', {}});
    list.push_back({double(852.989), {}});

    auto& front = list.front();
    auto& children2 = front.childList;
    //children2.push_back({testUnregisteredStruct, {}});
    children2.push_back({'H', {}});
    children2.push_back({std::string("str_1A"), {}});
    children2.push_back({std::string("str_2B"), {}});

    auto& last = list.back();
    auto& children = last.childList;
    children.push_back({int(12), {}});
    children.push_back({float(34.09), {}});
    children.push_back({int(14), {}});

    auto& last2 = children.back();
    auto& children3 = last2.childList;
    children3.push_back({'E',{}});
    children3.push_back({float(1.99),{}});
    children3.push_back({'A',{}});

    children.push_back(testTree());

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
    if(!root.value.has_value())
    {
        return ntree::TreeNode{};
    }

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
    if(buffer.empty())
    {
        std::cout << "Error while serializing tree\n";
        return;
    }

    std::fstream output(fileName, std::ios::out | std::ios::binary);
    output.write(buffer.data(), buffer.size());

    std::cout << "N-ary tree was serialized to file " << fileName << std::endl;
}
