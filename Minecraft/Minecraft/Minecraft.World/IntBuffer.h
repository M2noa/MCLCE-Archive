#pragma once

#include "Buffer.h"

class IntBuffer : public Buffer
{
private:
	int *buffer;

public:
	IntBuffer(unsigned int capacity);	
	IntBuffer( unsigned int capacity, int *backingArray );
	virtual ~IntBuffer();

	virtual IntBuffer *flip();
	int get(unsigned int index);
	int *getBuffer();
	IntBuffer *put(intArray *inputArray, unsigned int offset, unsigned int length);
	IntBuffer *put(intArray inputArray);
	IntBuffer *put(int i);
};