#pragma once
#include "MaterialColor.h"

class ChunkRebuildData;

class Material
{
	friend class ChunkRebuildData;
public:
	static Material *air;
	static Material *grass;
	static Material *dirt;
	static Material *wood;
	static Material *stone;
	static Material *metal;
	static Material *heavyMetal;
	static Material *water;
	static Material *lava;
	static Material *leaves;
	static Material *plant;
	static Material *replaceable_plant;
	static Material *sponge;
	static Material *cloth;
	static Material *fire;
	static Material *sand;
	static Material *decoration;
	static Material *clothDecoration;
	static Material *glass;
	static Material *buildable_glass;
	static Material *explosive;
	static Material *coral;
	static Material *ice;
	static Material *topSnow;
	static Material *snow;
	static Material *cactus;
	static Material *clay;
	static Material *vegetable;
	static Material *egg;
	static Material *portal;
	static Material *cake;
	static Material *web;
	static Material *piston;

	static const int PUSH_NORMAL = 0;
	static const int PUSH_DESTROY = 1;
	static const int PUSH_BLOCK = 2;		// not pushable

	static void staticCtor();

private:
    bool _flammable, _replaceable, _neverBuildable;

public:
	MaterialColor *color;
private:
	bool _isAlwaysDestroyable;
	int pushReaction;
	bool destroyedByHand;
public:

    Material(MaterialColor *color);
    virtual bool isLiquid() ;
    virtual bool letsWaterThrough();
    virtual bool isSolid();
    virtual bool blocksLight();
    virtual bool blocksMotion();

private:
    virtual Material *neverBuildable();
protected:
	virtual Material *notAlwaysDestroyable();
    virtual Material *flammable();

public:
	virtual bool isFlammable();
	virtual Material *replaceable();
	virtual bool isReplaceable();
	virtual bool isSolidBlocking();
	virtual bool isAlwaysDestroyable();
	virtual int getPushReaction();
	
protected:
	Material *makeDestroyedByHand();

public:
	bool isDestroyedByHand();

protected:
	Material *destroyOnPush();
	Material *notPushable();
};

