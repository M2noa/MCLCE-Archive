#include "Dungeons.h"

#include "world/entity/EntityClassTree.h"
#include "world/entity/Entity.h"

EntityType EntityClassTree::getEntityTypeIdLegacy(EntityType entityId) {
	int typeInt = static_cast<int>(EntityType::TypeMask);
	return static_cast<EntityType>(static_cast<int>(entityId) & typeInt);
}

bool EntityClassTree::isMobLegacy(EntityType type) {
	int typeInt = static_cast<int>(type);
	return isMob(type) || (typeInt >= 10 && typeInt < 64);
}

EntityType EntityClassTree::getMobCategory(EntityType entityType) {
	if (EntityClassTree::isTypeInstanceOf(entityType, EntityType::Animal)) {
		return EntityType::Animal;
	} else if (EntityClassTree::isTypeInstanceOf(entityType, EntityType::Monster)) {
		return EntityType::Monster;
	} else if (EntityClassTree::isTypeInstanceOf(entityType, EntityType::WaterAnimal)) {
		return EntityType::WaterAnimal;
	} else if (EntityClassTree::isTypeInstanceOf(entityType, EntityType::Player)) {
		return EntityType::Player;
	} else if (EntityClassTree::isTypeInstanceOf(entityType, EntityType::Villager)) {
		return EntityType::Villager;
	} else if (EntityClassTree::isTypeInstanceOf(entityType, EntityType::Ambient)) {
		return EntityType::Ambient;
	} else {
		return EntityType::ItemEntity;
	}
}

bool EntityClassTree::isTypeInstanceOf(EntityType type, EntityType testFor) {
	// Okay, since this function has been written incorrectly multiple times here, I'm going to describe how it works
	// testFor can be one of two cases:
	// 1. A generic category of entity such as PathfinderMob or Monster
	// 2. A specific type of entity such as Ocelot or ZombieVillager
	// For case 1, the lower 8 bits (represented by EntityType::TypeMask) will be zero. That is what is in testIndex in
	// below code
	// For case 2, the lower 8 bits will be non-zero
	// In case 1, what we need to check is that entity we are testing against isn't missing any of the bits in the top
	// 24 bits of testFor
	// This means that if we are testing for PathfinderMob, then anything that has the PathfinderMob bits set (ie. Mob
	// AND PathfinderMob) will work. They can have more bits set, but not less
	// In case 2, we just check if he lower 8 bits are the same.
	// For an interesting case, we have ZombieMonster (a category) and we have Zombie (a Zombie Monster), ZombieVillager
	// (a ZombieMonster) and PigZombie (an UndeadMob). Even though PigZombies
	// can technically be cast to a Zombie, the game doesn't treat them as a zombie, thus the inconsistency here. If you
	// want an actual honest to goodness Zombie, you pass Zombie, if you want
	// a zombie villager, you pass ZombieVillager, if you want either then you pass ZombieMonster. If you also want
	// PigZombies you would pass in UndeadMob, but then you would have to filter out Skeletons
	// yourself. Whew.

	// Check by Category OR by Type Id (if type bits exist then check by type id)
	int testIndex = static_cast<int>(testFor) & static_cast<int>(EntityType::TypeMask);
	int testCategory = static_cast<int>(testFor) & ~static_cast<int>(EntityType::TypeMask);
	int typeCategory = static_cast<int>(type) & ~static_cast<int>(EntityType::TypeMask);

	if ( testCategory != 0 && testIndex == 0) {
		return (testCategory & typeCategory) == testCategory;
	} else{
		return (static_cast<int>(type)&static_cast<int>(EntityType::TypeMask)) == testIndex;
	}
}

bool EntityClassTree::isInstanceOf(const Entity& inEntity, EntityType type) {
	// TODO: rherlitz
	//return EntityClassTree::isTypeInstanceOf(inEntity.getEntityTypeId(), type);
	return true;
}

// D5: contains is the group, could be a separate enum
bool EntityClassTree::isOfType(EntityType type, EntityType contains) {
	if(type == contains) {
		return true;
	}

	if ((static_cast<int>(contains) & static_cast<int>(EntityType::TypeMask)) == (static_cast<int>(type) & static_cast<int>(EntityType::TypeMask))) {
		return true;
	}

	return false;
}

//bool EntityClassTree::isItemEntity(const Entity& inEntity) {
//	return isOfType(inEntity.getEntityTypeId(), EntityType::ItemEntity);
//}
//
bool EntityClassTree::isHangingEntity(const Entity& inEntity) {
	// TODO: rherlitz
	//return isOfType(inEntity.getEntityTypeId(), EntityType::Painting);
	return false;
}
//
bool EntityClassTree::isMob(EntityType type) {
	return isTypeInstanceOf(type, EntityType::Mob);
}
//
//bool EntityClassTree::isMob(const Entity& inEntity) {
//	return isMob(inEntity.getEntityTypeId());
//}
//
//bool EntityClassTree::isPlayer(const Entity& inEntity) {
//	return inEntity.getEntityTypeId() == EntityType::Player;
//}
//
//bool EntityClassTree::isMinecart(const Entity& inEntity) {
//	return isTypeInstanceOf(inEntity.getEntityTypeId(), EntityType::Minecart);
//}
