#include "pch_core.h"

#include "Core/Utility/PropertyBag.h"

PropertyBag::PropertyBag()
	: mJsonValue()
	, mChangeVersion(0) {
}

PropertyBag::PropertyBag(const Json::Value& jsonValue)
	: mJsonValue(jsonValue)
	, mChangeVersion(0) {
}

PropertyBag::PropertyBag(const std::string& serializedJson)
	: mJsonValue()
	, mChangeVersion(0) {

	Json::Reader reader;
	reader.parse(serializedJson, mJsonValue);
}

std::unique_ptr<PropertyBag> PropertyBag::clone() const {
	return std::make_unique<PropertyBag>(mJsonValue);
}

void PropertyBag::set(const std::string& key, std::vector<bool> val) {
	mChangeVersion++;
	auto& newValue = mJsonValue[key];

	size_t size = val.size();
	for (int index = 0; index < (int)size; ++index) {
		newValue[index] = (int)val[index];
	}
}

void PropertyBag::set(const std::string& key, std::vector<int> val) {
	mChangeVersion++;
	auto& newValue = mJsonValue[key];

	size_t size = val.size();
	for (int index = 0; index < (int)size; ++index) {
		newValue[index] = val[index];
	}
}

void PropertyBag::set(const std::string& key, std::vector<std::string> val) {
	mChangeVersion++;
	auto& newValue = mJsonValue[key];

	size_t size = val.size();
	for (int index = 0; index < (int)size; ++index) {
		newValue[index] = val[index];
	}
}

bool PropertyBag::has(const std::string& key) const {
	if (!mJsonValue.isNull() && mJsonValue.isObject()) {
		const Json::Value& val = mJsonValue[key];
		return !val.isNull();
	}
	return false;
}

size_t PropertyBag::memberCount() const {
	if (mJsonValue.isNull()) {
		return 0;
	}

	return mJsonValue.isObject() ? mJsonValue.size() : 1;
}

std::string PropertyBag::toString() const {
	return mJsonValue.toStyledString();
}

Json::Value PropertyBag::toJsonValue() const {
	return mJsonValue;
}

int PropertyBag::getChangeVersion() const {
	return mChangeVersion;
}
