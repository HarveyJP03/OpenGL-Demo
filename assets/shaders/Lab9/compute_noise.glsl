#version 460 core

layout(local_size_x = 32, local_size_y = 32) in; //512x512 image, 32x32 work groups, so each work group should be 16x16. 8 Warps per work group
layout(binding = 0, rgba8) uniform image2D outputImg;

uniform float u_frequency;
uniform float u_amplitude;
uniform float u_lacunarity;
uniform float u_persistence;


float noise(in vec2 p);
vec2 hash2(vec2 p);
float remap(float currValue, float  inMin,float inMax,float outMin, float outMax);
float FBM(vec2 position, int octaves);

void main()
{
	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy); //ID of the work item(thread) in relation to the whole dispatch, not just the work group
	vec2 uv = vec2(pixel_coords) / vec2(1024);
	vec4 pixelCol = vec4(0.0f);

	float FBMnoise = FBM(uv, 5);
	FBMnoise = remap(FBMnoise, -1, 1, 0, 1);

	imageStore(outputImg, pixel_coords, vec4(vec3(FBMnoise), 1.0)); //write color to image at pixel coords
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

float FBM(vec2 position, int octaves)
{
	float total = 0.0f;
	float totalAmplitude = 0.0f; //These get added to

	float frequency = u_frequency;
	float amplitude = u_amplitude;

	for(int i = 0; i < octaves; i++)
	{
		total += noise(position * frequency) * amplitude;
		frequency *= u_lacunarity;
		amplitude *= u_persistence;
		totalAmplitude += amplitude;
	}

	return u_amplitude * total / totalAmplitude;
}