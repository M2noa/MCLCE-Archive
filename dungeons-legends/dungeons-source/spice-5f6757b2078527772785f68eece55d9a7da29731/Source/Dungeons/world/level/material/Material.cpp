#include "Dungeons.h"

#include "CommonTypes.h"

#include "world/level/material/Material.h"

std::vector<std::unique_ptr<Material> > Material::mMaterials;
bool Material::mInitialized = false;

template<class ... Args>
Material& registerMaterial(Args&& ... args) {
	auto mat = make_unique<Material>(std::forward<Args>(args) ...);
	auto& ref = *mat;
	Material::addMaterial(std::move(mat));
	return ref;
}

bool Material::operator==(const Material& rhs) const {
	return this == &rhs;
}

bool Material::operator!=(const Material& rhs) const {
	return !operator==(rhs);
}

/*static*/
void Material::initMaterials() {
	mInitialized = true;
	_setupSurfaceMaterials();
}

/*static*/
void Material::teardownMaterials() {
	mMaterials.clear();
}

Material::Material(MaterialType type, Settings settings, float translucency)
	: mType(type)
	, mBlocksMotion(true)
	, mFlammable(false)
	, mNeverBuildable(false)
	, mAlwaysDestroyable(true)
	, mReplaceable(false)
	, mLiquid(false)
	, mTranslucency(translucency)
	, mSolid(true)
	, mMaterialColor(Color::NIL) {

	switch(settings) {
	case Settings::Gas: {
		mSolid = false;
		mBlocksMotion = false;
		_setReplaceable();
	}
	break;
	case Settings::Liquid: {
		mSolid = false;
		mLiquid = true;
		mBlocksMotion = false;
		_setReplaceable();
	}
	break;
	case Settings::Portal:
	case Settings::Decoration: {
		mSolid = false;
		mBlocksMotion = false;
	}
	break;
	default:
		break;
	}
}

Material::~Material() {

}

bool Material::isType(MaterialType surface) const {
	
	return surface == MaterialType::Any || mType == surface;
}

bool Material::isSolidBlocking() const {
	if (mNeverBuildable) {
		return false;
	}
	return getBlocksMotion();
}

bool Material::isFlammable() const {
	return mFlammable;
}

bool Material::isNeverBuildable() const {
	return mNeverBuildable;
}

bool Material::isAlwaysDestroyable() const {
	// these materials will always drop resources when destroyed,
	// regardless of player's equipment
	return mAlwaysDestroyable;
}

bool Material::isReplaceable() const {
	return mReplaceable;
}

bool Material::isLiquid() const {
	return mLiquid;
}

float Material::getTranslucency() const {
	return mTranslucency;
}

bool Material::getBlocksMotion() const {
	return mBlocksMotion;
}

bool Material::isSolid() const {
	return mSolid;
}

Color Material::getColor() const {
	return mMaterialColor;
}

bool Material::isSuperHot() const {
	return mSuperHot;
}

const Material& Material::getMaterial(MaterialType surface) {
	DEBUG_ASSERT(mInitialized == true, "Attempt to get material before initializing");
	
	return *mMaterials[enum_cast(surface)];
}

void Material::addMaterial(std::unique_ptr<Material> mat) {
	DEBUG_ASSERT(enum_cast(mat->mType) == mMaterials.size(), "Materials must be registered in the same order as the enum.");
	mMaterials.emplace_back(std::move(mat));
}

Material& Material::_setReplaceable() {
	mReplaceable = true;
	return *this;
}

Material& Material::_setFlammable() {
	mFlammable = true;
	return *this;
}

Material& Material::_setNotAlwaysDestroyable() {
	mAlwaysDestroyable = false;
	return *this;
}

Material& Material::_setNeverBuildable() {
	mNeverBuildable = true;
	return *this;
}

Material& Material::_setNotBlockingMotion() {
	mBlocksMotion = false;
	return *this;
}

Material& Material::_setNotSolid() {
	mSolid = false;
	return *this;
}

Material& Material::_setSuperHot() {
	mSuperHot = true;
	return *this;
}

