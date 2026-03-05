#pragma once

#include "Reader.h"

class InputStream;

class InputStreamReader : public Reader
{
private:
	DataInputStream *stream;

public:
	InputStreamReader(InputStream *in);

	virtual void close();
	virtual int read();
	virtual int read(wchar_t cbuf[], unsigned int offset, unsigned int length);
};