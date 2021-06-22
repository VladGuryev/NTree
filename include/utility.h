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

/*
 * Десериализация дерева из файла
 */
ntree::TreeNode deserializeFile(const std::string& fileName);

/*
 * Сериализация дерева в файл
 */
void serializeTree(const ntree::TreeNode& node, const std::string &fileName);

#endif // UTILITY_H
