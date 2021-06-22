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
    saveToBinary(&typeNum, typeNumberSize, buffer);        // номер типа
    int sizeOfObject = sizeof(char);
    saveToBinary(&sizeOfObject, objectWidthSize, buffer);  // размер объекта
    buffer.push_back(value);                               // объект
}

std::any charDeserializer(const std::vector<char> &buffer, const TypeInfo &typeInfo)
{
    char value;
    value = buffer[typeInfo.index];
    return value;
}

void intSerializer(int value, int typeNum, std::vector<char> &buffer)
{
    saveToBinary(&typeNum, typeNumberSize, buffer);         // номер типа
    saveToBinary(&objectWidthSize,objectWidthSize, buffer); // размер объекта
    saveToBinary(&value, objectWidthSize, buffer);          // объект
}

std::any intDeserializer(const std::vector<char> &buffer, const TypeInfo &typeInfo)
{
    int value;
    loadFromBinary(&value, typeInfo.objectSize, &buffer[typeInfo.index]);
    return value;
}

void floatSerializer(float value, int typeNum, std::vector<char> &buffer)
{
    saveToBinary(&typeNum, typeNumberSize, buffer);        // номер типа
    int sizeOfFloat = sizeof(float);
    saveToBinary(&sizeOfFloat, objectWidthSize, buffer);  // размер объекта
    saveToBinary(&value, sizeOfFloat, buffer);            // объект
}

std::any floatDeserializer(const std::vector<char> &buffer, const TypeInfo &typeInfo)
{
    float value;
    loadFromBinary(&value, typeInfo.objectSize, &buffer[typeInfo.index]);
    return value;
}

void doubleSerializer(double value, int typeNum, std::vector<char> &buffer)
{
    saveToBinary(&typeNum, typeNumberSize, buffer);        // номер типа
    int sizeOfDouble = sizeof(double);
    saveToBinary(&sizeOfDouble, objectWidthSize, buffer);  // размер объекта
    saveToBinary(&value, sizeOfDouble, buffer);            // объект
}

std::any doubleDeserializer(const std::vector<char> &buffer, const TypeInfo &typeInfo)
{
    double value;
    loadFromBinary(&value, typeInfo.objectSize, &buffer[typeInfo.index]);
    return value;
}

void stdStringSerializer(const std::string &value, int typeNum, std::vector<char> &buffer)
{
    saveToBinary(&typeNum, typeNumberSize, buffer);
    int typeNameLength = value.length();
    saveToBinary(&typeNameLength, objectWidthSize, buffer);
    saveToBinary(value.data(), typeNameLength, buffer);
}

std::any stdStringDeserializer(const std::vector<char> &buffer, const TypeInfo &typeInfo)
{
    std::string value;
    value.resize(typeInfo.objectSize);
    loadFromBinary(value.data(), typeInfo.objectSize, &buffer[typeInfo.index]);
    return value;
}

} // ntree
