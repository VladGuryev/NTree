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


#endif // UTILITY_H
