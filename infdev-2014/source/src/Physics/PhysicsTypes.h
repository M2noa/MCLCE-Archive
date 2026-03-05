#pragma once

namespace Esteem
{
	namespace Collision
	{
		enum class ShapeType
		{
			NONE = 0,

			CUBE,
			CYLINDER,
			SPHERE,
			CAPSULE,
			CONE,
			PYRAMIDE,
			QUAD,
			ELLIPSOID,
			PLANE,
			CONVEX_HULL,

			CUSTOM_CONVEX,
			CUSTOM_CONCAVE,

			TRIANGLES = 0xFF
		};

		enum FilterGroups
		{
			DefaultFilter = 1,
			StaticFilter = 2,
			KinematicFilter = 4,
			DebrisFilter = 8,
			SensorTrigger = 16,
			CharacterFilter = 32,

			AllCollidable = -1 & ~SensorTrigger,
			AllFilter = -1 //all bits sets: DefaultFilter | StaticFilter | KinematicFilter | DebrisFilter | SensorTrigger
		};

		//// \brief primarily Bullet's flags
		enum CollisionFlags
		{
			// bullet flags
			CF_STATIC_OBJECT = 1,
			CF_KINEMATIC_OBJECT = 2,
			CF_NO_CONTACT_RESPONSE = 4,
			CF_CUSTOM_MATERIAL_CALLBACK = 8,			// This allows per-triangle material (friction/restitution)
			CF_CHARACTER_OBJECT = 16,
			CF_DISABLE_VISUALIZE_OBJECT = 32,			// Disable debug drawing
			CF_DISABLE_SPU_COLLISION_PROCESSING = 64,	// Disable parallel/SPU processing
			CF_HAS_CONTACT_STIFFNESS_DAMPING = 128,
			CF_HAS_CUSTOM_DEBUG_RENDERING_COLOR = 256,
			CF_HAS_FRICTION_ANCHOR = 512,
			CF_HAS_COLLISION_SOUND_TRIGGER = 1024,

			// redifinition for easier use
			CFE_NOT_SOLID = CF_NO_CONTACT_RESPONSE,						// Objects will pass through this object
			CFE_PROGRAMMABLE_MATERIAL = CF_CUSTOM_MATERIAL_CALLBACK,	// Used to change anything physics related when a collision occurs, takes place in the physics calculations

			// custom types
			CFE_ENABLE_CALLBACK = 2048,						// Object will call the callback function, triggers and collisions alike.
			CFE_LIQUID = 4096,								// Objects hitting this type will get water physics applied, mix with NOT_SOLID to make traversable water
			CFE_CHARACTER_STATE = 8192,						// Characters hitting this type will get its state set, set the to be applied state seperatly
			CFE_PERMEABLE = 16384,							// Allow small objects and fluids (including gases) to pass through

			MAX_VALUE = CFE_PERMEABLE
		};

		enum PresetCollisionFlags
		{
			PRESET_RIGID_BODY = 0,
			PRESET_STATIC = CF_STATIC_OBJECT,
			PRESET_KINEMATIC = CF_KINEMATIC_OBJECT,
			PRESET_CHARACTER = CF_KINEMATIC_OBJECT | CF_CHARACTER_OBJECT,

			PRESET_TRIGGER = CFE_NOT_SOLID | CFE_ENABLE_CALLBACK,			// Preset rigidbody trigger type, for the static version use PRESET_TRIGGER_STATIC
			PRESET_LIQUID = CFE_LIQUID | CFE_NOT_SOLID,						// Preset rigidbody water/fluid type, for the static version use PRESET_FLUID_STATIC
			PRESET_CHAR_STATE_TRIGGER = CFE_CHARACTER_STATE | CFE_NOT_SOLID,// Preset character state changer, for the static version use PRESET_CHAR_STATE_TRIGGER_STATIC

			PRESET_TRIGGER_STATIC = PRESET_TRIGGER | CF_STATIC_OBJECT,		// Preset trigger type, use this in many trigger cases
			PRESET_LIQUID_STATIC = PRESET_LIQUID | CF_STATIC_OBJECT,		// Preset water/fluid type, use this in many fluid cases
			PRESET_CHAR_STATE_TRIGGER_STATIC = PRESET_CHAR_STATE_TRIGGER | CF_STATIC_OBJECT,	// Preset character state changer, use this in many ladder cases
		};
	}

	enum CharacterState
	{
		CS_IN_LIQUID = 1,
		CS_ON_LADDER = 2,
	};
}