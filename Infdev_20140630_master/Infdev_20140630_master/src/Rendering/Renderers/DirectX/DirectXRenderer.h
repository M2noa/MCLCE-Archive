#pragma once

#include "../../Renderer.h"
#include "./DirectXFactory.h"

namespace Esteem
{
	namespace DirectX
	{
		class DirectXRenderer : public Renderer
		{
		private:
			DirectXFactory* directXFactory;
		};
	}
}