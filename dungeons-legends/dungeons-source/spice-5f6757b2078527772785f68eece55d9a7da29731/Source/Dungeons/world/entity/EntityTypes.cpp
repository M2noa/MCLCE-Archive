/********************************************************
*   (c) Mojang. All rights reserved                       *
*   (c) Microsoft. All rights reserved.                   *
*********************************************************/
#include "Dungeons.h"

#include "world/entity/EntityTypes.h"
#include "world/entity/EntityClassTree.h"
#include "world/entity/EntityDefinitionIdentifier.h"

namespace std {
	template<>
	struct hash<EntityType> {
		std::size_t operator()(EntityType const& key) const {
			return (std::size_t)key;
		}
	};
}

std::unordered_map<EntityType, EntityMapping> ENTITY_TYPE_MAP({
	// Animals
	{ EntityType::Chicken, EntityMapping("chicken") },
	{ EntityType::Cow, EntityMapping("cow") },
	{ EntityType::Pig, EntityMapping("pig") },
	{ EntityType::Sheep, EntityMapping("sheep") },
	{ EntityType::Wolf, EntityMapping("wolf") },
	{ EntityType::Villager, EntityMapping("villager") },
	{ EntityType::MushroomCow, EntityMapping("mooshroom", "mushroomcow") },
	{ EntityType::Squid, EntityMapping("squid") },
	{ EntityType::Rabbit, EntityMapping("rabbit") },
	{ EntityType::Bat, EntityMapping("bat") },
	{ EntityType::IronGolem, EntityMapping("iron_golem", "irongolem") },
	{ EntityType::SnowGolem, EntityMapping("snow_golem", "snowgolem") },
	{ EntityType::Ocelot, EntityMapping("ocelot") },
	{ EntityType::Horse, EntityMapping("horse") },
	{ EntityType::PolarBear, EntityMapping("polar_bear", "polarbear") },

	{ EntityType::Donkey, EntityMapping("donkey") },
	{ EntityType::Mule, EntityMapping("mule") },
	{ EntityType::SkeletonHorse, EntityMapping("skeleton_horse", "skeletonhorse") },
	{ EntityType::ZombieHorse, EntityMapping("zombie_horse", "zombiehorse") },

	//	Hostile Mobs
	{ EntityType::Zombie, EntityMapping("zombie") },
	{ EntityType::Creeper, EntityMapping("creeper") },
	{ EntityType::Skeleton, EntityMapping("skeleton") },
	{ EntityType::Spider, EntityMapping("spider") },
	{ EntityType::PigZombie, EntityMapping("zombie_pigman", "pig_zombie") },
	{ EntityType::Slime, EntityMapping("slime") },
	{ EntityType::EnderMan, EntityMapping("enderman") },
	{ EntityType::Silverfish, EntityMapping("silverfish") },
	{ EntityType::CaveSpider, EntityMapping("cave_spider", "cavespider") },
	{ EntityType::Ghast, EntityMapping("ghast") },
	{ EntityType::LavaSlime, EntityMapping("magma_cube", "magmacube") },
	{ EntityType::Blaze, EntityMapping("blaze") },
	{ EntityType::ZombieVillager, EntityMapping("zombie_villager", "zombievillager") },
	{ EntityType::Witch, EntityMapping("witch") },

	{ EntityType::Stray, EntityMapping("stray", "skeleton.stray") },
	{ EntityType::Husk, EntityMapping("husk") },
	{ EntityType::WitherSkeleton, EntityMapping("wither_skeleton", "skeleton.wither") },
	{ EntityType::Guardian, EntityMapping("guardian") },
	{ EntityType::ElderGuardian, EntityMapping("elder_guardian", "guardian.elder") },

	{ EntityType::WitherBoss, EntityMapping("wither", "wither.boss") },
	{ EntityType::Dragon, EntityMapping("ender_dragon", "dragon") },
	{ EntityType::Shulker, EntityMapping("shulker") },
	{ EntityType::Endermite, EntityMapping("endermite") },

	{ EntityType::Player, EntityMapping("player") },

	//	Non-Mob
	{ EntityType::MinecartRideable, EntityMapping("minecart") },
	{ EntityType::MinecartHopper, EntityMapping("hopper_minecart", "minecarthopper") },
	{ EntityType::MinecartTNT, EntityMapping("tnt_minecart", "minecarttnt") },
	{ EntityType::MinecartChest, EntityMapping("chest_minecart", "minecartchest") },

	{ EntityType::ItemEntity, EntityMapping("item") },
	{ EntityType::PrimedTnt, EntityMapping("tnt", "primedtnt") },
	{ EntityType::FallingBlock, EntityMapping("falling_block", "fallingblock") },
	{ EntityType::MovingBlock, EntityMapping("moving_block", "movingblock") },
	{ EntityType::ExperiencePotion, EntityMapping("xp_bottle", "potion.experience") },
	{ EntityType::Experience, EntityMapping("xp_orb", "xporb") },
	{ EntityType::EyeOfEnder, EntityMapping("eye_of_ender_signal", "eyeofender") },
	{ EntityType::EnderCrystal, EntityMapping("ender_crystal", "endercrystal") },
	{ EntityType::ShulkerBullet, EntityMapping("shulker_bullet", "shulkerbullet") },
	{ EntityType::FishingHook, EntityMapping("fishing_hook", "fishinghook") },

	{ EntityType::DragonFireball, EntityMapping("dragon_fireball", "fireball.dragon") },
	{ EntityType::Arrow, EntityMapping("arrow", "arrow.skeleton") },
	{ EntityType::Snowball, EntityMapping("snowball") },
	{ EntityType::ThrownEgg, EntityMapping("egg", "thrownegg") },
	{ EntityType::Painting, EntityMapping("painting") },
	{ EntityType::LargeFireball, EntityMapping("fireball", "fireball.large") },
	{ EntityType::ThrownPotion, EntityMapping("splash_potion", "thrownpotion") },
	{ EntityType::Enderpearl, EntityMapping("ender_pearl", "thrownenderpearl") },
	{ EntityType::LeashKnot, EntityMapping("leash_knot", "leashknot") },
	{ EntityType::WitherSkull, EntityMapping("wither_skull", "wither.skull") },
	{ EntityType::WitherSkullDangerous, EntityMapping("wither_skull_dangerous", "wither.skull.dangerous") },

	{ EntityType::BoatRideable, EntityMapping("boat")},
	{ EntityType::LightningBolt, EntityMapping("lightning_bolt", "lightningbolt") },
	{ EntityType::SmallFireball, EntityMapping("small_fireball", "fireball.small") },
	{ EntityType::AreaEffectCloud, EntityMapping("area_effect_cloud", "areaeffectcloud") },

	{ EntityType::LingeringPotion, EntityMapping("lingering_potion", "lingeringpotion") },

#ifdef MCPE_EDU
	{ EntityType::Chalkboard, EntityMapping("chalkboard") },
	{ EntityType::Npc, EntityMapping("npc") },
	{ EntityType::TripodCamera, EntityMapping("tripod_camera", "tripodcamera") }
#endif
});

