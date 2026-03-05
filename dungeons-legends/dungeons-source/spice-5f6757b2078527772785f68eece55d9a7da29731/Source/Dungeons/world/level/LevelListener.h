/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/

#pragma once

//#include "world/entity/ParticleType.h"
#include "world/level/block/LevelEvent.h"
#include "world/level/BlockSourceListener.h"

class Entity;
//class Mob;
//class BlockEntity;
class Particle;
class Player;
class LevelChunk;
class Vec3;
enum class EntityLocation;
class TextureData;
enum class EntityType : int;

class LevelListener :
	public BlockSourceListener {
public:

	virtual ~LevelListener() {
	}

	virtual void allChanged() {
	}

	//virtual Particle* addParticle(ParticleType name, const Vec3& pos, const Vec3& dir, int data) {
	//	UNUSED_PARAMETER(name,pos,dir,data);
	//	return nullptr;
	//}

	virtual void playMusic(const std::string& name, const Vec3& pos, float songOffset) {
		UNUSED_PARAMETER(name, pos, songOffset);
	}

	virtual void playStreamingMusic(const std::string& name, int x, int y, int z) {
		UNUSED_PARAMETER(name, x, y, z);
	}

	virtual void onEntityAdded(Entity& entity) {
		UNUSED_PARAMETER(entity);
	}

	virtual void onEntityRemoved(Entity& entity) {
		UNUSED_PARAMETER(entity);
	}

	virtual void onNewChunkFor(Player& p, LevelChunk& lc) {
		UNUSED_PARAMETER(p, lc);
	}

	virtual void onChunkLoaded(LevelChunk& lc) {
		UNUSED_PARAMETER(lc);
	}

	virtual void onChunkUnloaded(LevelChunk& lc) {
		UNUSED_PARAMETER(lc);
	}

	virtual void levelEvent(LevelEvent type, const Vec3& pos, int data) {
		UNUSED_PARAMETER(type, pos, data);
	}

	virtual void levelSoundEvent(LevelSoundEvent type, const Vec3& pos, int data, EntityType entityType, bool isBabyMob, bool isGlobal) {
		UNUSED_PARAMETER(type,pos,data,entityType,isBabyMob,isGlobal);
	}

	virtual void takePicture(TextureData & outImage, Entity* camera, Entity* target, bool applySquareFrame = false, const std::string & filename = "") {
		UNUSED_PARAMETER(outImage,camera,target,applySquareFrame,filename);
	}
	
	virtual void playerListChanged() {
	}
};
