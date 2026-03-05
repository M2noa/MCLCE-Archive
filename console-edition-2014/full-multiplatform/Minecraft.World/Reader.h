#pragma once

class Reader
{
public:
	virtual ~Reader() {}

	virtual void close() = 0; //Closes the stream and releases any system resources associated with it.
	virtual int read() = 0; //Reads a single character.
	virtual int read(wchar_t cbuf[], unsigned int off, unsigned int len) = 0; //Reads characters into a portion of an array.
};