#include "stdafx.h"
#include "FloatBuffer.h"

//Allocates a new float buffer.
//The new buffer's position will be zero, its limit will be its capacity, and its mark will be undefined.
//It will have a backing array, and its array offset will be zero.
//
//Parameters:
//capacity - The new buffer's capacity, in floats
FloatBuffer::FloatBuffer(unsigned int capacity) : Buffer( capacity )
{
	buffer = new float[capacity];
	memset( buffer,0,sizeof(float)*capacity);
}

FloatBuffer::FloatBuffer( unsigned int capacity, float *backingArray ) : Buffer( capacity )
{
	hasBackingArray = true;
	buffer = backingArray;
}

FloatBuffer::~FloatBuffer()
{
	if( !hasBackingArray )
		delete[] buffer;
}

//Flips this buffer. The limit is set to the current position and then the position is set to zero.
//If the mark is defined then it is discarded.
//
//Returns:
//This buffer
FloatBuffer *FloatBuffer::flip()
{
	m_limit = m_position;
	m_position = 0;
	return this;
}

//Relative put method  (optional operation).
//Writes the given float into this buffer at the current position, and then increments the position.
//
//Parameters:
//f - The float to be written
//Returns:
//This buffer
FloatBuffer *FloatBuffer::put(float f)
{
	buffer[m_position++] = f;
	return this;
}

//Relative bulk get method.
//This method transfers floats from this buffer into the given destination array.
//An invocation of this method of the form src.get(a) behaves in exactly the same way as the invocation
//
//     src.get(a, 0, a.length) 
//Returns:
//This buffer
void FloatBuffer::get(floatArray *dst)
{
	assert( dst->length <= m_capacity );

	for (unsigned int i = 0; i < dst->length; i++)
		dst->data[i] = buffer[i]; 
}