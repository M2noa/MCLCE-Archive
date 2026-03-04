#pragma once
// 4J Stu - Represents Java standard lib abstract

#include "OutputStream.h"

class FileOutputStream : public OutputStream
{
public:
	FileOutputStream(const File &file);
	virtual ~FileOutputStream();
	virtual void write(unsigned int b);
	virtual void write(byteArray b);
	virtual void write(byteArray b, unsigned int offset, unsigned int length);
	virtual void close();
	virtual void flush() {}

private:
	HANDLE m_fileHandle;
};