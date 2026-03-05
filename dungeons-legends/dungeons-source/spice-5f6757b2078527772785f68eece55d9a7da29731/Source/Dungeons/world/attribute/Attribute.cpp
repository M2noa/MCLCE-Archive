/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#include "Dungeons.h"

#include "world/attribute/Attribute.h"

Attribute::Attribute(const std::string& name, RedefinitionMode redefMode, bool isSyncable)
	: mName(name)
	, mRedefinitionMode(redefMode)
	, mSyncable(isSyncable) {
	DEBUG_ASSERT(!name.empty(), "Attribute Name cannot be empty");
	AttributeCollection::instance().addAttribute(name, this);
}

const std::string& Attribute::getName() const {
	return mName;
}

bool Attribute::isClientSyncable() const {
	return mSyncable;
}

RedefinitionMode Attribute::getRedefinitionMode() const {
	return mRedefinitionMode;
}

Attribute& Attribute::getByName(const std::string& attribute) {
	auto& attributeRef = AttributeCollection::instance().getAttribute(attribute);
	return attributeRef;
}

////////////////AtributeCollection implementation//////////////////

AttributeCollection& AttributeCollection::instance() {
	static AttributeCollection me;
	return me;
}

Attribute& AttributeCollection::getAttribute(const std::string& name) {

	auto attribItr = mAttributesMap.find(name);
	if (attribItr == mAttributesMap.end()) {
		DEBUG_FAIL("Invalid Attribute Requested!");
		return *((mAttributesMap.begin())->second);
	}

	return *(attribItr->second);
}

void AttributeCollection::addAttribute(const std::string& name, Attribute* attribute) {

	DEBUG_ASSERT(mAttributesMap.find(name) == mAttributesMap.end(), "Duplicate Attribute Created!");
	mAttributesMap[name] = attribute;
}

bool AttributeCollection::hasAttribute(const std::string& name) {
	auto attribItr = AttributeCollection::instance().mAttributesMap.find(name);
	return attribItr != AttributeCollection::instance().mAttributesMap.end();
}
