
float4 texture2D_AA(in Texture2D tex, in sampler texSampler, in float2 uv)
{
	// Texture antialiasing
	//
	// The texture coordinates are modified so that the bilinear filter will be one pixel width wide instead of one texel width. 

	if (tex.CalculateLevelOfDetailUnclamped(TextureSampler0, uv) < -0.5f)
	{
		// Get the UV deltas
		float2 dUVdx = ddx(uv) * TEXTURE_DIMENSIONS.xy;
		float2 dUVdy = ddy(uv) * TEXTURE_DIMENSIONS.xy;
		float2 dU = float2(dUVdx.x, dUVdy.x);
		float2 dV = float2(dUVdx.y, dUVdy.y);

		float duUV = sqrt(dot(dU, dU));
		float dvUV = sqrt(dot(dV, dV));

		// First scale the uv so that each texel has a uv range of [0,1]
		float2 texelUV = frac(uv * TEXTURE_DIMENSIONS.xy);

		// Initially set uvModified to the floor of the texel position
		float2 uvModified = (uv * TEXTURE_DIMENSIONS.xy) - texelUV;

		// Modify the texelUV to push the uvs toward the edges.
		//          |                 |       |                   |
		//          |         _/      |       |           /       |
		//  Change  | U     _/        |  to   | U     ___/        |
		//          |     _/          |       |     /             |
		//          |    /            |       |    /              |
		//          |         X       |       |         X         |
		float scalerU = 1.0f / (duUV);
		float scalerV = 1.0f / (dvUV);
		float2 scaler = max(float2(scalerU, scalerV), 1.0f);
		texelUV = clamp(texelUV * scaler, 0.0f, .5f) + clamp(texelUV*scaler - (scaler - .5f), 0.0f, .5f);
		uvModified += texelUV;
		uvModified /= TEXTURE_DIMENSIONS.xy;

		// Update uv
		uv = uvModified;
	}

	return tex.Sample(TextureSampler0, uv);
}

float MakeDepthLinear(float z, float n, float f, bool scaleZ)
{
	//Remaps z from [0, 1] to [-1, 1].
	if (scaleZ) {
		z = 2.f * z - 1.f;
	}
	return (2.f * n) / (f + n - z * (f - n));
}

float2 computeScreenOffsets(in float3 pos, in float4 data, in float4x4 worldViewProj)
{
	float3 normal = data.xyz;
	float width = length(normal);
	float pixelScale = abs(data.w);
	float dir = (data.w < 0) ? -1.f : 1.f;

	float4 pos0 = float4(pos, 1.f);
	float4 pos1 = float4(pos + normal, 1.f);

	pos0 = mul(worldViewProj, pos0);
	pos1 = mul(worldViewProj, pos1);

	float2 screenNormal = (pos1.xy / pos1.w) - (pos0.xy / pos0.w);
	screenNormal = normalize(float2(-screenNormal.y, screenNormal.x));

	float w = dot(worldViewProj[3], float4(pos, 1.f));

	float pixelRadius = w * pixelScale;
	return screenNormal * (dir * width * pixelRadius);
}

// modulo that behaves like glsl (aka: like we actually expect)
float mod(float x, float y)
{
	return x - y * floor(x / y);
}