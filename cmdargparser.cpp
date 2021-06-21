#include "cmdargparser.h"

#include <string>
#include <string_view>

namespace args
{

/*
 * Тип сохраняемого токена
 */
enum class E_TokenType
{
    Arg,    // Аргумент командной строки
    Param   // Значение аргумента
};

// Вспомогательная функция для обработки токена командной строки
static void parseToken(std::string_view token);
// Вспомогательная функция для сохранения токена командной строки
static void saveToken(E_TokenType tokenType, std::string_view token);

// Аргументы и их параметры из командной строки
cmdArgs gCmdArgs;

void parse(const int argc, const char * const argv[])
{
    // Добавление пустой записи в структуру gCmdArgs для нулевого параметра командной строки
    gCmdArgs.push_back(cmdArgPair(cmdArgName{}, cmdArgValues{}));
    for(int i = 0; i < argc; i++)
    {
        parseToken(argv[i]);
    }
}

/*
 * Вспомогательная функция для обработки токена командной строки
 */
static void parseToken(std::string_view token)
{
    size_t tokenSize = token.size();
    // Проверка токена на наличие префикса аргумента из двух дефисов
    if(tokenSize > 1 && token.substr(0, 2) == "--")
    {
        saveToken(E_TokenType::Arg, token.substr(2));
        return;
    }
    // Проверка токена на наличие префикса аргумента из одного дефиса
    if(tokenSize > 0 && token.substr(0, 1) == "-")
    {
        saveToken(E_TokenType::Arg, token.substr(1));
        return;
    }
    // Если токен не является аргументом, то сохранить его как параметр
    saveToken(E_TokenType::Param, token);
    return;
}

/*
 * Разбиение строки на подстроки по заданному разграничителю
 */
std::vector<std::string> split(std::string_view str, std::string_view delim)
{
    std::vector<std::string> out;
    size_t startInd = 0;
    size_t endInd = 0;

    if(str.empty())
    {
        return out;
    }

    while ((endInd = str.find(delim, startInd)) < str.size()) // поиск окончания очередной части
    {
        out.push_back(std::string(str.substr(startInd, endInd - startInd)));
        startInd = endInd + delim.size();
    }

    if(startInd < str.size())
    {
        // Добавление остатка строки
        out.push_back(std::string(str.substr(startInd)));
    }
    else if(startInd == str.size())
    {
        // Разделитель в конце - добавление пустой строки
        out.push_back({});
    }
    return out;
}

/*
 * Сохранение токена командной строки
 */
static void saveToken(E_TokenType tokenType, std::string_view token)
{
    if(tokenType == E_TokenType::Arg)
    {
        // Разбиение токена по символу "="
        std::vector<std::string> tokens = split(token, "=");
        cmdArgName  arg= tokens.front();
        cmdArgValues params = {};
        // Если второй элемент вектора, содержащий все символы токена после символа '=', не пуст
        if(tokens.back().size() > 0)
        {
            params = { ++tokens.begin(), tokens.end() };
        }
        gCmdArgs.push_back({ arg, params });
    }
    else if(tokenType == E_TokenType::Param)
    {
        /* Получение ссылки на параметры последнего добавленного аргумента
         * и запись параметра в список параметров аргумента
         */
        std::pair<cmdArgName, cmdArgValues> &lastArgParams = gCmdArgs.back();
        lastArgParams.second.push_back(std::string(token));
    }
}

std::pair<bool,std::string> find(const std::set<std::string> &requiredArgs)
{
    bool isArgExist = false;
    std::string argValue = {};
    // Условие наличия искомого аргумента в структуре gCmdArgs и подходящего параметра этого аргумента
    auto predicate = [&requiredArgs, &isArgExist, &argValue] (const cmdArgs::value_type &arg) -> bool
    {
        if(requiredArgs.count(arg.first) > 0 && arg.second.size() > 0)
        {
            isArgExist = true;
            argValue = arg.second.back();
            return true;
        }
        return false;
    };
    std::find_if(gCmdArgs.rbegin(), gCmdArgs.rend(), predicate);
    return { isArgExist, argValue };
}

cmdArgs findEntries(const std::set<cmdArgName> &args)
{
    cmdArgs entries;
    auto predicate = [&args, &entries] (const cmdArgs::value_type &curArg)
    {
        if(args.count(curArg.first) > 0)
        {
            entries.push_back(curArg);
        }
    };
    std::for_each(gCmdArgs.begin(), gCmdArgs.end(), predicate);
    return entries;
}

} // namespace args
