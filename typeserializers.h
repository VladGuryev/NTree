#ifndef TYPESERIALIZERS_H
#define TYPESERIALIZERS_H

#include <any>
#include <string>
#include <vector>

namespace ntree
{

// FormatInfo
extern int headerSize;      // amount of bytes for header with types
extern int typeNumberSize;  // amount of bytes for typeNumber
extern int objectWidthSize; // amount of bytes for objectWidthSize
extern int childCountSize;  // amount of bytes for childCountSize

// Meta information about type that is located in the binary file
struct TypeInfo
{
    int index      = 0; // Current index of byte from that the type is located
    int objectSize = 0;
    int typeNum    = 0; // Number of type in the header for deserialization
};

void saveToBinary(const void *addr, std::size_t size, std::vector<char> &buffer);

void loadFromBinary(void* addr, std::size_t size, const char* buffer);

void charSerializer(char value, int typeNum, std::vector<char>& buffer);

std::any charDeserializer(const std::vector<char> &buffer, const TypeInfo &typeInfo);;

void intSerializer(int value, int typeNum, std::vector<char>& buffer);

std::any intDeserializer(const std::vector<char> &buffer, const TypeInfo &typeInfo);;

} //ntree

#endif // TYPESERIALIZERS_H
