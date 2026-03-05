
vec2 computeScreenOffsets(vec3 pos, vec4 data, mat4 worldViewProj)
{
	vec3 normal = data.xyz;
	float width = length(normal);
	float pixelScale = abs(data.w);
	float dir = (data.w < 0) ? -1.f : 1.f;

	vec4 pos0 = vec4(pos, 1.f);
	vec4 pos1 = vec4(pos + normal, 1.f);

	pos0 = pos0 * worldViewProj;
	pos1 = pos1 * worldViewProj;

	vec2 screenNormal = (pos1.xy / pos1.w) - (pos0.xy / pos0.w);
	screenNormal = normalize(vec2(-screenNormal.y, screenNormal.x));

	float w = dot(worldViewProj[3], vec4(pos, 1.f));

	float pixelRadius = w * pixelScale;

	return screenNormal * (dir * width * pixelRadius);
}
