#pragma once
#include "Buffer.h"

class FloatBuffer : public Buffer
{
private:
	float *buffer;

public:
	FloatBuffer(unsigned int capacity);
	FloatBuffer( unsigned int capacity, float *backingArray );
	virtual ~FloatBuffer();

	FloatBuffer *flip();
	FloatBuffer *put(float f);
	void get(floatArray *dst);
	float *_getDataPointer() {return buffer;}
};