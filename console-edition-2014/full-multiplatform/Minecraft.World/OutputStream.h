#pragma once
// 4J Stu - Represents Java standard lib abstract

class OutputStream
{
public:
	virtual ~OutputStream() {}

	virtual void write(unsigned int b) = 0;
	virtual void write(byteArray b) = 0;
	virtual void write(byteArray b, unsigned int offset, unsigned int length) = 0;
	virtual void close() = 0;
	virtual void flush() = 0;
};