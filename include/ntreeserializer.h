#ifndef NTREESERIALIZER_H
#define NTREESERIALIZER_H

#include <functional>
#include <iomanip>
#include <iostream>
#include <typeindex>
#include <unordered_map>

#include "ntree.h"
#include "typeserializers.h"

namespace ntree {

class NTreeSerializer
{

public:

    using serializer   = std::function<void(const std::any&, std::vector<char>&)>;
    using deserializer = std::function<std::any(const std::vector<char>&, const TypeInfo&)>;

public:

    NTreeSerializer() = default;

    void serialize(const TreeNode &node, std::vector<char> &str);
    TreeNode deserialize(const std::vector<char> &buffer);

private:

    void serializeTree(const TreeNode &node, std::vector<char> &buffer);
    TreeNode deserializeTreeRecursive(const std::vector<char>& buffer, int& index);
    TreeNode deserializeTreeIterative(const std::vector<char>& buffer, int index);

    void serializeAny(const std::any& a, std::vector<char> &buffer);
    std::any deserializeAny(const std::vector<char> &buffer, const TypeInfo &typeInfo);

    int saveHeader(std::vector<char> &buffer);
    int loadHeader(const std::vector<char>& buffer);

    static int registerType(const std::string &typeName);

    template<class T, class F>
    static inline std::pair<const std::type_index, serializer>
    addSerializer(const F& f, int typeNumber)
    {
        using namespace std::placeholders;
        auto serBinded = std::bind(f, _1, typeNumber, _2);

        auto ser = [g = serBinded] (const std::any &a, std::vector<char>& buffer)
        {
            if constexpr (std::is_void_v<T>)
                g();
            else
                g(std::any_cast<const T&>(a), buffer);
        };

        return std::make_pair(std::type_index(typeid(T)), ser);
    }

    static inline std::pair<std::string, deserializer>
    addDeserializer(const deserializer& f, const std::string& typeName);

    template<class T, class F>
    friend inline void registerSerializer(const F& f, const std::string& typeName);

    friend void registerDeserializer(const NTreeSerializer::deserializer& deserializer,
                                     const std::string& typeName);
private:

    static inline int s_typeNumber = 1;
    static std::unordered_map<std::string, int> m_typeInfo;
    std::unordered_map<int, std::string> m_typeInfoReversed;

    static std::unordered_map<std::type_index, serializer> s_serializeAnyVisitors;
    static std::unordered_map<std::string, deserializer>   s_deserializeAnyVisitors;
};

template<class T, class F>
inline void registerSerializer(const F& serializer,
                               const std::string& typeName)
{
    //std::cout << "Registered serializer for type: " << typeName << std::endl;
    NTreeSerializer::s_serializeAnyVisitors.insert(
        NTreeSerializer::addSerializer<T>(serializer, NTreeSerializer::registerType(typeName)));
}

void registerDeserializer(const NTreeSerializer::deserializer& deserializer,
                          const std::string& typeName);

} // ntree

#endif // NTREESERIALIZER_H