std::string EntityTypeToString(EntityType entityType, EntityTypeNamespaceRules namespaceRule) {
	if (entityType != EntityType::Undefined) {
		auto i = ENTITY_TYPE_MAP.find(entityType);
		if (i != ENTITY_TYPE_MAP.end()) {
			return i->second.getMappingName(namespaceRule);
		}
	}

	//	If we've made it here than we can assume we didn't find an entity with this type so let's check through our custom entities.
	//	TODO: Add code for finding custom entities created from EntityDefinitions here.

	return "unknown";
}

std::string EntityTypeIdWithoutCategories(EntityType entityType, EntityTypeNamespaceRules namespaceRule) {
	// TODO: rherlitz

	//if (entityType != EntityType::Undefined) {
	//	for (auto&& mapping : ENTITY_TYPE_MAP) {
	//		if (EntityClassTree::isOfType(mapping.first, entityType)) {
	//			return mapping.second.getMappingName(namespaceRule);
	//		}
	//	}
	//}

	//	If we've made it here than we can assume we didn't find an entity with this type so let's check through our custom entities.
	//	TODO: Add code for finding custom entities created from EntityDefinitions here.

	return "unknown";
}

EntityType EntityTypeFromString(const std::string& str) {
	if (!str.empty() && str != "unknown") {
		//	First thing we want to do is try to strip any namespace from this entity's name to make it easier to find what entity we're looking for.
		const std::size_t namespacePos = str.find(EntityDefinitionIdentifier::NAMESPACE_SEPARATOR);
		const std::string entityNamespace = str.substr(0, namespacePos);
		const std::string lowerName = Util::toLower(str.substr(namespacePos + 1));

		//	If there is no namespace or it's minecraft we want to test against vanilla entities.
		if (namespacePos == std::string::npos || entityNamespace == "minecraft") {
			//	Loop through our ENTITY_TYPE_MAP and see if this entity name is a thing.
			for (auto&& mapping : ENTITY_TYPE_MAP) {
				if (mapping.second.mPrimaryName == lowerName
					|| mapping.second.mAlternateName == lowerName) {
					return mapping.first;
				}
			}
		}

		//	If we've made it here than we can assume we didn't find an entity with this name so let's check through our custom entities.
		//	TODO: Add code for finding custom entities created from EntityDefinitions here.
	}

	// Unknown type
	return EntityType::Undefined;
}

std::string EntityTypeResolveAlias(const std::string& fromString, EntityTypeNamespaceRules namespaceRule) {
	return EntityTypeToString(EntityTypeFromString(fromString), namespaceRule);
}
