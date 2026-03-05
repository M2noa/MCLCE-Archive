/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#pragma once

#include "common_header.h"
#include "CommonTypes.h"

// class GameRulesChangedPacket;
class CompoundTag;

class GameRule {
public:
	enum class Type : uint8_t {
		Invalid = 0,
		Bool,
		Int,
		Float
	};

	union Value {
		bool boolVal;
		int intVal;
		float floatVal;
	};

	GameRule();
	GameRule(bool value, bool shouldSync);
	GameRule(int value, bool shouldSync);
	GameRule(float value, bool shouldSync);

	Type getType() const;
	void resetType(Type type);

	bool getBool() const;
	void setBool(bool value);

	int getInt() const;
	void setInt(int value);

	float getFloat() const;
	void setFloat(float value);
	
	bool shouldSyncToClients() const;

private:

	bool mShouldSyncToClients;
	Type mType;
	Value mValue;
};

class GameRules {
public:

	GameRules();

// 	Unique<GameRulesChangedPacket> createAllGameRulesPacket();

	// thought about using templates but it seemed overkill
	void registerRule(const std::string& name, bool value, bool shouldSync);
	void registerRule(const std::string& name, int value, bool shouldSync);
	void registerRule(const std::string& name, float value, bool shouldSync);

// 	Unique<GameRulesChangedPacket> setRule(const std::string& name, bool value, bool returnPacket = true);
// 	Unique<GameRulesChangedPacket> setRule(const std::string& name, int value, bool returnPacket = true);
// 	Unique<GameRulesChangedPacket> setRule(const std::string& name, float value, bool returnPacket = true);

	const std::unordered_map<std::string, GameRule>& getRules() const;
	const GameRule* getRule(const std::string& name) const;

	bool getBool(const std::string& name) const;
	int getInt(const std::string& name) const;
	float getFloat(const std::string& name) const;

	bool hasRule(const std::string& name) const;

	void setTagData(CompoundTag& tag) const;
	void getTagData(const CompoundTag& tag);

	static const std::string FALL_DAMAGE;
	static const std::string PVP;
	static const std::string DROWNING_DAMAGE;
	static const std::string FIRE_DAMAGE;

private:
	void _addEduRules();

// 	Unique<GameRulesChangedPacket> _setRule(const std::string& name, GameRule::Value value, GameRule::Type type, bool returnPacket);

// 	Unique<GameRulesChangedPacket> _createPacket(const std::string& name, const GameRule& rule);

	std::unordered_map<std::string, GameRule> mGameRules;
};
