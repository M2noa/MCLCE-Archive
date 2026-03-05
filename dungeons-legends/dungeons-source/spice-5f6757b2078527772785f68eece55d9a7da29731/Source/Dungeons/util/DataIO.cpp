#include "Dungeons.h"

#include "util/DataIO.h"

//
// BytesDataOutput
//

void BytesDataOutput::writeString( const std::string& v ){
	DEBUG_ASSERT(v.length() <= 0x7fff, "Writing strings longer than this wont work. Hopefully we're not relying on it.");
	const int length = v.length() & 0x7fff;
	writeShort(length);
	writeBytes(v.c_str(), length);
	//LOGI("Writing: %d bytes as String: %s\n", v.length(), v.c_str());
}

void BytesDataOutput::writeLongString( const std::string& v ){
	writeInt(v.length());
	writeBytes(v.c_str(), v.length());
	//LOGI("Writing: %d bytes as String: %s\n", v.length(), v.c_str());
}

//
// BytesDataInput
//

std::string BytesDataInput::readLongString() {
	int len = readInt();
	if (len <= 0) {
		return "";
	}

	std::string buffer(len, 0);
	readBytes((void*)buffer.data(), len);

	return buffer;
}

std::string BytesDataInput::readString() {
	int len = readShort();
	if (len <= 0) {
		return "";
	}
	if (len > MAX_STRING_LENGTH - 1) {
		len = MAX_STRING_LENGTH - 1;
	}

	std::string buffer(len, 0);
	readBytes((void*)buffer.data(), len);

	return buffer;
}
