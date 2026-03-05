
vec3 getBadgerInfluenceOverlay(vec3 worldPos, vec3 diffuse)
{
	vec4 overlayColor = vec4(0, 0, 0, 0);
	for (int i = 0; i < NUM_STRUCTURES; ++i)
	{
		vec3 testPos = worldPos.xyz - vec3(0.5, 0.5, 0.5);
		vec3 dist = abs(testPos.xyz - STRUCTURE_DATA[i].xyz);
		vec3 aoe = STRUCTURE_DATA[i].www + vec3(0.5, 0.5, 0.5);
		vec3 res = step(dist, aoe);
		float a = res.x * res.y * res.z;

		float kNumRings = 5.0;
		float kAnimTime = mod(TIME, 2.0) / 2.0;

		float ring = max(dist.x, max(dist.y, dist.z));
		ring = floor(ring - (kAnimTime * kNumRings));
		ring = fract(ring / kNumRings);
		a *= mix(0.5, 1.0, abs(ring * 2.0 - 1.0));

		overlayColor.rgb += STRUCTURE_COLOR[i].rgb * STRUCTURE_COLOR[i].a * a;
	}

	diffuse.rgb = clamp(diffuse.rgb + overlayColor.rgb, vec3(0,0,0), vec3(1,1,1));

	return diffuse;
}
