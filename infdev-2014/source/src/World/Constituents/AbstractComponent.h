#pragma once

#include <memory>
#include <cppu/cgc/pointers.h>

namespace cppu
{
	namespace serial
	{
		class ArchiveWriter;
		class ArchiveReader;
	}
}

namespace Esteem
{
	class WorldController;
	class Entity;

	class AbstractComponent
	{
		friend class Entity;
	private:
		cgc::weak_ptr<Entity> entity;
		bool enabled;

	public:
		AbstractComponent(const cgc::strong_ptr<Entity>& entity)
			: entity(entity)
			, enabled(true)
		{
		}

		virtual ~AbstractComponent() { }

		virtual void Initialize() = 0;

		virtual void Start() {}

		virtual void Update() {}

		virtual void LateUpdate() {}

		virtual void DirtyCleanUp() {}

		virtual void OnEnable() {};

		virtual void OnDisable() {};

		virtual void OnDestroy() {};

		virtual bool GetEnabled() const { return enabled; }

		virtual void SetEnabled(bool enabled) { this->enabled = enabled; }

		virtual void Destroy() {}

		virtual void Serialize(serial::ArchiveWriter& archive) const {};

		virtual void DeSerialize(serial::ArchiveReader& archive) {};

		inline const cgc::weak_ptr<Entity>& GetEntity() const { return entity; };
	};
}
