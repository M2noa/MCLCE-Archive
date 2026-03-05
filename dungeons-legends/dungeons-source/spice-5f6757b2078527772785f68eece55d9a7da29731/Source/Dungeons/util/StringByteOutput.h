/********************************************************
*   (c) Mojang. All rights reserved                       *
*   (c) Microsoft. All rights reserved.                   *
*********************************************************/
#pragma once

#include "common_header.h"

#include "util/DataIO.h"

class StringByteOutput : public BytesDataOutput {
public:

	StringByteOutput(std::string& buffer) :
		mBuffer(buffer) {
	}

	virtual void writeBytes(const void* data, size_t bytes) override {
		mBuffer.append((char*)data, bytes);
	}

protected:

	std::string& mBuffer;

};

class BigEndianStringByteOutput : public StringByteOutput {

public:
	BigEndianStringByteOutput(std::string& buffer) : StringByteOutput(buffer) {
	}

	void writeReverseBytes(void* v, int length) {
		for (int i = length - 1; i >= 0; --i) {
			writeByte(static_cast<byte *>(v)[i]);
		}
	}

	virtual void writeFloat(float v) override {
		writeReverseBytes(&v, sizeof(float));
	}

	virtual void writeDouble(double v) override {
		writeReverseBytes(&v, sizeof(double));
	}

	virtual void writeShort(short v) override {
		writeReverseBytes(&v, sizeof(short));
	}

	virtual void writeInt(int v) override {
		writeReverseBytes(&v, sizeof(int));
	}

	virtual void writeLongLong(int64_t v) override {
		writeReverseBytes(&v, sizeof(int64_t));
	}
};
