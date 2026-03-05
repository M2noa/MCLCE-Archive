
float3 getBadgerInfluenceOverlay(float3 worldPos, float3 diffuse)
{
	float4 overlayColor = float4(0, 0, 0, 0);
	for (int i = 0; i < NUM_STRUCTURES; ++i)
	{
		float3 testPos = worldPos.xyz - float3(0.5, 0.5, 0.5);
		float3 dist = abs(testPos.xyz - STRUCTURE_DATA[i].xyz);
		float3 aoe = STRUCTURE_DATA[i].www + float3(0.5, 0.5, 0.5);
		float3 res = step(dist, aoe);
		float a = res.x * res.y * res.z;

		const float kNumRings = 5.0;
		const float kAnimTime = mod(TIME, 2.0) / 2.0;

		float ring = max(dist.x, max(dist.y, dist.z));
		ring = floor(ring - (kAnimTime * kNumRings));
		ring = frac(ring / kNumRings);
		a *= lerp(0.5, 1.0, abs(ring * 2.0 - 1.0));

		overlayColor.rgb += STRUCTURE_COLOR[i].rgb * STRUCTURE_COLOR[i].a * a;
	}

	diffuse.rgb = saturate(diffuse.rgb + overlayColor.rgb);

	return diffuse;
}
