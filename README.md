# Heterogeneous  N-ary tree with (de)serialization functionality
---

### Description

Heterogeneous N-ary tree allows to store in nodes values of any type. You can add node with any value
not caring about its type in runtime. But there are some limitations in usage of additional functionality.
If tree is needed to be printed in console the printing functions for every presented type should be registered in the
framework. Ths is also true for serialization and deserialization. The tree provides convenient API to register any type
handlers you want.

### Limitations:
- Max size of node value: 2<sup>31</sup> – 1 bytes
- Max number of children of one node: 2<sup>31</sup> – 1
- There is no exact number for quantity of types that can be registered, but assuming the type name
can be stored in 16 bytes, so the total amount of types is not more than 10<sup>8</sup> that should be enough for many
applications.

### Prerequisites
- C++17 
- Cmake 3.5 or higher

### Compiling and running code
To build project:
```
$ cmake --configure .
$ cmake --build .
```
The program will be placed in the `build` directory of the root project directory.
The project contains `data` directory with `input.data` file for test purposes. 
To use this file for demonstration launch the program like this:
```
[build]$ ./NTree  -i ../data/input.data
```
or
```
[build]$ ./NTree  -t
```
If you have some `treeIn.data` file assuming it has tree inside 
it can be serialized to `treeOut.data` like this:
```
[build]$ ./NTree  -i treeIn.data -o treeOut.data
```

### Command line arguments
The program supports _4_ types of arguments:
* `-h (--help)` help info
* `-t (--test)` program will be launched with programly generated tree that will be
serialized to test_tree.data and deserialized back from that file and printed in the
console
* `-i (--input)` to set input file name for deserialization
* `-o (--output)` to set output file name for serialization

### Details
The serialization format is binary. It allows to serialize tree with any type
that supported by serialization and deserialization functions. POD types
serializators are provided out of the box and `std::string` as example of custom type. 
To add your own type for support you should write functions with signatures:
```
serializer - void(const std::any&, std::vector<char>&)
deserializer - std::any(const std::vector<char>&, const TypeInfo&)
```
and register them in the serialization framework like this:
```
std::string typeName = "std::string";
ntree::registerSerializer<std::string>(ntree::stdStringSerializer, typeName);
ntree::registerDeserializer(ntree::stdStringDeserializer, typeName);
```
While serializing the dynamic type of value in `std::any` is resolved and the appropriate serialization function is chosen
if registered. Same for deserialization. If there is no (de)serialization function for some type, the exception is thrown.

For printing type in console while pre-order tree traversal provide function with signature:
```
void(const std::any&)
```
where `std::any` can be of any type, obviously. Register print function in the framework like this (`std::string` example):
```
 ntree::registerPrinter<std::string>([](const std::string& str){ std::cout << str; }, typeName);
```
If there is no printing function registered in the framework for the type of any node, the error will be printed on the
place of that node in the tree.


### Serialization format

Basic structure of the file consists of two sections: 
* header section
* data section

1. Header section contains meta information about the types used in the tree nodes.
The types in the header section are listed as in array and implied to have their own index in that array.
Although, the indexes of types are not presented in the file because they are restored easily by the type 
list sequence.
2. Data section contains tree node objects that are serialized in the pre-order traversal. Every node 
data frame is finished by its children count size which might equal to zero.

#####  Overview of the header section
* header size section (4 bytes int) - the amount of bytes in the header section from the first to the last byte.
* type string size (4 bytes int) - 4 bytes to store the number of bytes of string type name
* string type name (arbitrary amount of bytes)

* ...

* another type string size
* another string type name

##### Header section structure is described in the table below:

| Header size  | Type name size_1  |Type name string_1| Type name size_2| Type name string_2|...|
| ------------- | ------------- |------------- |------------- |------------- |------------- |
| 4 bytes  | 4 bytes  | N1 bytes  | 4 bytes | N2 bytes|
| M bytes  | N1  |  | N2|


#####  Overview of the data section
* node type number (index of type in the list from the header) - 4 bytes
* node object size (4 bytes) - amount of bytes following this frame holding object 
* object bytes (arbitrary amount of bytes)
* node children count (4 bytes)
				
##### Data section structure is described in the table below:	

| Type number of node_1  | Object size of node_1  | Object node_1| Children count of node_1 | Type number of node_i|...|
| ------------- | ------------- |------------- |------------- |------------- |------------- |
| 4 bytes  | 4 bytes  | N1 bytes  | 4 bytes | 4 bytes|
| index of type_1 in the type list  | N1  |  | K children| index of type_i in the type list|