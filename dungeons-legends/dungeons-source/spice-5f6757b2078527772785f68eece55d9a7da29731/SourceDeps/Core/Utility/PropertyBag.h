/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#pragma once

#include <json/json.h>
#include <unordered_map>

template<typename T>
struct jsonValConversion {
	bool is(const Json::Value& val) const;
	T as(const Json::Value& val) const;

};

template<>
struct jsonValConversion<bool> {
	bool is(const Json::Value& val) const {
		return val.isBool();
	}

	bool as(const Json::Value& val) const {
		return val.asBool();
	}

};

template<>
struct jsonValConversion<int> {
	bool is(const Json::Value& val) const {
		//IsUInt was addedbecause sometimes the json saves out ints as uints, and you should be able to pull it out as either just fine
		return val.isInt() || val.isUInt();
	}

	int as(const Json::Value& val) const {
		return val.asInt();
	}

};

template<>
struct jsonValConversion<float> {
	bool is(const Json::Value& val) const {
		return val.isNumeric();
	}

	float as(const Json::Value& val) const {
		return val.asFloat();
	}

};

template<>
struct jsonValConversion<std::string> {
	bool is(const Json::Value& val) const {
		return val.isString();
	}

	std::string as(const Json::Value& val) const {
		return val.asString();
	}

};

template<>
struct jsonValConversion<Json::Value> {
	bool is(const Json::Value& val) const {
		UNUSED_PARAMETER(val);
		return true;
	}

	Json::Value as(const Json::Value& val) const {
		return val;
	}

};

template<>
struct jsonValConversion<std::vector<bool>> {
	bool is(const Json::Value& val) const {
		return val.isArray();
	}

	std::vector<bool> as(const Json::Value& val) const {
		std::vector<bool> returnVector;
		int size = val.size();
		returnVector.resize(size, false);
		for (int index = 0; index < size; ++index) {
			returnVector[index] = val[index].asBool();
		}
		return returnVector;
	}
};

template<>
struct jsonValConversion<std::vector<int>> {
	bool is(const Json::Value& val) const {
		return val.isArray();
	}

	std::vector<int> as(const Json::Value& val) const {
		std::vector<int> returnVector;
		int size = val.size();
		returnVector.resize(size, 0);
		for (int index = 0; index < size; ++index) {
			returnVector[index] = val[index].asInt();
		}
		return returnVector;
	}
};

template<>
struct jsonValConversion<std::vector<std::string>> {
	bool is(const Json::Value& val) const {
		return val.isArray();
	}

	std::vector<std::string> as(const Json::Value& val) const {
		std::vector<std::string> returnVector;
		int size = val.size();
		returnVector.resize(size);
		for (int index = 0; index < size; ++index) {
			returnVector[index] = val[index].asString();
		}
		return returnVector;
	}
};

class PropertyBag {
public:

	PropertyBag();
	PropertyBag(const Json::Value& jsonValue);
	PropertyBag(const std::string& serializedJson);

	PropertyBag(const PropertyBag& other) = delete;

	std::unique_ptr<PropertyBag> clone() const;

	template<typename T>
	bool is(const std::string& key) const {
		if (!mJsonValue.isNull() && mJsonValue.isObject()) {
			const Json::Value& val = mJsonValue[key];
			jsonValConversion<T> converter;
			return converter.is(val);
		}
		return false;
	}

	template<typename T>
	T get(const std::string& key, T defaultVal) const {
		if (!mJsonValue.isNull() && mJsonValue.isObject()) {
			const Json::Value& val = mJsonValue[key];
			jsonValConversion<T> converter;
			if (converter.is(val)) {
				return converter.as(val);
			}
		}
		return defaultVal;
	}

	void set(const std::string& key, std::vector<bool> val);
	void set(const std::string& key, std::vector<int> val);
	void set(const std::string& key, std::vector<std::string> val);

	bool has(const std::string& key) const;
	size_t memberCount() const;

	std::string toString() const;
	Json::Value toJsonValue() const;

	int getChangeVersion() const;

protected:
	Json::Value mJsonValue;
	int mChangeVersion;
};
