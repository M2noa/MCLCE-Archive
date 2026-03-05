/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#include "Dungeons.h"

#include "world/level/storage/GameRules.h"
// #include "network/packet/GameRulesChangedPacket.h"
#include "nbt/CompoundTag.h"

const std::string GameRules::FALL_DAMAGE("falldamage");
const std::string GameRules::PVP("pvp");
const std::string GameRules::DROWNING_DAMAGE("drowningdamage");
const std::string GameRules::FIRE_DAMAGE("firedamage");

GameRule::GameRule()
	: mShouldSyncToClients(false)
	, mType(Type::Invalid) {
	mValue.intVal = 0;
}

GameRule::GameRule(bool value, bool shouldSync)
	: mShouldSyncToClients(shouldSync)
	, mType(Type::Bool) {
	mValue.boolVal = value;
}

GameRule::GameRule(int value, bool shouldSync)
	: mShouldSyncToClients(shouldSync)
	, mType(Type::Int) {
	mValue.intVal = value;
}

GameRule::GameRule(float value, bool shouldSync)
	: mShouldSyncToClients(shouldSync)
	, mType(Type::Float) {
	mValue.floatVal = value;
}

GameRule::Type GameRule::getType() const {
	return mType;
}

void GameRule::resetType(Type type) {
	mType = type;
	mValue.intVal = 0;
}

const std::unordered_map<std::string, GameRule>& GameRules::getRules() const {
	return mGameRules;
}

const GameRule* GameRules::getRule(const std::string& name) const {
	auto iter = mGameRules.find(name);
	if (iter == mGameRules.end()) {
		return nullptr;
	}
	return &iter->second;
}

bool GameRule::getBool() const {
	DEBUG_ASSERT(mType == Type::Bool, "Type Should be bool");
	return mValue.boolVal;
}

void GameRule::setBool(bool value) {
	DEBUG_ASSERT(mType == Type::Bool, "Type Should be bool");
	mValue.boolVal = value;
}

int GameRule::getInt() const {
	DEBUG_ASSERT(mType == Type::Int, "Type Should be Int");
	return mValue.intVal;
}

void GameRule::setInt(int value) {
	DEBUG_ASSERT(mType == Type::Int, "Type Should be Int");
	mValue.intVal = value;
}

float GameRule::getFloat() const {
	DEBUG_ASSERT(mType == Type::Float, "Type Should be Float");
	return mValue.floatVal;
}

void GameRule::setFloat(float value) {
	DEBUG_ASSERT(mType == Type::Float, "Type Should be Float");
	mValue.floatVal = value;
}

bool GameRule::shouldSyncToClients() const {
	return mShouldSyncToClients;
}

GameRules::GameRules()
	: mGameRules() {
	_addEduRules();
}

// Unique<GameRulesChangedPacket> GameRules::createAllGameRulesPacket() {
// 	Unique<GameRulesChangedPacket> packet = std::make_unique<GameRulesChangedPacket>();
// 	for (auto& rulePair : mGameRules) {
// 		auto& name = rulePair.first;
// 		auto& rule = rulePair.second;
// 		packet->mRuleData.addRule(name, rule);
// 	}
// 
// 	return packet;
// }

void GameRules::registerRule(const std::string& name, bool value, bool shouldSync) {
	mGameRules.emplace(name, GameRule(value, shouldSync));
}

void GameRules::registerRule(const std::string& name, int value, bool shouldSync) {
	mGameRules.emplace(name, GameRule(value, shouldSync));
}

void GameRules::registerRule(const std::string& name, float value, bool shouldSync) {
	mGameRules.emplace(name, GameRule(value, shouldSync));
}

