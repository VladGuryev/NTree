#include "cmdargparser.h"
#include "utility.h"
#include "printtree.h"

//Supported command line arguments
namespace
{
    const args::cmdArgName inputOption1 = "i";
    const args::cmdArgName inputOption2 = "input";
    const std::set<args::cmdArgName> inputOpts = {inputOption1, inputOption2};

    const args::cmdArgName outputOption1 = "o";
    const args::cmdArgName outputOption2 = "output";
    const std::set<args::cmdArgName> outputOpts = {outputOption1, outputOption2};

    const args::cmdArgName testOption1 = "t";
    const args::cmdArgName testOption2 = "test";
    const std::set<args::cmdArgName> testOpts = {testOption1, testOption2};

    const args::cmdArgName helpOption1 = "h";
    const args::cmdArgName helpOption2 = "help";
    const std::set<args::cmdArgName> helpOpts = {helpOption1, helpOption2};

    const std::string helpInfo = "Help information:\n"
                                 " 1. Pass -t or --test without parameters to launch test.\n"
                                 " 2. Pass -i or --input to set input file name after argument.\n"
                                 " 3. Pass -o or --output to set output filename after argument.\n"
                                 " Possible usage: ./a.out -i tree.data -o tree2.data\n"
                                 "                 ./a.out --input=tree.data";
}

void registerSerializers()
{
    std::string typeName = "std::string";
    ntree::registerSerializer<std::string>(ntree::stdStringSerializer, typeName);
    ntree::registerDeserializer(ntree::stdStringDeserializer, typeName);
    ntree::registerPrinter<std::string>([](const std::string& str){ std::cout << str; }, typeName);
}

int main(int argc, char* argv[])
{
    // For demonstration custom (de)serializers registration
    registerSerializers();

    args::parse(argc, argv);
    bool supportedArgsPassed = false;

    auto helpArgs = args::findEntries(helpOpts);
    if(!helpArgs.empty())
    {
        std::cout << helpInfo;
        return 0;
    }

    auto testArg = args::findEntries(testOpts);
    if(!testArg.empty())
    {
        testNTree();
        return 0;
    }

    std::pair<bool,std::string> inputArg = args::find(inputOpts);
    std::string inputFileName;
    if(inputArg.first)
    {
        supportedArgsPassed = true;
        inputFileName = inputArg.second;
    }
    std::pair<bool,std::string> outputArg = args::find(outputOpts);
    std::string outputFileName;
    if(outputArg.first)
    {
        supportedArgsPassed = true;
        outputFileName = outputArg.second;
    }

    if(!supportedArgsPassed)
    {
        std::cout << "No arguments provided. Program will be stopped." << std::endl;
        return 0;
    }

    ntree::TreeNode deserializedTree = deserializeFile(inputFileName);
    if(!deserializedTree.value.has_value())
    {
        return 0;
    }
    ntree::printTree(deserializedTree);
    serializeTree(deserializedTree, outputFileName);

    return 0;
}
