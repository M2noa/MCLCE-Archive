#include "stdafx.h"

#include "BufferedReader.h"

//Creates a buffering character-input stream that uses a default-sized input buffer.
//Parameters:
//in - A Reader
BufferedReader::BufferedReader( Reader *in ) : reader( in ), readMark( 0 ), bufferedMark( 0 ), eofReached( false )
{
	bufferSize = 64;
	buffer = new wchar_t[bufferSize];
	memset( buffer,0,sizeof(wchar_t)*bufferSize);
	bufferMore();
}

BufferedReader::~BufferedReader()
{
	delete[] buffer;
}

void BufferedReader::bufferMore()
{
	// Don't buffer more unless we are going to read at least twice as much as what is already left
	if( bufferedMark - readMark > (BUFFER_MORE_AMOUNT / 2) )
		return;

	if( bufferSize < (bufferedMark + BUFFER_MORE_AMOUNT) )
	{
		// Enlarge the buffer
		wchar_t *temp = new wchar_t[bufferSize * 2];
		memset( temp,0,sizeof(wchar_t)*bufferSize*2);
		std::copy( buffer, buffer+bufferSize, temp );

		delete[] buffer;
		buffer = temp;
		bufferSize = bufferSize * 2;
	}

	int value = 0;
	unsigned int newCharsBuffered = 0;
	while( newCharsBuffered < BUFFER_MORE_AMOUNT && (value = reader->read() ) != -1 )
	{
		buffer[bufferedMark++] = value;
		newCharsBuffered++;
	}
}

//Closes the stream and releases any system resources associated with it. Once the stream has been closed,
//further read(), ready(), mark(), reset(), or skip() invocations will throw an IOException. Closing a previously closed stream has no effect.
void BufferedReader::close()
{
	reader->close();
}

//Reads a single character.
//Returns:
//The character read, as an integer in the range 0 to 65535 (0x00-0xffff), or -1 if the end of the stream has been reached
int BufferedReader::read()
{
	// We should have buffered at least as much as we have read
	assert( bufferedMark >= readMark );

	if( bufferedMark == readMark )
	{
		int value = reader->read();
		if( value == -1 )
			return -1;

		buffer[bufferedMark++] = value;

		bufferMore();
	}

	return buffer[readMark++];
}

//Reads characters into a portion of an array.
//This method implements the general contract of the corresponding read method of the Reader class.
//As an additional convenience, it attempts to read as many characters as possible by repeatedly invoking the read method
//of the underlying stream. This iterated read continues until one of the following conditions becomes true:
//
//The specified number of characters have been read,
//The read method of the underlying stream returns -1, indicating end-of-file, or
//The ready method of the underlying stream returns false, indicating that further input requests would block.
//If the first read on the underlying stream returns -1 to indicate end-of-file then this method returns -1.
//Otherwise this method returns the number of characters actually read.
//Subclasses of this class are encouraged, but not required, to attempt to read as many characters as possible in the same fashion.
//
//Ordinarily this method takes characters from this stream's character buffer, filling it from the underlying stream as necessary.
//If, however, the buffer is empty, the mark is not valid, and the requested length is at least as large as the buffer,
//then this method will read characters directly from the underlying stream into the given array.
//Thus redundant BufferedReaders will not copy data unnecessarily.
//
//Parameters:
//cbuf - Destination buffer
//off - Offset at which to start storing characters
//len - Maximum number of characters to read
//Returns:
//The number of characters read, or -1 if the end of the stream has been reached
int BufferedReader::read(wchar_t cbuf[], unsigned int off, unsigned int len)
{
	if( bufferSize < (bufferedMark + len) )
	{
		// Enlarge the buffer
		wchar_t *temp = new wchar_t[bufferSize * 2];
		memset( temp,0,sizeof(wchar_t)*bufferSize*2);
		std::copy( buffer, buffer+bufferSize, temp );

		delete[] buffer;
		buffer = temp;
		bufferSize = bufferSize * 2;
	}
	
	unsigned int charsRead = 0;
	while( charsRead < len && readMark <= bufferedMark )
	{
		cbuf[off + charsRead] = buffer[ readMark++ ];
		charsRead++;
	}

	int value = 0;
	while( charsRead < len && (value = reader->read() ) != -1 )
	{
		buffer[bufferedMark++] = value;
		cbuf[off+charsRead] = value;
		charsRead++;
		readMark++;
	}

	bufferMore();

	return charsRead;
}

//Reads a line of text. A line is considered to be terminated by any one of a line feed ('\n'), a carriage return ('\r'),
//or a carriage return followed immediately by a linefeed.
//Returns:
//A String containing the contents of the line, not including any line-termination characters, or null if the end of the stream has been reached
wstring BufferedReader::readLine()
{
	wstring output = L"";
	bool newLineCharFound = false;

	while( readMark < bufferedMark )
	{
		wchar_t value = buffer[readMark++];

		if( !newLineCharFound )
		{
			if( ( value == '\n')  || ( value == '\r') )
			{
				newLineCharFound = true;
			}
			else
			{
				output.push_back(value);
			}
		}
		else
		{
			if( ( value != '\n')  && ( value != '\r') )
			{
				readMark--; // Move back the read mark on char so we get this char again next time
				break;
			}
		}

		// This will only actually read more from the stream if we have less than half of the amount that
		// will be added left to read
		bufferMore();
	}
	return output;
}