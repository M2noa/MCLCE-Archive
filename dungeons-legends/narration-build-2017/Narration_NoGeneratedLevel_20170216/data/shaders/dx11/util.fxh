
float4 texture2D_AA(in Texture2D tex, in sampler texSampler, in float2 uv)
{
	// Texture antialiasing
	//
	// The texture coordinates are modified so that the bilinear filter will be one pixel width wide instead of one texel width. 

	// Get the UV deltas
	float2 dUVdx = ddx(uv) * TEXTURE_DIMENSIONS.xy;
	float2 dUVdy = ddy(uv) * TEXTURE_DIMENSIONS.xy;
	float2 dU = float2(dUVdx.x, dUVdy.x);
	float2 dV = float2(dUVdx.y, dUVdy.y);

	float duUV = sqrt(dot(dU, dU));
	float dvUV = sqrt(dot(dV, dV));

	// Determine mip map LOD
    float d = max(dot(dUVdx, dUVdx), dot(dUVdy, dUVdy));
	float mipLevel = .5f * log2(d);
	mipLevel = mipLevel + .5f;	// Mip bias to reduce aliasing
	mipLevel = clamp(mipLevel, 0.0f, TEXTURE_DIMENSIONS.z);

	float2 uvModified; 
	if( mipLevel >= 1.0f)
	{
		uvModified = uv;
	}
	else
	{
		// First scale the uv so that each texel has a uv range of [0,1]
		float2 texelUV = frac(uv * TEXTURE_DIMENSIONS.xy);

		// Initially set uvModified to the floor of the texel position
		uvModified = (uv * TEXTURE_DIMENSIONS.xy) - texelUV;

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
	}
	float4 diffuse = tex.Sample(TextureSampler0, uvModified);
	return diffuse;

}

