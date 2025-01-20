#version 460 core

layout(local_size_x = 32, local_size_y = 32) in; //512x512 image, 32x32 work groups, so each work group should be 16x16. 8 Warps per work group
layout(binding = 0, rgba8) uniform image2D outputImg;

uniform float u_frequency;
uniform float u_amplitude;
uniform float u_lacunarity;
uniform float u_persistence;

uniform float u_noiseType;

float noise(in vec2 p);
vec2 hash2(vec2 p);
float remap(float currValue, float  inMin,float inMax,float outMin, float outMax);
float FBM(vec2 position, int octaves);
float rigidFBM(vec2 position, float octaves);
float turbulentFBM(vec2 position, float octaves);
float worley(vec2 position);

void main()
{
	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy); //ID of the work item(thread) in relation to the whole dispatch, not just the work group
	vec2 uv = vec2(pixel_coords) / vec2(1024);
	vec4 pixelCol = vec4(0.0f);

	float gradientNoise = noise(uv * u_frequency) * u_amplitude;
	gradientNoise = remap(gradientNoise, -1, 1, 0, 1);

	float FBMnoise = FBM(uv, 5);
	FBMnoise = remap(FBMnoise, -1, 1, 0, 1);

	float FBMrigidNoise = rigidFBM(uv, 5);
	//FBMrigidNoise = remap(FBMrigidNoise, -1, 1, 0, 1);

	float FBMturbulentNoise = turbulentFBM(uv, 5);
	//FBMturbulentNoise = remap(FBMturbulentNoise, -1, 1, 0, 1);

	float combinedNoise = 0.43 * FBM(uv, 5) * FBMnoise + 0.3 * rigidFBM(uv, 5) + 0.3 * turbulentFBM(uv, 5);;

	float noise = 0.0f;

	if(u_noiseType == 0.0f) noise = gradientNoise;
	if(u_noiseType == 1.0f) noise = FBMnoise;
	if(u_noiseType == 2.0f) noise = FBMrigidNoise;
	if(u_noiseType == 3.0f) noise = FBMturbulentNoise;
	if(u_noiseType == 4.0f) noise = combinedNoise;
	if(u_noiseType == 5.0f) noise = worley(uv);



	imageStore(outputImg, pixel_coords, vec4(vec3(noise), 1.0)); //write color to image at pixel coords
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

float rigidFBM(vec2 position, float octaves)
{
	float total = 0.0f;
	float totalAmplitude = 0.0f;

	float frequency = u_frequency;
	float amplitude = u_amplitude;

	for(int i = 0; i < octaves; i++)
	{
		float noise = noise(position * frequency);
		noise = abs(noise);
		noise = 1.0 - noise;
		noise *= noise;
		
		total += noise * amplitude;
		totalAmplitude += amplitude;

		frequency *= u_lacunarity;
		amplitude *= u_persistence;
	}

	total /= totalAmplitude;
	total *= u_amplitude;
	return total * total;
}

float turbulentFBM(vec2 position, float octaves)
{
	float total = 0.0f;
	float totalAmplitude = 0.0f;

	float frequency = u_frequency;
	float amplitude = u_amplitude;
	float persistence = u_persistence;

	for(int i = 0; i < octaves; i++)
	{
		float noise = noise(position * frequency);
		noise = abs(noise);
		
		total += noise * amplitude;
		totalAmplitude += amplitude;

		frequency *= u_lacunarity;
		amplitude *= u_persistence;
	}

	total /= totalAmplitude;
	total *= u_amplitude;
	return total;
}

float worley(vec2 position)
{
	const float gridSize = 4; //Need this to be a float, otherwise cellSize comes out as an integer
	const int i_gridSize = 4;  //Need an int version of this for the array, but not allowed to use casts in GLSL...
	const float cellSize = 1 / gridSize;
    vec2 points[i_gridSize][i_gridSize];

	for(int x = 0; x < gridSize; x++)
	{
		for(int y = 0; y < gridSize; y++)
		{
			float xminValue = x * cellSize;
			float yminValue = y * cellSize;

			float xmaxValue = xminValue + cellSize;
			float ymaxValue = yminValue + cellSize;

			float xValue = (xminValue + xmaxValue) * 0.5f;
			float yValue = (yminValue + ymaxValue) * 0.5f;

			vec2 point = vec2(xValue, yValue);


			points[x][y] = point;
		}
	}

	float minDist = 1.0f;

	for(int x = 0; x < gridSize; x++)
	{
		for(int y = 0; y < gridSize; y++)
		{
			minDist = min(minDist, distance(position, points[x][y]));
		}
	}

	return minDist * u_amplitude;


}