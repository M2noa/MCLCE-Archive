#pragma once

#include <vector>
#include "../../RenderingFactory.h"
#include "./Objects/DirectXShader.h"

namespace Esteem
{
	namespace DirectX
	{
		class DirectXFactory : public RenderingFactory
		{
		private:
			std::vector<DirectXShader*> shaders;
		};
	}
}
