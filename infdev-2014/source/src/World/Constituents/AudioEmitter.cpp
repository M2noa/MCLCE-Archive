#include "AudioEmitter.h"
#include "../Objects/Entity.h"

#define entity GetEntity()

namespace Esteem
{
	void AudioEmitter::Initialize()
	{
		emitter.setPosition(reinterpret_cast<const sf::Vector3f&>(entity->GetPosition()));
	}

	void AudioEmitter::Start()
	{

	}

	void AudioEmitter::Play(const std::string& path)
	{
		this->buffer = std::make_shared<sf::SoundBuffer>(sf::SoundBuffer());

		// TODO: Offload this to the factory, max 256 sound buffers active/loaded at a time
		if(!this->buffer->loadFromFile(path))
			Debug::Log("Could not load " + path);
		else
		{
			emitter.setBuffer(*this->buffer.get());
			emitter.play();
		}
	}

	void AudioEmitter::Play(std::shared_ptr<sf::SoundBuffer>& buffer)
	{
		this->buffer = nullptr;

		this->buffer = buffer;
		emitter.setBuffer(*this->buffer.get());
		emitter.play();
	}

	void AudioEmitter::Update()
	{

	}

	void AudioEmitter::LateUpdate()
	{

	}

	void AudioEmitter::OnEnable()
	{

	}

	void AudioEmitter::OnDisable()
	{

	}

	void AudioEmitter::OnDestroy()
	{
		
	}

	void AudioEmitter::DirtyCleanUp()
	{
		emitter.setPosition(reinterpret_cast<const sf::Vector3f&>(entity->GetPosition()));
	}
}

#undef entity