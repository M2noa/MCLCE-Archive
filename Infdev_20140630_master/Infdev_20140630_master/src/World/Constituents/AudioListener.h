#pragma once

#include "./AbstractConstituent.h"

#include <SFML/Audio.hpp>

namespace Esteem
{
	class AudioListener : public AbstractConstituent
	{
	private:
		sf::Listener listener;
		bool enabled;

		AudioListener()
			: AbstractConstituent(cgc::strong_ptr<Entity>())
		{}

	public:

		AudioListener(const cgc::strong_ptr<Entity>& entity)
			: AbstractConstituent(entity)
		{}

		virtual void Initialize();

		virtual void Start();

		virtual void Update();

		virtual void LateUpdate();

		virtual void OnEnable();

		virtual void OnDisable();

		virtual void OnDestroy();

		virtual void DirtyCleanUp();

		virtual bool GetEnabled() const { return enabled; }
		virtual void SetEnabled(bool enable) { enabled = enable; enable ? OnEnable() : OnDisable(); };
		virtual void Destroy() { OnDestroy(); delete this; }
	};
}
