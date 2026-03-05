#include "stdafx.h"
#include "Buffer.h"

Buffer::Buffer( unsigned int capacity ) : m_capacity( capacity ), m_position( 0 ), m_limit( capacity ), hasBackingArray( false )
{
}

//Clears this buffer. The position is set to zero, the limit is set to the capacity, and the mark is discarded.
//This method does not actually erase the data in the buffer, but it is named as if it did because it will most often
//be used in situations in which that might as well be the case.
//
//Returns:
//This buffer
Buffer *Buffer::clear()
{
	m_position = 0;
	m_limit = m_capacity;

	return this;
}

//Sets this buffer's limit. If the position is larger than the new limit then it is set to the new limit.
//If the mark is defined and larger than the new limit then it is discarded.
//Parameters:
//newLimit - The new limit value; must be non-negative and no larger than this buffer's capacity
//Returns:
//This buffer
Buffer *Buffer::limit( unsigned int newLimit )
{
	assert( newLimit <= m_capacity );

	m_limit = newLimit;

	if( m_position > newLimit )
		m_position = newLimit;

	return this;
}

unsigned int Buffer::limit()
{
	return m_limit;
}

//Sets this buffer's position. If the mark is defined and larger than the new position then it is discarded.
//Parameters:
//newPosition - The new position value; must be non-negative and no larger than the current limit
//Returns:
//This buffer
Buffer *Buffer::position( unsigned int newPosition )
{
	assert( newPosition <= m_limit );

	m_position = newPosition;

	return this;
}

//Returns this buffer's position.
//Returns:
//The position of this buffer
unsigned int Buffer::position()
{
	return m_position;
}

//Returns the number of elements between the current position and the limit.
//Returns:
//The number of elements remaining in this buffer
unsigned int Buffer::remaining()
{
	return m_limit - m_position;
}