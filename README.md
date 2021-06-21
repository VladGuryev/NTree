# NTree
Heterogeneous  N-ary tree with (de)serialization functionality

Limitations:
Number of children in one node: 2^31 – 1


**Serialization format of N-ary tree**

Basic structure of the file consists of two sections: 
1. header
2. data

Header section contains the list of the types that are used in the NTree.
The list implies that the index of the type in that list is the real index of type as marker number
in the data section.

*Header section structure is described on the picture below:

[Header size] [Type name size2] [Type name "string1"] [Type name size2] [Type name "string2"]
 - - - - -    - - - - -          - - - - -             - - - - -         - - - - -
| 4 bytes |  | 4 bytes |        | N1 bytes |          | 4 bytes |       | N2 bytes |        . . . . . etc
 - - - - -    - - - - -          - - - - -             - - - - -         - - - - -
          [N1 number] [Type number is implied to be 1]  [N2 number] [Type number is implied to be 2]
						
						
*Data section structure is described on the picture below:

[Type number1] [Node1 value size] [Node1 value] [Node1 childlist count] [Type number2]
 - - - - -      - - - - -          - - - - -    - - - - -                - - - - -
| 4 bytes |    | 4 bytes |        | N1 bytes |  | 4 bytes |             | 4 bytes |        . . . . . etc
 - - - - -      - - - - -          - - - - -    - - - - -                - - - - -
               [N1 number]