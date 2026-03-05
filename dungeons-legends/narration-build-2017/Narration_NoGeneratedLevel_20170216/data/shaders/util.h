#if __VERSION__ >= 300

uniform highp vec3 TEXTURE_DIMENSIONS;

vec4 texture2D_AA(in sampler2D tex, in highp vec2 uv)
{
	// Texture antialiasing
	//
	// The texture coordinates are modified so that the bilinear filter will be one pixel wide instead of one texel wide. 

	// Get the UV deltas
	highp vec2 dUVdx = dFdx(uv) * TEXTURE_DIMENSIONS.xy;
	highp vec2 dUVdy = dFdy(uv) * TEXTURE_DIMENSIONS.xy;
	highp vec2 dU = vec2(dUVdx.x, dUVdy.x);
	highp vec2 dV = vec2(dUVdx.y, dUVdy.y);

	highp float duUV = sqrt(dot(dU, dU));
	highp float dvUV = sqrt(dot(dV, dV));

	// Determine mip map LOD
	highp float d = max(dot(dUVdx, dUVdx), dot(dUVdy, dUVdy));
	highp float mipLevel = .5 * log2(d);
	mipLevel = mipLevel + .5;	// Mip bias to reduce aliasing
	mipLevel = clamp(mipLevel, 0.0, TEXTURE_DIMENSIONS.z);

	highp vec2 uvModified;
	if( mipLevel >= 1.0)
	{
		uvModified = uv;
	}
	else
	{
		// First scale the uv so that each texel has a uv range of [0,1]
		highp vec2 texelUV = fract(uv * TEXTURE_DIMENSIONS.xy);

		// Initially set uvModified to the floor of the texel position
		uvModified = (uv * TEXTURE_DIMENSIONS.xy) - texelUV;

		// Modify the texelUV to push the uvs toward the edges.
		//          |                 |       |                   |
		//          |         _/      |       |           /       |
		//  Change  | U     _/        |  to   | U     ___/        |
		//          |     _/          |       |     /             |
		//          |    /            |       |    /              |
		//          |         X       |       |         X         |
		highp float scalerU = 1.0 / (duUV);
		highp float scalerV = 1.0 / (dvUV);
		highp vec2 scaler = max(vec2(scalerU, scalerV), 1.0);
		texelUV = clamp(texelUV * scaler, 0.0, .5) + clamp(texelUV*scaler - (scaler - .5), 0.0, .5);
		uvModified += texelUV;
		uvModified /= TEXTURE_DIMENSIONS.xy;
	}
	vec4 diffuse = texture2D(tex, uvModified);
	return diffuse;

}

#endif //__VERSION__ >= 300
