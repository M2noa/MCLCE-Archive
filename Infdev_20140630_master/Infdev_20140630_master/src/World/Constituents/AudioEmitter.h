#pragma once

#include "./AbstractConstituent.h"
#include "Utils/Debug.h"

#include <string>
#include <memory>
#include <SFML/Audio.hpp>

namespace Esteem
{
	class AudioEmitter : public AbstractConstituent
	{
	private:
		std::shared_ptr<sf::SoundBuffer> buffer;
		sf::Sound emitter;
		bool enabled;

		AudioEmitter()
			: AbstractConstituent(cgc::strong_ptr<Entity>())
		{}

	public:

		AudioEmitter(const cgc::strong_ptr<Entity>& entity)
			: AbstractConstituent(entity)
		{}

		virtual void Initialize();

		virtual void Start();

		void Play(const std::string& path);

		void Play(std::shared_ptr<sf::SoundBuffer>& buffer);

		inline void Play()
		{
			emitter.play();
		}

		inline void Pause()
		{
			emitter.pause();
		}

		inline void Stop()
		{
			emitter.stop();
		}

		inline void SetVolume(float volume)
		{
			emitter.setVolume(volume);
		}

		inline float GetVolume()
		{
			return emitter.getVolume();
		}

		inline void SetAttenuation(float volume)
		{
			emitter.setAttenuation(volume);
		}

		inline float GetAttenuation()
		{
			return emitter.getAttenuation();
		}

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