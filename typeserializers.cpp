#include "typeserializers.h"

namespace ntree
{

int headerSize      = 4;
int typeNumberSize  = 4;
int objectWidthSize = 4;
int childCountSize  = 4;

void saveToBinary(const void *addr, std::size_t size, std::vector<char> &buffer)
{
    const char* ptr = reinterpret_cast<const char*>(addr);
    for(std::size_t i = 0; i < size; i++)
    {
        char ch = *ptr;
        buffer.push_back(ch);
        ptr++;
    }
}

void loadFromBinary(void *addr, std::size_t size, const char *buffer)
{
    memcpy(reinterpret_cast<char*>(addr), buffer, size);
}

void charSerializer(char value, int typeNum, std::vector<char> &buffer)
{
    std::string typeName = "char";
    saveToBinary(&typeNum, typeNumberSize, buffer);        // номер типа
    int sizeOfObject = sizeof(char);
    saveToBinary(&sizeOfObject, objectWidthSize, buffer);  // размер объекта
    buffer.push_back(value);                               // объект
}

std::any charDeserializer(const std::vector<char> &buffer, const TypeInfo &typeInfo)
{
    char value;
    if(typeInfo.objectSize == 1)
    {
        value = buffer[typeInfo.index];
    }
    return value;
}

void intSerializer(int value, int typeNum, std::vector<char> &buffer)
{
    std::string typeName = "int";
    saveToBinary(&typeNum, typeNumberSize, buffer);        // номер типа
    saveToBinary(&objectWidthSize,
                 objectWidthSize, buffer);                 // размер объекта
    saveToBinary(&value, objectWidthSize, buffer);         // объект
}

std::any intDeserializer(const std::vector<char> &buffer, const TypeInfo &typeInfo)
{
    int value;
    if(typeInfo.objectSize == 4)
    {
        loadFromBinary(&value, typeInfo.objectSize, &buffer[typeInfo.index]);
    }
    return value;
}


} // ntree
