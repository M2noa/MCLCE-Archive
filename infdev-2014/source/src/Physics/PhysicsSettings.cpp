#include "PhysicsSettings.h"

namespace Esteem
{
	//btCollisionConfiguration* BulletPhysicsSettings::overrideBulletPhysicsConfiguration = nullptr;
	btCollisionConfiguration* PhysicsSettings::overrideBulletPhysicsConfiguration = nullptr;
	CustomStaticConstructorFunc PhysicsSettings::customStaticConstructor = &PhysicsSettings::DummyStaticConstructor;
	CustomRigidConstructorFunc PhysicsSettings::customRigidConstructor = &PhysicsSettings::DummyRigidConstructor;
	CustomWorldCreatorFunc PhysicsSettings::customWorldCreator = &PhysicsSettings::CreateWorld;
}