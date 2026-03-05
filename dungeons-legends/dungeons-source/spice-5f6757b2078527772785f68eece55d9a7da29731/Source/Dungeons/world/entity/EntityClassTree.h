/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/entity/EntityTypes.h"

class Entity;

class EntityClassTree {
public:
	// Legacy Type
	static EntityType getEntityTypeIdLegacy(EntityType entityId);
	static bool isMobLegacy(EntityType type);

	// RTTI
	static bool isTypeInstanceOf(EntityType type, EntityType testFor);
	static bool isInstanceOf(const Entity& inEntity, const EntityType type);
	static bool isOfType(EntityType type, EntityType contains);

	// Shortcuts
//	static bool isItemEntity(const Entity& inEntity);
	static bool isHangingEntity(const Entity& inEntity);
	static bool isMob(EntityType type);
//	static bool isMob(const Entity& inEntity);
//	static bool isPlayer(const Entity& inEntity);
//	static bool isMinecart(const Entity& inEntity);

	// Spawning categories
	static EntityType getMobCategory(EntityType entityType);
};
