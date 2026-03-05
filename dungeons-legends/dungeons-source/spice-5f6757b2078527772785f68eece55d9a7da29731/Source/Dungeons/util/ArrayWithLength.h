#pragma once

#include <vector>

class Node;
class Item;
class Block;

typedef std::vector<unsigned char> byteArray;
typedef std::vector<short> shortArray;
typedef std::vector<int> intArray;
typedef std::vector<float> floatArray;
typedef std::vector<Node*> NodeArray;
typedef std::vector<Item*> ItemArray;
typedef std::vector<const Block*> BlockArray;
