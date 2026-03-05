#include "Dungeons.h"

#include "network/BinaryStream.h"

ReadOnlyBinaryStream::ReadOnlyBinaryStream(std::string&& buffer)
	: mOwnedBuffer(std::move(buffer))
	, mBuffer(mOwnedBuffer)
{
}

ReadOnlyBinaryStream::ReadOnlyBinaryStream(const std::string& buffer, bool copyBuffer)
	: mOwnedBuffer(copyBuffer ? buffer : "")
	, mBuffer(copyBuffer ? mOwnedBuffer : buffer)
{
}

BinaryStream::BinaryStream()
	: BinaryStream("")
{
}

BinaryStream::BinaryStream(std::string&& buffer)
	: ReadOnlyBinaryStream(mOwnedBuffer, false)
	, mOwnedBuffer(std::move(buffer))
	, mBuffer(mOwnedBuffer)
{
}

BinaryStream::BinaryStream(std::string& buffer, bool copyBuffer)
	: ReadOnlyBinaryStream(copyBuffer ? mOwnedBuffer : buffer, false)
	, mOwnedBuffer(copyBuffer ? buffer : "")
	, mBuffer(copyBuffer ? mOwnedBuffer : buffer)
{
}

BinaryStream::BinaryStream(const std::string& buffer)
	: ReadOnlyBinaryStream(mOwnedBuffer, false)
	, mOwnedBuffer(buffer)
	, mBuffer(mOwnedBuffer)
{
}

void BinaryStream::reserve(size_t size){
	mBuffer.reserve(size);
}

void BinaryStream::reset(){
	mBuffer.clear();
	setReadPointer(0);
}

std::string BinaryStream::getAndReleaseData() {
	auto buffer = std::move(mBuffer);
	reset();
	return buffer;
}

bool ReadOnlyBinaryStream::read(void *target, size_t num){
	if(num == 0){
		return true;
	}

	uint64_t checkedNumber = static_cast<uint64_t>(mReadPointer) + static_cast<uint64_t>(num); //This could overflow otherwise

	if(checkedNumber > mBuffer.length()){ //We exceed the size of this buffer
		memset(target, 0, num); //Set everything to empty on the target
		if(mReadPointer < mBuffer.length()){ //If we still have some data left, read it into target
			mBuffer.copy((char*)target, static_cast<uint64_t>(mBuffer.length() - mReadPointer), mReadPointer);
			mReadPointer = static_cast<unsigned int>(mBuffer.length());
		}
		return false;
	}

	mBuffer.copy((char*)target, num, mReadPointer);
	mReadPointer += num;

	return true;
}

void BinaryStream::write(const void *origin, size_t num){
	mBuffer.append((const char*) origin, num);
}

bool ReadOnlyBinaryStream::getBool(){
	return getByte() > 0;
}

void BinaryStream::writeBool(bool value){
	writeByte((uint8_t) (value ? 1 : 0));
}

uint8_t ReadOnlyBinaryStream::getByte(){
	uint8_t value;
	read(&value, sizeof(value));

	return value;
}

void BinaryStream::writeByte(uint8_t value){
	write(&value, sizeof(value));
}

int16_t ReadOnlyBinaryStream::getSignedShort(){
	int16_t value;
	read(&value, sizeof(value));

	return value;
}

void BinaryStream::writeSignedShort(int16_t value){
	write(&value, sizeof(value));
}

uint16_t ReadOnlyBinaryStream::getUnsignedShort(){
	uint16_t value;
	read(&value, sizeof(value));

	return value;
}

void BinaryStream::writeUnsignedShort(uint16_t value){
	write(&value, sizeof(value));
}

int32_t ReadOnlyBinaryStream::getSignedBigEndianInt(){
	int32_t value;
	read(&value, sizeof(value));

	return Util::swapEndian<int32_t>(value);
}

void BinaryStream::writeSignedBigEndianInt(int32_t value){
	int32_t swap = Util::swapEndian<int32_t>(value);
	write(&swap, sizeof(swap));
}

int32_t ReadOnlyBinaryStream::getSignedInt(){
	int32_t value;
	read(&value, sizeof(value));

	return value;
}

void BinaryStream::writeSignedInt(int32_t value){
	write(&value, sizeof(value));
}

uint32_t ReadOnlyBinaryStream::getUnsignedInt(){
	uint32_t value;
	read(&value, sizeof(value));

	return value;
}

void BinaryStream::writeUnsignedInt(uint32_t value){
	write(&value, sizeof(value));
}

int64_t ReadOnlyBinaryStream::getSignedInt64(){
	int64_t value;
	read(&value, sizeof(value));

	return value;
}

void BinaryStream::writeSignedInt64(int64_t value){
	write(&value, sizeof(value));
}

