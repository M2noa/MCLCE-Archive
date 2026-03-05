#pragma once

/* import java.io.* */
#include "common_header.h"

#include "util/DataIO.h"
#include "CommonTypes.h"

class Tag {
public:

	virtual ~Tag() {
	}

	virtual void deleteChildren() {
	}

	enum class Type : uint8_t {
		End = 0,
		Byte = 1,
		Short = 2,
		Int = 3,
		Int64 = 4,
		Float = 5,
		Double = 6,
		ByteArray = 7,
		String = 8,
		List = 9,
		Compound = 10,
		IntArray = 11,
	};
	
	static const std::string NullString;

	virtual void write(IDataOutput& dos) const = 0;	///*throws IOException*/;
	virtual void load(IDataInput& dis) = 0;	///*throws IOException*/;

	virtual std::string toString() const = 0;
	virtual Type getId() const = 0;

	virtual bool equals(const Tag& rhs) const;

	virtual void print(PrintStream& out) const;
	virtual void print(const std::string& prefix, PrintStream& out) const;

	virtual Tag* setName(const std::string& name);
	virtual std::string getName() const;

	static Unique<Tag> readNamedTag(IDataInput& dis, std::string& name);
	static void writeNamedTag(const std::string& name, const Tag& tag, IDataOutput& dos);

	static Unique<Tag> newTag(Type type, const std::string& name);

	static std::string getTagName(Type type);

	virtual Unique<Tag> copy() const = 0;

	static const int TAGERR_OUT_OF_BOUNDS = 1;
	static const int TAGERR_BAD_TYPE = 2;
protected:

	Tag(const std::string& name);
};
