/********************************************************
*   (c) Mojang. All rights reserved                       *
*   (c) Microsoft. All rights reserved.                   *
*********************************************************/
#pragma once

#include <cstring>
#include <string>

#include "util/DataIO.h"

// Uses BitStream as a growing buffer
class RakDataOutput : public BytesDataOutput {
public:

	//RakDataOutput() {}
	RakDataOutput(RakNet::BitStream& bitstream)
		:   mBitStream(bitstream){
	}

	virtual void writeBytes(const void* data, size_t bytes) override {
		mBitStream.WriteBits((const unsigned char*)data, bytes * 8);
	}

	RakNet::BitStream& getBitStream() {
		return mBitStream;
	}

private:

	RakNet::BitStream& mBitStream;
};

class RakDataInput : public BytesDataInput {
public:

	//RakDataOutput() {}
	RakDataInput(RakNet::BitStream& bitstream)
		: mBitStream(bitstream) {
	}

	virtual bool readBytes(void* data, size_t bytes) override {
		if (numBytesLeft() < bytes) {
			return false;
		}
		mBitStream.ReadBits((unsigned char*) data, bytes * 8);
		return true;
	}

	RakNet::BitStream& getBitStream() {
		return mBitStream;
	}

	virtual size_t numBytesLeft() const override {
		return mBitStream.GetNumberOfUnreadBits() / 8;
	}

private:

	RakNet::BitStream& mBitStream;
};