// Unique<GameRulesChangedPacket> GameRules::_setRule(const std::string& name, GameRule::Value value, GameRule::Type type, bool returnPacket) {
// 	auto iter = mGameRules.find(name);
// 	if (iter == mGameRules.end()) {
// 		DEBUG_FAIL("Invalid rule set! register the rule first");
// 		return nullptr;
// 	}
// 	auto& rule = iter->second;
// 
// 	switch(type) {
// 	case GameRule::Type::Bool: rule.setBool(value.boolVal); break;
// 	case GameRule::Type::Float: rule.setFloat(value.floatVal); break;
// 	case GameRule::Type::Int: rule.setInt(value.intVal); break;
// 	case GameRule::Type::Invalid: DEBUG_FAIL("Invalid value set"); break;
// 	}
// 
// 	if (returnPacket) {
// 		return _createPacket(name, rule);
// 	}
// 	return nullptr;
// }
// 
// Unique<GameRulesChangedPacket> GameRules::setRule(const std::string& name, bool value, bool returnPacket) {
// 	GameRule::Value val;
// 	val.boolVal = value;
// 	return _setRule(name, val, GameRule::Type::Bool, returnPacket);
// }
// 
// Unique<GameRulesChangedPacket> GameRules::setRule(const std::string& name, int value, bool returnPacket) {
// 	GameRule::Value val;
// 	val.intVal = value;
// 	return _setRule(name, val, GameRule::Type::Int, returnPacket);
// }
// 
// Unique<GameRulesChangedPacket> GameRules::setRule(const std::string& name, float value, bool returnPacket) {
// 	GameRule::Value val;
// 	val.floatVal = value;
// 	return _setRule(name, val, GameRule::Type::Float, returnPacket);
// }

bool GameRules::getBool(const std::string& name) const {
	auto iter = mGameRules.find(name);
	if (iter == mGameRules.end()) {
		return false;
	}
	return iter->second.getBool();
}

int GameRules::getInt(const std::string& name) const {
	auto iter = mGameRules.find(name);
	if (iter == mGameRules.end()) {
		return 0;
	}
	return iter->second.getInt();
}

float GameRules::getFloat(const std::string& name) const {
	auto iter = mGameRules.find(name);
	if (iter == mGameRules.end()) {
		return 0;
	}
	return iter->second.getFloat();
}

bool GameRules::hasRule(const std::string& name) const {
	auto iter = mGameRules.find(name);
	return (iter != mGameRules.end());
}

void GameRules::setTagData(CompoundTag& tag) const {
	for (auto& pair : mGameRules) {
		auto& name = pair.first;
		auto& rule = pair.second;

		switch (rule.getType()) {
		case GameRule::Type::Bool:
			tag.putBoolean(name, rule.getBool());
			break;
		case GameRule::Type::Int:
			tag.putInt(name, rule.getInt());
			break;
		case GameRule::Type::Float:
			tag.putFloat(name, rule.getFloat());
			break;
		default:
			DEBUG_FAIL("Invalid rule type");
			break;
		}
	}
}

void GameRules::getTagData(const CompoundTag& tag) {
	for (auto& pair : mGameRules) {
		auto& name = pair.first;
		auto& rule = pair.second;
		if (tag.contains(name)) {
			switch (rule.getType()) {
			case GameRule::Type::Bool:
				rule.setBool(tag.getBoolean(name));
				break;
			case GameRule::Type::Int:
				rule.setInt(tag.getInt(name));
				break;
			case GameRule::Type::Float:
				rule.setFloat(tag.getFloat(name));
				break;
			default:
				DEBUG_FAIL("Invalid rule type");
				break;
			}
		}
	}
}

void GameRules::_addEduRules() {
	registerRule(FALL_DAMAGE, true, true);
	registerRule(PVP, true, true);
	registerRule(DROWNING_DAMAGE, true, true);
	registerRule(FIRE_DAMAGE, true, true);
}

// Unique<GameRulesChangedPacket> GameRules::_createPacket(const std::string& name, const GameRule& rule) {
// 	if (rule.shouldSyncToClients()) {
// 		Unique<GameRulesChangedPacket> packet = std::make_unique<GameRulesChangedPacket>();
// // 		packet->mRuleData.addRule(name, rule);
// 		return packet;
// 	}
// 	return nullptr;
// }
