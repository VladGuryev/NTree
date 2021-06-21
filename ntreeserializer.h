#ifndef NTREESERIALIZER_H
#define NTREESERIALIZER_H

#include <functional>
#include <iomanip>
#include <iostream>
#include <typeindex>

#include "ntree.h"
#include "typeserializers.h"

namespace ntree {

class NTreeSerializer
{

public:

    NTreeSerializer() = default;

    void serialize(const TreeNode &node, std::vector<char> &str);
    TreeNode deserialize(const std::vector<char> &buffer);

private:

    template<class T, class F>
    friend inline void register_any_serialize_visitor(NTreeSerializer& serializer,const F& f);

    using serializer   = std::function<void(const std::any&, std::vector<char>&)>;
    using deserializer = std::function<std::any(const std::vector<char>&, const TypeInfo&)>;

private:

    void serializeTree(const TreeNode &node, std::vector<char> &buffer);
    TreeNode deserializeTree(const std::vector<char>& buffer, int& index);

    void serializeAny(const std::any& a, std::vector<char>& buffer);
    std::any deserializeAny(const std::vector<char> &buffer, const TypeInfo &typeInfo);

    static int registerType(const std::string &typeName);
    int loadHeader(const std::vector<char>& buffer);

    template<class T, class F>
    friend inline void register_any_serialize_visitor(const F& f);

    template<class F>
    static inline std::pair<std::string, deserializer>
    addDeserializer(const F& f, const std::string& typeName)
    {
         return std::make_pair(typeName, f);
    }

    template<class T, class F>
    static inline std::pair<const std::type_index, serializer>
    addSerializer(const F& f, int typeNumber)
    {
        using namespace std::placeholders;
        auto serBinded = std::bind(f, _1, typeNumber, _2);

        auto ser =  [g = serBinded]
            (const std::any &a, std::vector<char>& buffer)
        {
            if constexpr (std::is_void_v<T>)
                g();
            else
                g(std::any_cast<const T&>(a), buffer);
        };

        return std::make_pair(std::type_index(typeid(T)), ser);
    }

private:

    static inline int typeNumber = 1;
    static std::unordered_map<std::string, int> typeInfo;   //typename to its index
    std::unordered_map<int, std::string> typeInfoReversed;  // typename index to its name

    static std::unordered_map<std::type_index, serializer> serializeAnyVisitors;
    static std::unordered_map<std::string, deserializer>   deserializeAnyVisitors;
};

template<class T, class F>
inline void register_any_serialize_visitor(const F& f)
{
    std::cout << "Register visitor for type: " << std::quoted(typeid(T).name()) << std::endl;
    NTreeSerializer::serializeAnyVisitors.insert(NTreeSerializer::addSerializer<T>(f));
}

} // ntree

#endif // NTREESERIALIZER_H
