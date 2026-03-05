/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include <string>
#include <vector>
#include <json/json.h>

#include "world/phys/Vec3.h"

#if !defined(PUBLISH) && !defined(MCPE_HEADLESS)
#define ENABLE_JSON_LOGGING
#endif

#define TOKENPASTE(x, y) x ## y
#define TOKENPASTE2(x, y) TOKENPASTE(x, y)
#define JSON_SCOPE(name) JsonUtil::ScopeMarker TOKENPASTE2(marker, __LINE__)(name)

class Color;

namespace JsonUtil {
	struct ScopeMarker {
	private:
		bool mValid;
	public:
		ScopeMarker(const std::string &name);
		ScopeMarker(ScopeMarker &&other);

		ScopeMarker(const ScopeMarker &other) = delete;

		~ScopeMarker();
	};

	void debugMessage(const std::string &message);
	void logMessage(const std::string &message);
	void warningMessage(const std::string &message);
	void errorMessage(const std::string &message);

	template <typename FunctionType>
	void withMember(Json::Value &root, const std::string &memberName, FunctionType apply) {
		JSON_SCOPE(memberName);
		if (root.isNull()) {
			return;
		}
		if (root.isMember(memberName)) {
			apply(root[memberName]);
		}
	}

	template <typename FunctionType>
	void withMember(const Json::Value &root, const std::string &memberName, FunctionType apply) {
		JSON_SCOPE(memberName);
		if (root.isNull()) {
			return;
		}
		if (root.isMember(memberName)) {
			apply(root[memberName]);
		}
	}

	template <typename FunctionType>
	void forEach(const Json::Value &root, FunctionType apply) {
		if (root.isNull()) {
			return;
		}
		if (root.isArray()) {
			for (const auto &member : root) {
				apply(member);
			}
		}
		else if(!root.isNull()) {
			apply(root);
		}
	}

	template <typename FunctionType>
	void forEachMember(const Json::Value &root, FunctionType apply) {
		if (root.isNull()) {
			return;
		}
		if (root.isArray()) {
			for (const auto &member : root) {
				apply(member);
			}
		}
		else if (root.isObject()) {
			auto memberNames = root.getMemberNames();
			for (const auto &memberName : memberNames) {
				JSON_SCOPE(memberName);
				apply(root[memberName]);
			}
		}
	}

	Json::Value patchJson(Json::Value const &base, Json::Value const &patch);

	template <typename ValueType>
	bool parseValue(const Json::Value &value, ValueType &output);
	template <>
	bool parseValue<std::string>(const Json::Value &value, std::string &output);
	template <>
	bool parseValue<float>(const Json::Value &value, float &output);
	template <>
	bool parseValue<int>(const Json::Value &value, int &output);
	template <>
	bool parseValue<bool>(const Json::Value &value, bool &output);
	template <>
	bool parseValue<Vec3>(const Json::Value &value, Vec3 &output);
	template <>
	bool parseValue<Color>(const Json::Value &value, Color &output);
	template <>
	bool parseValue<std::vector<std::string>>(const Json::Value &value, std::vector<std::string> &output);
	template<>
	bool parseValue<std::set<std::string>>(const Json::Value &value, std::set<std::string> &output);

	template <typename ValueType>
	bool parseObjectKey(const Json::Value &value, const std::string &key, ValueType &output, bool required = false) {
		if (value.isNull()) {
			return false;
		}
		if (value.isObject()) {
			if (value.isMember(key)) {
				JSON_SCOPE(key);
				parseValue(value[key], output);
				return true;
			}
			else if (required) {
				errorMessage("Object does not have member " + key + ".");
				return false;
			}
			else {
				debugMessage("Object does not have optional member " + key + ".");
				return false;
			}
		}
		else if (required) {
			errorMessage("Trying to access member " + key + " of a non-Object.");
			return false;
		}
		else {
			debugMessage("Trying to access optional member " + key + " of a non-Object.");
			return false;
		}
	}

	template <typename OutputIterator>
	bool parseStrings(const Json::Value &root, OutputIterator it) {
		if (root.isNull()) {
			return false;
		}
		if (root.isString()) {
			*it = root.asString();
		}
		else if (root.isArray()) {
			for (const auto &element : root) {
				if (!element.isString()) {
					warningMessage("Found non-string while parsing string array.");
					return false;
				}
				else {
					*it++ = element.asString();
				}
			}
		}
		return true;
	}

	// Internal message handling

	enum class MessageSeverity : int {
		Debug,
		Log,
		Warning,
		Error
	};

	struct Message {
		std::string mScope;
		std::string mMessage;
		MessageSeverity mSeverity;
	};

	typedef std::vector<Message> MessageLog;

	class ParseHandler {
	public:
		static ParseHandler * get();

		void pushScope(const std::string &name);
		void popScope();

		void postMessage(const std::string &message, MessageSeverity sev);

		bool isParsing() const;

		bool hasMessages() const;
		MessageLog getMessages() const;

		void clear();

	private:
		std::string mParseName;
		std::vector<std::string> mScope;

		MessageLog mMessages;
	};
}
