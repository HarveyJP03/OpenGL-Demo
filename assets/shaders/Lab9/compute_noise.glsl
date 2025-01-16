#version 460 core

layout(local_size_x = 16, local_size_y = 16) in; //512x512 image, 32x32 work groups, so each work group should be 16x16. 8 Warps per work group
layout(binding = 0, rgba8) uniform image2D outputImg;

uniform sampler2D u_image;

float noise(in vec2 p);
vec2 hash2(vec2 p);
float remap(float currValue, float  inMin,float inMax,float outMin, float outMax);

void main()
{
	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy); //ID of the work item(thread) in relation to the whole dispatch, not just the work group
	vec2 uv = vec2(pixel_coords) / vec2(1024);
	vec4 pixelCol = texture2D(u_image, uv);

	float noiseValue = noise(uv * 10.0f) * 1.5f;
	noiseValue = remap(noiseValue, -1, 1, 0, 1);

	imageStore(outputImg, pixel_coords, vec4(vec3(noiseValue), 1.0)); //write color to image at pixel coords
}

float noise(in vec2 p)
{
	vec2 i = floor(p);
	vec2 f = fract(p);

	vec2 u = f * f * (3.0 - 2.0 * f);

	return mix(mix(dot(hash2(i + vec2(0.0, 0.0)), f - vec2(0.0, 0.0)),
		dot(hash2(i + vec2(1.0, 0.0)), f - vec2(1.0, 0.0)), u.x),
		mix(dot(hash2(i + vec2(0.0, 1.0)), f - vec2(0.0, 1.0)),
			dot(hash2(i + vec2(1.0, 1.0)), f - vec2(1.0, 1.0)), u.x), u.y);
}

vec2 hash2(vec2 p) 
{
	p = vec2(dot(p, vec2(127.1, 311.7)),
		dot(p, vec2(269.5, 183.3)));

	return -1.0 + 2.0 * fract(sin(p) * 43758.5453123 );
}

float remap(float currValue, float  inMin,float inMax,float outMin, float outMax) 
{
	float t = (currValue - inMin) / (inMax - inMin);
	return mix(outMin, outMax, t);
}