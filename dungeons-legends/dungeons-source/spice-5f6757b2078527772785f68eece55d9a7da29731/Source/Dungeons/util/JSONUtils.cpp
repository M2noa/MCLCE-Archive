/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/

#include "Dungeons.h"

#include "util/JSONUtils.h"
#include "Core/Math/Color.h"

namespace JsonUtil {
	static ParseHandler _globalHandler;

	ParseHandler * ParseHandler::get() {
		return &_globalHandler;
	}

	void ParseHandler::pushScope(const std::string &name) {
		mScope.push_back(name);
	}

	void ParseHandler::popScope() {
		mScope.pop_back();
	}

	void ParseHandler::postMessage(const std::string &message, MessageSeverity sev) {
		Message m;
		m.mMessage = message;
		m.mSeverity = sev;
		std::stringstream scopeStream;
		for (size_t i = 0; i < mScope.size(); ++i) {
			for (size_t j = 0; j < i; ++j) {
				scopeStream << "  ";
			}
			scopeStream << mScope[i] << Util::NEW_LINE;
		}
		m.mScope = scopeStream.str();
		mMessages.emplace_back(std::move(m));
	}

	bool ParseHandler::isParsing() const {
		return !mScope.empty();
	}

	bool ParseHandler::hasMessages() const {
		return !mMessages.empty();
	}

	MessageLog ParseHandler::getMessages() const {
		return mMessages;
	}

	void ParseHandler::clear() {
		mMessages.clear();
	}

	ScopeMarker::ScopeMarker(const std::string &name) : mValid(true) {
		ParseHandler::get()->pushScope(name);
	}

	ScopeMarker::ScopeMarker(ScopeMarker &&other) : mValid(other.mValid) {
		other.mValid = false;
	}

	ScopeMarker::~ScopeMarker() {
		if (mValid) {
			ParseHandler::get()->popScope();
		}
	}

	void debugMessage(const std::string &message) {
		ParseHandler::get()->postMessage(message, MessageSeverity::Debug);
	}
	void logMessage(const std::string &message) {
		ParseHandler::get()->postMessage(message, MessageSeverity::Log);
	}
	void warningMessage(const std::string &message) {
		ParseHandler::get()->postMessage(message, MessageSeverity::Warning);
	}
	void errorMessage(const std::string &message) {
		ParseHandler::get()->postMessage(message, MessageSeverity::Error);
	}

	Json::Value patchJson(Json::Value const &base, Json::Value const &patch) {
		if (base.isObject() && patch.isObject()) {
			Json::Value result = patch;
			auto members = base.getMemberNames();
			for (auto &member : members) {
				if (result.isMember(member)) {
					result[member] = patchJson(base[member], patch[member]);
				}
				else {
					result[member] = base[member];
				}
			}
			return result;
		}
		else if (base.isArray() && patch.isArray()) {
			Json::Value listResult = patch;
			while (listResult.size() < base.size()) {
				listResult.append(base[listResult.size()]);
			}
			return listResult;
		}
		else if (!patch.isNull()) {
			return patch;
		}
		else {
			return base;
		}
	}

	template <>
	bool parseValue<std::string>(const Json::Value &value, std::string &output) {
		if (value.isString()) {
			output = value.asString();
			return true;
		}
		else {
			warningMessage("Expected String.");
			return false;
		}
	}

	template <>
	bool parseValue<float>(const Json::Value &value, float &output) {
		if (value.isNumeric()) {
			output = value.asFloat();
			return true;
		}
		else {
			warningMessage("Expected Number.");
			return false;
		}
	}

	template <>
	bool parseValue<bool>(const Json::Value &value, bool &output) {
		if (value.isBool()) {
			output = value.asBool();
			return true;
		}
		else {
			warningMessage("Expected Boolean.");
			return false;
		}
	}

	template <>
	bool parseValue<int>(const Json::Value &value, int &output) {
		if (value.isIntegral()) {
			output = value.asInt();
			return true;
		}
		else {
			warningMessage("Expected Integral Number.");
			return false;
		}
	}

	template <>
	bool parseValue<Vec3>(const Json::Value &value, Vec3 &output) {
		if (value.isArray() && value.size() == 3) {
			{
				JSON_SCOPE("x");
				parseValue(value[0], output.x);
			}
			{
				JSON_SCOPE("y");
				parseValue(value[1], output.y);
			}
			{
				JSON_SCOPE("z");
				parseValue(value[2], output.z);
			}
			return true;
		}
		else {
			warningMessage("Expected [ x, y, z ].");
			return false;
		}
	}

	template <>
	bool parseValue<Color>(const Json::Value &value, Color &output) {
		int r = 255, g = 255, b = 255, a = 255;
		if (value.isArray() && value.size() >= 3) {
			{
				JSON_SCOPE("r");
				parseValue(value[0], r);
			}
			{
				JSON_SCOPE("g");
				parseValue(value[1], g);
			}
			{
				JSON_SCOPE("b");
				parseValue(value[2], b);
			}

			if (value.size() >= 4) {
				JSON_SCOPE("a");
				parseValue(value[3], a);
			}

			output = Color::from255Range(Math::clamp(r, 0, 255), Math::clamp(g, 0, 255), Math::clamp(b, 0, 255), Math::clamp(a, 0, 255));
			return true;
		}
		else if (value.isString()) {
			output = Color::fromARGB(strtoul(value.asString().substr(1).c_str(), nullptr, 16));
			return true;
		}
		else {
			warningMessage("Expected [ r, g, b ] or [r, g, b, a] or \"#RRGGBB\".");
			return false;
		}
	}

	template <>
	bool parseValue<std::vector<std::string>>(const Json::Value &value, std::vector<std::string> &output) {
		return parseStrings(value, std::back_inserter<std::vector<std::string>>(output));
	}

	template<>
	bool parseValue<std::set<std::string>>(const Json::Value &value, std::set<std::string> &output) {
		return parseStrings(value, std::inserter(output, output.begin()));
	}
}
