#pragma once
using namespace std;

#include "stdafx.h"
#include "Node.h"
#include "System.h"
#include "BasicTypeContainers.h"

class BinaryHeap
{
private:
	NodeArray heap;
    int sizeVar;

	// 4J Jev, add common ctor code.
	void _init();

public:
	BinaryHeap();
	~BinaryHeap();

	Node *insert(Node *node);
	void clear();
    Node *peek();
    Node *pop();
    void remove(Node *node);
    void changeCost(Node *node, float newCost);
    int size();

private:
	void upHeap(int idx);
    void downHeap(int idx);

public:
    bool isEmpty();
};