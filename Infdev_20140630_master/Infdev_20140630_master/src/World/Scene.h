#pragma once

#include <vector>

namespace Esteem
{
	class Scene
	{
	protected:
		bool loaded;

	public:
		Scene()
			: loaded(false)
		{
		}

		virtual ~Scene()
		{
			if(loaded)
				UnloadScene();
		}

		virtual void LoadScene() { loaded = true; }
		virtual void UnloadScene() { loaded = false; }
	};
}