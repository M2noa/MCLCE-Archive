/********************************************************
*   (c) Mojang. All rights reserved                       *
*   (c) Microsoft. All rights reserved.                   *
*********************************************************/
#pragma once

#include "common_header.h"

#include "util/DataIO.h"

class StringByteInput : public BytesDataInput {

public:

	StringByteInput(const std::string& in, int startIdx = 0, int endIdx = 0 ) :
		mBuffer(in)
		, mIdx(startIdx)
		, mEnd( endIdx ? endIdx : in.length()){

	}

	virtual bool readBytes(void* data, size_t bytes) override {
		if (mIdx == mBuffer.size()) {
			return false;
		}
		else {
			const size_t toRead = std::min(numBytesLeft(), bytes);
			memcpy(data, mBuffer.data() + mIdx, toRead);
			mIdx += toRead;
			return true;
		}
	}

	virtual size_t numBytesLeft() const override {
		return mEnd - mIdx;
	}

protected:

	int mIdx, mEnd;
	const std::string& mBuffer;
};

class BigEndianStringByteInput : public StringByteInput {

public:

	BigEndianStringByteInput(const std::string& in, int startIdx = 0, int endIdx = 0) : StringByteInput(in, startIdx, endIdx) {
	}

	virtual bool readBytes(void* data, size_t bytes) override {
		if (mIdx == mBuffer.size()) {
			return false;
		}
		else {
			const int toRead = std::min(numBytesLeft(), bytes);
			memcpy(data, mBuffer.data() + mIdx, toRead);
			mIdx += toRead;
			return true;
		}
	}

	virtual bool readBigEndianBytes(void* data, size_t bytes) {
		assert(bytes == 2 || bytes == 4 || bytes == 8);
		if (mIdx == mBuffer.size()) {
			return false;
		}
		else {
			const int toRead = std::min(numBytesLeft(), bytes);
			memcpy(data, mBuffer.data() + mIdx, toRead);
			mIdx += toRead;
			// BigEndian - swap some bytes
			byte *ptrLeft = static_cast<byte*>(data);
			byte *ptrRight = static_cast<byte*>(data) + bytes - 1;
			while (ptrLeft <= ptrRight) {
				byte temp = *ptrLeft;
				*ptrLeft = *ptrRight;
				*ptrRight = temp;

				++ptrLeft;
				--ptrRight;
			}
			return true;
		}
	}

	virtual float readFloat() override {
		float o = 0;
		readBigEndianBytes(&o, sizeof(float));
		return o;
	}

	virtual double readDouble() override {
		double o = 0;
		readBigEndianBytes(&o, sizeof(double));
		return o;
	}

	virtual short readShort() override {
		int16_t o = 0;
		readBigEndianBytes(&o, sizeof(int16_t));
		return o;
	}

	virtual int readInt() override {
		int32_t o = 0;
		readBigEndianBytes(&o, sizeof(int32_t));
		return o;
	}

	virtual int64_t readLongLong() override {
		int64_t o = 0;
		readBigEndianBytes(&o, sizeof(int64_t));
		return o;
	}
};
