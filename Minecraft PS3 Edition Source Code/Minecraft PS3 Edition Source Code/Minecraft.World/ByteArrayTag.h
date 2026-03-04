#pragma once
#include "Tag.h"
#include "System.h"

class ByteArrayTag : public Tag
{
public:
	byteArray data;
	
	ByteArrayTag(const wstring &name) : Tag(name) { }
	ByteArrayTag(const wstring &name, byteArray data) : Tag(name) {this->data = data; }			// 4J - added ownData param
	
	void write(DataOutput *dos)
	{
		dos->writeInt(data.length);
		dos->write(data);
	}

	void load(DataInput *dis)
	{
		int length = dis->readInt();
		
		if ( data.data ) delete[] data.data;
		data  = byteArray(length);
		dis->readFully(data);
	}

	byte getId() { return TAG_Byte_Array; }

	wstring toString()
	{
		static wchar_t buf[32];
		swprintf(buf, 32, L"[%d bytes]",data.length);
		return wstring( buf );
	}

	bool equals(Tag *obj)
	{
		if (Tag::equals(obj))
		{
			ByteArrayTag *o = (ByteArrayTag *) obj;
			return ((data.data == NULL && o->data.data == NULL) || (data.data != NULL && data.length == o->data.length && memcmp(data.data, o->data.data, data.length) == 0) );
		}
		return false;
	}

	Tag *copy()
	{
		byteArray cp = byteArray(data.length);
		System::arraycopy(data, 0, &cp, 0, data.length);
		return new ByteArrayTag(getName(), cp);
	}
};