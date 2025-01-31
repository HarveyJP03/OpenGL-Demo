#version 460 core

layout(local_size_x = 8, local_size_y = 1) in; //256 particles, 8x1 local, so 32x1 global = 256 in 1D

float rand(vec2 co);
vec3 randomDirection(vec2 seed);
float remap(float currValue, float  inMin,float inMax,float outMin, float outMax);
float rigidFBM(vec2 position, float octaves);


struct Particle
{
	vec4 position;
	vec4 velocity;
};

layout(std430, binding = 0) buffer particles //read SSBO
{
	Particle particle[];
};

void main()
{
	uint index = gl_GlobalInvocationID.x;
	particle[index].position.xyz = vec3(0.0f, 10.0f, -9.0f); //emmitter postion
	
	float randomAge = rand(vec2(gl_GlobalInvocationID.x, gl_GlobalInvocationID.x)); //age
	randomAge = remap(randomAge, 0, 1, 2.0, 5.0);

	//Just some more randomisation here
	float randx = rand(vec2(gl_GlobalInvocationID.x, gl_GlobalInvocationID.x * gl_GlobalInvocationID.x));
	float randy = rand(vec2(gl_GlobalInvocationID.x, randx * gl_GlobalInvocationID.x));
	float randz = rand(vec2(randy, randx * randy));
	
	particle[index].velocity = vec4(randomDirection(vec2(randz, randy * randx) ), 0.69f);

	particle[index].position.w = randomAge;
}

float rand(vec2 co)
{
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

vec3 randomDirection(vec2 seed) 
{
    float theta = rand(seed) * 2.0 * 3.14159;
    float phi = acos(2.0 * rand(seed * 2.0) - 1.0);
    float x = sin(phi) * cos(theta) ;
    float y = sin(phi) * sin(theta);
    float z = cos(phi);
    return vec3(x, y, z);
}

float remap(float currValue, float  inMin,float inMax,float outMin, float outMax) 
{
	float t = (currValue - inMin) / (inMax - inMin);
	return mix(outMin, outMax, t);
}
