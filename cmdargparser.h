#ifndef CMDARGPARSER_H
#define CMDARGPARSER_H

#include <algorithm>
#include <set>
#include <string>
#include <utility>
#include <vector>

namespace args
{

// Имя аргумента командной строки
using cmdArgName = std::string;
// Значения аргумента командной строки
using cmdArgValues = std::vector<std::string>;
// Пара: {имя аргумента; значения аргумента}
using cmdArgPair = std::pair<cmdArgName, cmdArgValues>;
// Набор пар имен аргументов и их значений, извлеченных из командной строки
using cmdArgs = std::vector<cmdArgPair>;

// Формирование структуры cmdArgs с аргументами и параметрами из командной строки
void parse(const int a_argc, const char * const a_argv[]);
// Извлечение последнего непустого параметра для аргументов одной опции
std::pair<bool,std::string> find(const std::set<cmdArgName> &requiredArgs);
// Получение подмножества gCmdArgs, содержащего переданный пользователем набор аргументов
cmdArgs findEntries(const std::set<cmdArgName> &args);

// Аргументы и их параметры из командной строки
extern cmdArgs gCmdArgs;

} // namespace args


#endif // CMDARGPARSER_H
