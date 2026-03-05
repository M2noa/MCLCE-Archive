/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#pragma once

#include "MaterialType.h"
#include "Core/Math/Color.h"

class Material {
public:

	enum class Settings {
		Normal,
		Gas,
		Liquid,
		Decoration,
		Portal
	};

	static const Material& getMaterial(MaterialType type);

	Material(MaterialType type, Settings settings = Settings::Normal, float translucency = 0.0f);

	Material(const Material&) = delete;
	Material& operator=(const Material&) = delete;

	static void addMaterial(std::unique_ptr<Material> mat);

	bool operator==(const Material& rhs) const;
	bool operator!=(const Material& rhs) const;

	static void initMaterials();
	static void teardownMaterials();

	~Material();
	
	bool isType(MaterialType surface) const;

	bool isSolidBlocking() const;
	
	bool isFlammable() const;
	bool isNeverBuildable() const;
	bool isAlwaysDestroyable() const;
	bool isReplaceable() const;
	bool isLiquid() const;
	float getTranslucency() const;
	bool getBlocksMotion() const;
	bool isSolid() const;
	bool isSuperHot() const;

	Color getColor() const;

protected:

	Material& _setReplaceable();
	Material& _setFlammable();
	Material& _setNotAlwaysDestroyable();
	Material& _setNeverBuildable();
	Material& _setNotBlockingMotion();
	Material& _setNotSolid();
	Material& _setSuperHot();

private:

	static void _setupSurfaceMaterials();

	void _setMapColor(const Color& color);

	static bool mInitialized;
	
	MaterialType mType;
	bool mFlammable;
	bool mNeverBuildable;
	bool mAlwaysDestroyable;
	bool mReplaceable;
	bool mLiquid;
	float mTranslucency;
	bool mBlocksMotion;
	bool mSolid;
	bool mSuperHot = false;
	Color mMaterialColor;

	static std::vector<std::unique_ptr<Material> > mMaterials;
};
