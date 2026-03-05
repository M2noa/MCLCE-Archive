/********************************************************
*   (c) Mojang. All rights reserved                       *
*   (c) Microsoft. All rights reserved.                   *
*********************************************************/
#pragma once

#include "common_header.h"
#include "network/BinaryStream.h"

// Interface for writing primitives to a stream
class IDataOutput {
public:

	virtual ~IDataOutput() {
	}

	// Write a "Pascal" string [Len(Short)][Characters, len={strlen(),s.length()}]
	//virtual void writePStr(const char* v, int len = -1) = 0;
	//virtual void writeCStr(const char* v, int len = -1) = 0;
	virtual void writeString(const std::string& v) = 0;
	virtual void writeLongString(const std::string& v) = 0;
	virtual void writeFloat(float v)    = 0;
	virtual void writeDouble(double v)  = 0;

	virtual void writeByte(char v)      = 0;
	virtual void writeShort(short v)    = 0;
	virtual void writeInt(int v)        = 0;
	virtual void writeLongLong(int64_t v) = 0;

	virtual void writeBytes(const void* data, size_t bytes) = 0;
};

// Interface for reading primitives from a stream
class IDataInput {
public:

	virtual ~IDataInput() {
	}

	virtual std::string readString() = 0;
	virtual std::string readLongString() = 0;

	//virtual void		readPStr(char**) = 0;
	//virtual void		readCStr(char** s, int len = -1) = 0;
	virtual float       readFloat()     = 0;
	virtual double      readDouble()    = 0;

	virtual char        readByte()      = 0;
	virtual short       readShort()     = 0;
	virtual int         readInt()       = 0;
	virtual int64_t readLongLong()  = 0;

	virtual bool readBytes(void* data, size_t bytes) = 0;
	virtual size_t numBytesLeft() const = 0;

};

/**
 * Redirects all calls to writeBytes
 */
class BytesDataOutput : public IDataOutput {
public:
	//virtual void writePStr(const char* v, int len = -1);
	//virtual void writeCStr(const char* v, int len = -1);
	virtual void writeString(const std::string& v) override;
	virtual void writeLongString(const std::string& v) override;

	virtual void writeFloat(float v) override {
		writeBytes(&v, sizeof(float));
	}

	virtual void writeDouble(double v) override {
		writeBytes(&v, sizeof(double));
	}

	virtual void writeByte(char v) override {
		// This will always be the correct endian, bytes, I luv u
		writeBytes(&v, 1);
	}

	virtual void writeShort(int16_t v) override {
		writeBytes(&v, sizeof(int16_t));
	}

	virtual void writeInt(int32_t v) override {
		writeBytes(&v, sizeof(int32_t));
	}

	virtual void writeLongLong(int64_t v) override {
		writeBytes(&v, sizeof(int64_t));
	}

	virtual void writeBytes(const void* data, size_t bytes) override = 0;
};

/**
 * Redirects all calls to readBytes
 */
class BytesDataInput : public IDataInput {
public:
	//virtual void readPStr(char** s);
	//virtual void readCStr(char* s, int len = -1);
	virtual std::string readString() override;
	virtual std::string readLongString() override;

	virtual float readFloat() override {
		float o = 0;
		readBytes(&o, sizeof(float));
		return o;
	}

	virtual double readDouble() override {
		double o = 0;
		readBytes(&o, sizeof(double));
		return o;
	}

	virtual char readByte() override {
		char o = 0;
		// This will always be the correct endian, bytes, I luv u
		readBytes(&o, 1);
		return o;
	}

	virtual short readShort() override {
		int16_t o = 0;
		readBytes(&o, sizeof(int16_t));
		return o;
	}

	virtual int readInt() override {
		int32_t o = 0;
		readBytes(&o, sizeof(int32_t));
		return o;
	}

	virtual int64_t readLongLong() override {
		int64_t o = 0;
		readBytes(&o, sizeof(int64_t));
		return o;
	}

	virtual bool readBytes(void* data, size_t bytes) override = 0;

private:

	static const int MAX_STRING_LENGTH = SHRT_MAX;
};

class BinaryDataInput : public BytesDataInput {
public:

	BinaryDataInput(ReadOnlyBinaryStream& stream)
			:   mStream(stream){
	}

	virtual std::string readString() override{
		return mStream.getString();
	}

	virtual std::string readLongString() override{
		return mStream.getString();
	}

	virtual float readFloat() override {
		return mStream.getFloat();
	}

	virtual double readDouble() override {
		return mStream.getDouble();
	}

	virtual char readByte() override {
		return mStream.getByte();
	}

	virtual short readShort() override {
		return mStream.getSignedShort();
	}

	virtual int readInt() override {
		return mStream.getVarInt();
	}

	virtual int64_t readLongLong() override {
		return mStream.getVarInt64();
	}

	virtual bool readBytes(void* data, size_t bytes) override {
		return mStream.read(data, bytes);
	}

	virtual size_t numBytesLeft() const override{
		return mStream.getUnreadLength();
	}

	ReadOnlyBinaryStream& getStream() {
		return mStream;
	}

private:

	ReadOnlyBinaryStream& mStream;
};

class BinaryDataOutput : public BytesDataOutput {
public:

	BinaryDataOutput(BinaryStream& stream)
			:   mStream(stream){
	}

	virtual void writeString(const std::string& v) override{
		mStream.writeString(v);
	}
	virtual void writeLongString(const std::string& v) override{
		mStream.writeString(v);
	}

	virtual void writeFloat(float v) override {
		mStream.writeFloat(v);
	}

	virtual void writeDouble(double v) override {
		mStream.writeDouble(v);
	}

	virtual void writeByte(char v) override {
		mStream.writeByte(v);
	}

	virtual void writeShort(int16_t v) override {
		mStream.writeSignedShort(v);
	}

	virtual void writeInt(int32_t v) override {
		mStream.writeVarInt(v);
	}

	virtual void writeLongLong(int64_t v) override {
		mStream.writeVarInt64(v);
	}

	virtual void writeBytes(const void* data, size_t bytes) override {
		mStream.write(data, bytes);
	}

	BinaryStream& getStream() {
		return mStream;
	}

private:

	BinaryStream& mStream;
};

class PrintStream {
public:

	void print(const std::string& s) {
		UNUSED_PARAMETER(s);
	}

	void println(const std::string& s) {
		print(s);
		print("\n");
	}

};

class FileError {
public:

	static const int NOT_OPENED = 1;
	static const int NOT_FULLY_HANDLED = 2;
};
