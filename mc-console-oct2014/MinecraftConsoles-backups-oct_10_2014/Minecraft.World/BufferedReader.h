#pragma once

#include "Reader.h"

class BufferedReader : public Reader
{
private:
	Reader *reader;
	wchar_t *buffer;
		
	unsigned int readMark;
	unsigned int bufferedMark;
	unsigned int bufferSize;
	bool eofReached;

	static const unsigned int BUFFER_MORE_AMOUNT = 64;
	void bufferMore();

public:
	BufferedReader( Reader *in );
	virtual ~BufferedReader();

	virtual void close();
	virtual int read();
	virtual int read(wchar_t cbuf[], unsigned int off, unsigned int len);
	wstring readLine();
};