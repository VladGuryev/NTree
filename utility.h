#ifndef UTILITY_H
#define UTILITY_H

#include <iostream>
#include <fstream>
#include <vector>

#include "ntree.h"
#include "ntreeserializer.h"

/*
 * Получение размера файла
 */
std::streamsize fileSize(std::fstream& fs);

/*
 * Создание тестового NTree
 */
ntree::TreeNode createTestTree();

/*
 * Создание тестового NTree
 */
void testNTree();


//void charSerializer(char value, std::vector<char>& buffer)
//{
//    std::string typeName = "char";
//    int typeNum = registerType(typeName);
//    saveToBinary(&typeNum, typeNumberSize, buffer);        // номер типа

//    int sizeOfObject = sizeof(char);
//    saveToBinary(&sizeOfObject, objectWidthSize, buffer);  // размер объекта

//    buffer.push_back(value);                               // объект
//};


/*
 * Десериализация дерева из файла
 */
ntree::TreeNode deserializeFile(const std::string& fileName);

/*
 * Сериализация дерева в файл
 */
void serializeTree(const ntree::TreeNode& node, const std::string &fileName);

#endif // UTILITY_H
