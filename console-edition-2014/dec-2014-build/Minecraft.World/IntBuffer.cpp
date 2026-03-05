#include "stdafx.h"

#include "IntBuffer.h"

//Allocates a new int buffer.
//The new buffer's position will be zero, its limit will be its capacity, and its mark will be undefined.
//It will have a backing array, and its array offset will be zero.
//
//Parameters:
//capacity - The new buffer's capacity, in ints
IntBuffer::IntBuffer(unsigned int capacity) : Buffer( capacity )
{
	buffer = new int[capacity];
	memset( buffer,0,sizeof(int)*capacity);
}

IntBuffer::IntBuffer( unsigned int capacity, int *backingArray ) : Buffer( capacity )
{
	hasBackingArray = true;
	buffer = backingArray;
}

IntBuffer::~IntBuffer()
{
	if( !hasBackingArray )
		delete[] buffer;
}

int *IntBuffer::getBuffer()
{
	return buffer;
}

//Flips this buffer. The limit is set to the current position and then the position is set to zero.
//If the mark is defined then it is discarded.
//
//Returns:
//This buffer
IntBuffer *IntBuffer::flip()
{
	m_limit = m_position;
	m_position = 0;
	return this;
}

//Absolute get method. Reads the int at the given index.
//Parameters:
//index - The index from which the int will be read
//Returns:
//The int at the given index
int IntBuffer::get(unsigned int index)
{
	assert( index < m_limit );

	return buffer[index];
}

//Relative bulk put method  (optional operation).
//This method transfers ints into this buffer from the given source array.
//If there are more ints to be copied from the array than remain in this buffer, that is, if length > remaining(),
//then no ints are transferred and a BufferOverflowException is thrown.
//
//Otherwise, this method copies length ints from the given array into this buffer, starting at the given offset in the array
//and at the current position of this buffer. The position of this buffer is then incremented by length.
//
//In other words, an invocation of this method of the form dst.put(src, off, len) has exactly the same effect as the loop
//
//     for (int i = off; i < off + len; i++)
//         dst.put(a[i]); 
//except that it first checks that there is sufficient space in this buffer and it is potentially much more efficient.
//Parameters:
//src - The array from which ints are to be read
//offset - The offset within the array of the first int to be read; must be non-negative and no larger than array.length
//length - The number of ints to be read from the given array; must be non-negative and no larger than array.length - offset
//Returns:
//This buffer
IntBuffer *IntBuffer::put(intArray *inputArray, unsigned int offset, unsigned int length)
{
	assert( offset + length < inputArray->length );

	std::copy( inputArray->data +offset, inputArray->data +offset+length, buffer+m_position );

	m_position += length;

	return this;
}

IntBuffer *IntBuffer::put(intArray inputArray)
{
	if( inputArray.length > remaining() )
		assert( false ); //TODO 4J Stu - Some kind of exception?

	std::copy( inputArray.data, inputArray.data + inputArray.length, buffer+m_position );

	m_position += inputArray.length;

	return this;
}

//Writes the given int into this buffer at the current position, and then increments the position.
//
//Parameters:
//i - The int to be written
//Returns:
//This buffer
IntBuffer *IntBuffer::put(int i)
{
	assert( m_position < m_limit );

	buffer[m_position++] = i;

	return this;
}