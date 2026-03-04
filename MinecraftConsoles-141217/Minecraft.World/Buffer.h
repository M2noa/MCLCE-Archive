#pragma once

//A buffer is a linear, finite sequence of elements of a specific primitive type. Aside from its content,
//the essential properties of a buffer are its capacity, limit, and position:
//
//A buffer's capacity is the number of elements it contains. The capacity of a buffer is never negative and never changes.
//
//A buffer's limit is the index of the first element that should not be read or written.
//A buffer's limit is never negative and is never greater than its capacity.
//
//A buffer's position is the index of the next element to be read or written.
//A buffer's position is never negative and is never greater than its limit.
class Buffer
{
protected:
	const unsigned int m_capacity;
	unsigned int m_position;
	unsigned int m_limit;
	unsigned int m_mark;
	bool hasBackingArray;

public:
	Buffer( unsigned int capacity );
	virtual ~Buffer() {}

	Buffer *clear();
	Buffer *limit(unsigned int newLimit);
	unsigned int limit();
	Buffer *position( unsigned int newPosition );
	unsigned int position();
	unsigned int remaining();

	virtual Buffer *flip() = 0;
};