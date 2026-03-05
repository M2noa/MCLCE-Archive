/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/

#include "Dungeons.h"

#include "CommonTypes.h"
#include "EntityDefinitionIdentifier.h"
//#include "EntityFactory.h"
#include "world/entity/EntityTypes.h"

const std::string EntityDefinitionIdentifier::NAMESPACE_SEPARATOR = ":";
const std::string EntityDefinitionIdentifier::EVENT_BEGIN = "<";
const std::string EntityDefinitionIdentifier::EVENT_END = ">";

EntityDefinitionIdentifier::EntityDefinitionIdentifier() { }

EntityDefinitionIdentifier::EntityDefinitionIdentifier(const std::string &nameSpace, const std::string &identifier, const std::string &initEvent)
	: mNamespace(nameSpace)
	, mIdentifier(identifier)
	, mInitEvent(initEvent) {
}

EntityDefinitionIdentifier::EntityDefinitionIdentifier(const EntityDefinitionIdentifier &other)
	: mNamespace(other.mNamespace)
	, mIdentifier(other.mIdentifier)
	, mInitEvent(other.mInitEvent) {
}

EntityDefinitionIdentifier::EntityDefinitionIdentifier(EntityDefinitionIdentifier &&other)
	: mNamespace(std::move(other.mNamespace))
	, mIdentifier(std::move(other.mIdentifier))
	, mInitEvent(std::move(other.mInitEvent)) {
}

static void _extractIdentifier(const std::string &name, EntityDefinitionIdentifier &id) {
	const std::size_t namespacePos = name.find(EntityDefinitionIdentifier::NAMESPACE_SEPARATOR);
	const std::size_t eventBeginPos = name.find(EntityDefinitionIdentifier::EVENT_BEGIN);
	const std::size_t eventEndPos = name.find(EntityDefinitionIdentifier::EVENT_END);
	const std::size_t notFound = std::string::npos;

	if (namespacePos != notFound) {
		id.mNamespace = name.substr(0, namespacePos);
	}
	else {
		id.mNamespace = "ERROR";
		DEBUG_FAIL(("Namespace not found in entity: " + name).c_str());
	}

	if (eventBeginPos != notFound && eventEndPos != notFound) {
		id.mIdentifier = name.substr(namespacePos + 1, eventBeginPos - namespacePos - 1);
		id.mInitEvent = name.substr(eventBeginPos + 1, eventEndPos - eventBeginPos - 1);
	}
	else {
		id.mIdentifier = name.substr(namespacePos + 1);
	}

}

EntityDefinitionIdentifier::EntityDefinitionIdentifier(const char *fullName) {
	_extractIdentifier(std::string(fullName), *this);
}

EntityDefinitionIdentifier::EntityDefinitionIdentifier(const std::string &fullName) {
	_extractIdentifier(fullName, *this);
}

EntityDefinitionIdentifier::EntityDefinitionIdentifier(EntityType type)
	: mNamespace("minecraft")
	, mIdentifier(EntityTypeIdWithoutCategories(type))
	, mInitEvent() {  }

EntityDefinitionIdentifier & EntityDefinitionIdentifier::operator =(const EntityDefinitionIdentifier &other) {
	mNamespace = other.mNamespace;
	mIdentifier = other.mIdentifier;
	mInitEvent = other.mInitEvent;

	return *this;
}

bool EntityDefinitionIdentifier::operator==(const EntityDefinitionIdentifier &other) {
	return getFullName() == other.getFullName();
}
