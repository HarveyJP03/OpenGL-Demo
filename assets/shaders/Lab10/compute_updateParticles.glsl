#version 460 core

layout(local_size_x = 8, local_size_y = 1) in; //64 particles, 8x1 local, so 8x1 global = 64 in 1D

uniform float u_deltaTime;
uniform float u_speed;
uniform vec3 u_emitterLocation;
uniform vec2 u_velocityRangeX;
uniform vec2 u_velocityRangeY;
uniform vec2 u_velocityRangeZ;

struct Particle
{
	vec4 position;
	vec4 velocity;
};

layout(std430, binding = 0) buffer particles //read SSBO
{
	Particle particle[];
};

float rand(vec2 co);
float remap(float currValue, float  inMin,float inMax,float outMin, float outMax);
vec3 randomDirection(vec2 seed);

void main()
{
	vec4 gravity = vec4(0.0f, 0.0f, 0.0f, 0.0f);
	
	uint index = gl_GlobalInvocationID.x;
	particle[index].position.w -= u_deltaTime; //age
	particle[index].velocity += gravity * u_deltaTime;
	particle[index].position.xyz += particle[index].velocity.xyz * u_deltaTime + 0.5 * u_deltaTime * u_deltaTime * gravity.xyz; //SUVAT

	if(particle[index].position.w <= 0.0f)
	{
		particle[index].position.xyz = u_emitterLocation; //emmitter postion
		
		float randomAge = rand(vec2(gl_GlobalInvocationID.x, gl_GlobalInvocationID.x)); //age
		randomAge = remap(randomAge, 0, 1, 2.0, 5.0);
		particle[index].position.w = randomAge;

		float randx = rand(vec2(gl_GlobalInvocationID.x, gl_GlobalInvocationID.x * gl_GlobalInvocationID.x));
		float randy = rand(vec2(gl_GlobalInvocationID.x, randx * gl_GlobalInvocationID.x));
		float randz = rand(vec2(randy, randx * randy));
	
		particle[index].velocity = vec4( randomDirection( vec2( randz * randx - randy, randy * randx ) ), 0.69f) * u_speed;
		
		particle[index].velocity.x = remap(particle[index].velocity.x, -1, 1, u_velocityRangeX.x, u_velocityRangeX.y);
		particle[index].velocity.y = remap(particle[index].velocity.y, -1, 1, u_velocityRangeY.x, u_velocityRangeY.y);
		particle[index].velocity.z = remap(particle[index].velocity.z, -1, 1, u_velocityRangeZ.x, u_velocityRangeZ.y);
	}
}

float rand(vec2 co)
{
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

float remap(float currValue, float  inMin,float inMax,float outMin, float outMax) 
{
	float t = (currValue - inMin) / (inMax - inMin);
	return mix(outMin, outMax, t);
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




