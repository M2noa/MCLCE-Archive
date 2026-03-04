#pragma once

namespace Esteem
{
	template<class T, class R>
	class RenderTechnique abstract
	{
	protected:
		R& renderer;

		RenderTechnique(R& renderer)
			: renderer(renderer)
		{ }

	public:
		virtual ~RenderTechnique() = default;
		virtual void RenderFrame(const T& renderData) = 0;

		virtual void Initialize() = 0;
	};
}