//	@Note: It should be noted that the order of this matters when registering the material types.
//	These should stay in the same order as they are in the MaterialType enum.
void Material::_setupSurfaceMaterials() {
	//air
	registerMaterial(MaterialType::Air, Settings::Gas, 1.f);
	//_addMaterial(SurfaceType::Air, mat);
	//dirt
	registerMaterial(MaterialType::Dirt)._setMapColor(Color::fromARGB(0x976D4D));
	//wood
	registerMaterial(MaterialType::Wood)._setFlammable()._setMapColor(Color::fromARGB(0x8F7748));
	//stone
	registerMaterial(MaterialType::Stone)._setNotAlwaysDestroyable()._setMapColor(Color::fromARGB(0x707070));
	//metal
	registerMaterial(MaterialType::Metal)._setNotAlwaysDestroyable()._setMapColor(Color::fromARGB(0xA7A7A7));
	//water
	registerMaterial(MaterialType::Water, Settings::Liquid, 1.f)._setReplaceable()._setMapColor(Color::fromARGB(0x4040ff));
	//lava
	registerMaterial(MaterialType::Lava, Settings::Liquid, 1.f)._setReplaceable()._setSuperHot()._setMapColor(Color::fromARGB(0xff0000));
	//leaves
	registerMaterial(MaterialType::Leaves, Settings::Normal, 0.5f)._setNeverBuildable()._setFlammable()._setMapColor(Color::fromARGB(0x007C00));
	//plant
	registerMaterial(MaterialType::Plant, Settings::Decoration, 1.f)._setMapColor(Color::fromARGB(0x007C00));
	//replaceable plant
	registerMaterial(MaterialType::ReplaceablePlant, Settings::Decoration, 1.f)._setFlammable()._setReplaceable()._setMapColor(Color::fromARGB(0x007C00));
	//sponge
	registerMaterial(MaterialType::Sponge)._setMapColor(Color::fromARGB(0xE5E533));
	//cloth
	registerMaterial(MaterialType::Cloth, Settings::Normal, 0.8f)._setFlammable()._setMapColor(Color::fromARGB(0xC7C7C7));
	//bed
	registerMaterial(MaterialType::Bed, Settings::Normal, 0.1f)._setMapColor(Color::fromARGB(0x993333));
	//fire
	registerMaterial(MaterialType::Fire, Settings::Gas, 1.f)._setSuperHot()._setMapColor(Color::fromARGB(0xff0000));
	//sand
	registerMaterial(MaterialType::Sand)._setMapColor(Color::fromARGB(0xF7E9A3));
	//decoration
	registerMaterial(MaterialType::Decoration, Settings::Decoration, 1.f);
	//glass
	registerMaterial(MaterialType::Glass, Settings::Normal, 1.f)._setNeverBuildable();
	//explosive
	registerMaterial(MaterialType::Explosive)._setFlammable()._setNeverBuildable()._setMapColor(Color::fromARGB(0xff0000));
	//ice
	registerMaterial(MaterialType::Ice)._setNeverBuildable()._setMapColor(Color::fromARGB(0xa0a0ff));
	//packed ice
	registerMaterial(MaterialType::PackedIce)._setMapColor(Color::fromARGB(0xa0a0ff));
	//topSnow
	registerMaterial(MaterialType::TopSnow, Settings::Decoration, 0.89f)._setReplaceable()._setNeverBuildable()._setNotAlwaysDestroyable()._setMapColor(Color::fromARGB(0xffffff));
	//snow
	registerMaterial(MaterialType::Snow)._setNotAlwaysDestroyable()._setMapColor(Color::fromARGB(0xffffff));
	//cactus
	registerMaterial(MaterialType::Cactus, Settings::Normal, 0.5f)._setNeverBuildable()._setMapColor(Color::fromARGB(0x007C00));
	//clay
	registerMaterial(MaterialType::Clay)._setMapColor(Color::fromARGB(0xA4A8B8));
	//vegetable
	registerMaterial(MaterialType::Vegetable)._setMapColor(Color::fromARGB(0x007C00));
	//portal
	registerMaterial(MaterialType::Portal, Settings::Portal);
	//cake
	registerMaterial(MaterialType::Cake, Settings::Normal, 0.8f);
	//web
	registerMaterial(MaterialType::Web, Settings::Normal, 0.8f)._setNotBlockingMotion()._setNotAlwaysDestroyable()._setNotSolid()._setMapColor(Color::fromARGB(0xC7C7C7));
	//redstone wire
	registerMaterial(MaterialType::RedstoneWire, Settings::Normal, 0.8f)._setNotBlockingMotion()._setNotSolid();
	//carpet
	registerMaterial(MaterialType::Carpet, Settings::Normal, 0.8f)._setFlammable()._setNotSolid()._setNotBlockingMotion()._setMapColor(Color::fromARGB(0xC7C7C7));
	//buildable Glass
	registerMaterial(MaterialType::BuildableGlass, Settings::Normal, 1.f);
	//slime block
	registerMaterial(MaterialType::Slime, Settings::Normal, 0.1f);
	//piston block
	registerMaterial(MaterialType::Piston, Settings::Normal)._setMapColor(Color::fromARGB(0x707070));
	//Allow block
	registerMaterial(MaterialType::Allow);
	//Deny block
	registerMaterial(MaterialType::Deny);
}

void Material::_setMapColor(const Color& color) {
	mMaterialColor = color;
}
