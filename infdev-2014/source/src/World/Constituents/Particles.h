#pragma once

#include "./AbstractConstituent.h"
#include "Rendering/Objects/RenderObject.h"

namespace Esteem
{
	class Particles : public AbstractConstituent
	{
	private:
		RenderObject* renderObject;
	};
}