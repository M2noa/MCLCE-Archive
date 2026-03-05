/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#pragma once

enum class AttributeOperands : int {
	OPERAND_MIN = 0,
	OPERAND_MAX = 1,
	OPERAND_CURRENT = 2,
	TOTAL_OPERANDS = 3,
	OPERAND_INVALID = TOTAL_OPERANDS,
};

enum class RedefinitionMode : int8_t {
	KeepCurrent = 0,
	UpdateToNewDefault
};

class Attribute {
public:

	Attribute(const std::string& name, RedefinitionMode redefMode, bool syncable = false);

	const std::string& getName() const;
	bool isClientSyncable() const;
	RedefinitionMode getRedefinitionMode() const;

	static Attribute& getByName(const std::string& attribute);

private:

	RedefinitionMode mRedefinitionMode;
	bool mSyncable;
	std::string mName;
};

class AttributeCollection {
	friend class Attribute;

	static AttributeCollection& instance();

	Attribute& getAttribute(const std::string& name);
	void addAttribute(const std::string& name, Attribute* attribute);

private:

	std::unordered_map<std::string, Attribute*> mAttributesMap;

public:
	static bool hasAttribute(const std::string& name);
};