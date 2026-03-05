#pragma once

#pragma warning( disable : 4996 )
//Disable warning due to the deprecation of std::string::copy, even when we are checking parameters beforehand

#include <string>
#include <cmath>

//5 bits
#define DEFAULT_FIXED_FLOAT_SIZE 32.
//31 bits
#define NORMALIZED_FLOAT_SIZE 2147483647.

class BinaryStream;
class ReadOnlyBinaryStream;

//specialization for serialization
template<typename Type>
struct serialize{
	void static write(const Type& val, BinaryStream& stream);

	Type static read(ReadOnlyBinaryStream& stream);
};

class ReadOnlyBinaryStream {
	friend class BinaryDataOutput;
	friend class BinaryDataInput;
public:
	explicit ReadOnlyBinaryStream(std::string&& buffer);
	explicit ReadOnlyBinaryStream(const std::string& buffer, bool copyBuffer = true);

	virtual ~ReadOnlyBinaryStream() {
	}

	void operator=(const ReadOnlyBinaryStream&) = delete;

	void setReadPointer(unsigned int pos) {
		if (pos > mBuffer.length()) {
			mReadPointer = static_cast<unsigned int>(mBuffer.length());
		} else {
			mReadPointer = pos;
		}
	}

	unsigned int getReadPointer() const {
		return mReadPointer;
	}

	size_t getUnreadLength() const {
		return mBuffer.length() - static_cast<size_t>(mReadPointer);
	}

	size_t getLength() const {
		return mBuffer.length();
	}

	bool getBool();

	uint8_t getByte();

	uint16_t getUnsignedShort();

	int16_t getSignedShort();

	uint32_t getUnsignedInt();

	int32_t getSignedBigEndianInt();

	int32_t getSignedInt();

	uint64_t getUnsignedInt64();

	int64_t getSignedInt64();

	uint32_t getUnsignedVarInt();

	uint64_t getUnsignedVarInt64();

	int32_t getVarInt();

	int64_t getVarInt64();

	double getDouble();

	float getFloat();

	float getFixedFloat(double size = DEFAULT_FIXED_FLOAT_SIZE);

	float getNormalizedFloat();

	std::string getString();

	template <typename Type>
	Type getType();

	template <typename Type>
	void readType(Type& type) {
		type = getType<Type>();
	}

	const std::string& getData() const {
		return mBuffer;
	}

private:
	virtual bool read(void* target, size_t num);

	unsigned int mReadPointer = 0;

	const std::string mOwnedBuffer;
	const std::string& mBuffer;
};

class BinaryStream : public ReadOnlyBinaryStream {
	friend class BinaryDataOutput;
	friend class BinaryDataInput;
public:

	BinaryStream();
	explicit BinaryStream(std::string&& buffer);
	explicit BinaryStream(std::string& buffer, bool copyBuffer = true);
	explicit BinaryStream(const std::string& buffer);

	virtual ~BinaryStream(){
	}

	void operator=(const BinaryStream&) = delete;

	void reserve(size_t size);

	void reset();

	std::string getAndReleaseData();

	void writeBool(bool value);

	void writeByte(uint8_t value);

	void writeUnsignedShort(uint16_t value);

	void writeSignedShort(int16_t value);

	void writeUnsignedInt(uint32_t value);

	void writeSignedBigEndianInt(int32_t value);

	void writeSignedInt(int32_t value);

	void writeUnsignedInt64(uint64_t value);

	void writeSignedInt64(int64_t value);

	//TODO: add checks for max 5 bytes
	void writeUnsignedVarInt(uint32_t value);

	void writeUnsignedVarInt64(uint64_t value);

	void writeVarInt(int32_t value);

	//TODO: add checks for max 10 bytes
	void writeVarInt64(int64_t value);

	void writeDouble(double value);

	void writeFloat(float value);

	void writeFixedFloat(float value, double size = DEFAULT_FIXED_FLOAT_SIZE);

	//Between [-1.0, 1.0]
	void writeNormalizedFloat(float value);

	void writeString(const std::string& value);

	void writeStream(BinaryStream& stream);

	template <typename Type>
	void writeType(const Type &in);

private:

	void write(const void* origin, size_t num);

	std::string mOwnedBuffer;
	std::string& mBuffer;
};