uint64_t ReadOnlyBinaryStream::getUnsignedInt64(){
	uint64_t value;
	read(&value, sizeof(value));

	return value;
}

void BinaryStream::writeUnsignedInt64(uint64_t value){
	write(&value, sizeof(value));
}

double ReadOnlyBinaryStream::getDouble(){
	double value;
	read(&value, sizeof(value));

	return value;
}

void BinaryStream::writeDouble(double value){
	write(&value, sizeof(value));
}

float ReadOnlyBinaryStream::getFloat(){
	float value;
	read(&value, sizeof(value));

	return value;
}

void BinaryStream::writeFloat(float value){
	write(&value, sizeof(value));
}

float ReadOnlyBinaryStream::getFixedFloat(double size /*= DEFAULT_FIXED_FLOAT_SIZE*/){
	return static_cast<float>(getVarInt64() / size);
}

void BinaryStream::writeFixedFloat(float value, double size /*= DEFAULT_FIXED_FLOAT_SIZE*/){
	writeVarInt64(static_cast<int64_t>(value * size));
}

float ReadOnlyBinaryStream::getNormalizedFloat(){
	return static_cast<float>(getVarInt64() / NORMALIZED_FLOAT_SIZE);
}

void BinaryStream::writeNormalizedFloat(float value){
	writeVarInt64(static_cast<int64_t>(value * NORMALIZED_FLOAT_SIZE));
}

int32_t ReadOnlyBinaryStream::getVarInt(){
	uint32_t decoded_value = getUnsignedVarInt();

	return (int32_t) (decoded_value & 1 ? ~(decoded_value >> 1) : (decoded_value >> 1));
}

uint32_t ReadOnlyBinaryStream::getUnsignedVarInt(){
	uint32_t decoded_value = 0;
	int shift_amount = 0;

	uint8_t in;

	do{
		in = getByte();
		decoded_value |= (uint32_t)(in & 0x7F) << shift_amount;
		shift_amount += 7;
	}while ((in & 0x80) != 0);

	return decoded_value;
}

void BinaryStream::writeVarInt(int32_t value){
	writeUnsignedVarInt(static_cast<uint32_t>(value < 0 ? ~(value << 1) : (value << 1)));
}

void BinaryStream::writeUnsignedVarInt(uint32_t uvalue){

	do{
		uint8_t next_byte = uvalue & 0x7F;
		uvalue >>= 7;

		if (uvalue)
			next_byte |= 0x80;

		writeByte(next_byte);

	}while (uvalue);
}

int64_t ReadOnlyBinaryStream::getVarInt64(){
	uint64_t decoded_value = getUnsignedVarInt64();

	return (int64_t) (decoded_value & 1 ? ~(decoded_value >> 1) : (decoded_value >> 1));
}

uint64_t ReadOnlyBinaryStream::getUnsignedVarInt64(){
	uint64_t decoded_value = 0;
	int shift_amount = 0;

	uint8_t in;

	do{
		in = getByte();
		decoded_value |= (uint64_t)(in & 0x7F) << shift_amount;
		shift_amount += 7;
	}while ((in & 0x80) != 0);

	return decoded_value;
}

void BinaryStream::writeUnsignedVarInt64(uint64_t uvalue){
	do{
		uint8_t next_byte = uvalue & 0x7F;
		uvalue >>= 7;

		if (uvalue)
			next_byte |= 0x80;

		writeByte(next_byte);

	}while (uvalue);
}

void BinaryStream::writeVarInt64(int64_t value){
	writeUnsignedVarInt64(static_cast<uint64_t>(value < 0 ? ~(value << 1) : (value << 1)));
}

std::string ReadOnlyBinaryStream::getString(){
	uint32_t length = getUnsignedVarInt();
	std::string buffer;
	if(length > buffer.max_size()){
		setReadPointer(getLength());
		DEBUG_ASSERT(length <= buffer.max_size(), "String allocation overflow");
		return buffer;
	}
	buffer.resize(length);
	read((void*)buffer.data(), length);

	return buffer;
}

void BinaryStream::writeString(const std::string &value){
	writeUnsignedVarInt(value.length());
	if(value.length() > 0){
		write(value.data(), value.length());
	}
}

void BinaryStream::writeStream(BinaryStream &stream){
	write(stream.getData().data() + stream.getReadPointer(), stream.getUnreadLength());
	stream.setReadPointer(stream.getLength());
}

template <typename Type>
void BinaryStream::writeType(const Type &in){
	serialize<Type>::write(in, *this);
}

template <typename Type>
Type ReadOnlyBinaryStream::getType(){
	return std::move(serialize<Type>::read(*this));
}

//#define GUARD_FOR_SERIALIZE_HELPER 1
//#include "SerializeHelper.h"
//#undef GUARD_FOR_SERIALIZE_HELPER
