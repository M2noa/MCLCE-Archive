#pragma once

namespace mce {
	enum class PrimitiveMode : byte {
		None,
		QuadList,
		TriangleList,
		TriangleStrip,
		//TriangleFan, /* Not a type in DX11 */
		LineList,
		LineStrip
	};

}
