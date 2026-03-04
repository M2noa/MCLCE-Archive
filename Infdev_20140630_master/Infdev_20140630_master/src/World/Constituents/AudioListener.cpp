#include "AudioListener.h"
#include "../Objects/Entity.h"

#define entity GetEntity()

namespace Esteem
{
	void AudioListener::Initialize()
	{

	}

	void AudioListener::Start()
	{

	}

	void AudioListener::Update()
	{

	}

	void AudioListener::LateUpdate()
	{

	}

	void AudioListener::OnEnable()
	{

	}

	void AudioListener::OnDisable()
	{

	}

	void AudioListener::OnDestroy()
	{

	}

	void AudioListener::DirtyCleanUp()
	{
		listener.setPosition(reinterpret_cast<const sf::Vector3f&>(entity->GetPosition()));
		listener.setUpVector(reinterpret_cast<const sf::Vector3f&>(entity->GetUp()));
		listener.setDirection(reinterpret_cast<const sf::Vector3f&>(entity->GetForward()));
	}
}

#undef entity