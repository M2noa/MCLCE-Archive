/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/

#pragma once

enum class EntityType;

struct EntityDefinitionIdentifier {
	std::string mNamespace;
	std::string mIdentifier;
	std::string mInitEvent;

	static const std::string NAMESPACE_SEPARATOR;
	static const std::string EVENT_BEGIN;
	static const std::string EVENT_END;

	EntityDefinitionIdentifier();
	EntityDefinitionIdentifier(const std::string &nameSpace, const std::string &identifier, const std::string &initEvent = std::string());
	EntityDefinitionIdentifier(const std::string &fullName);
	EntityDefinitionIdentifier(const char *fullName);
	EntityDefinitionIdentifier(EntityType type);
	EntityDefinitionIdentifier(const EntityDefinitionIdentifier &other);
	EntityDefinitionIdentifier(EntityDefinitionIdentifier &&other);

	EntityDefinitionIdentifier & operator=(const EntityDefinitionIdentifier &other);
	bool operator==(const EntityDefinitionIdentifier &other);

	inline std::string getCanonicalName() const {
		return mNamespace + NAMESPACE_SEPARATOR + mIdentifier;
	}

	inline std::string getFullName() const {
		return mNamespace + NAMESPACE_SEPARATOR + mIdentifier + EVENT_BEGIN + mInitEvent + EVENT_END;
	}
};
