#include <iostream>
//#include "ntree.h"
#include "ntreeserializer.h"


using namespace std;
using namespace ntree;


struct S
{
    int t = -184;
} s;

/*****************************************************************************
 * Размер файла
 *
 * @return
 *  Размер файла
 *  - количества байт в файле
 */
std::streamsize fileSize(std::fstream& fs)
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


TreeNode createTestTree()
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

    auto& last2 = children.back();
    auto& children3 = last2.childList;
    children3.push_back({'E',{}});
    children3.push_back({'U',{}});
    children3.push_back({'A',{}});

    return root;
}

int main()
{
    auto root = createTestTree();
    printTree(root);
    cout << endl;

    NTreeSerializer s;
    std::vector<char> buffer;

    s.serialize(root, buffer);

    std::fstream output("NTree_serialized.data", std::ios::out | std::ios::binary);
    output.write(buffer.data(), buffer.size());
    output.close();
    cout << "tree serialized\n";


    // Open the stream to 'lock' the file.
    std::fstream input("NTree_serialized.data", std::ios::in | std::ios::binary);
    // Read the whole file into the buffer.
    buffer.clear();
    auto fz = fileSize(input);
    buffer.resize(fz);

    input.read(buffer.data(), fz);

    auto root2 = s.deserialize(buffer);

    cout << endl;
    cout << "deserialized tree: \n";
    printTree(root2);


    return 0;
}
