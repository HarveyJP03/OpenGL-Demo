#version 460 core

layout(local_size_x = 8, local_size_y = 1) in; //64 particles, 8x1 local, so 8x1 global = 64 in 1D

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
	particle[index].position.xyz = vec3(0.0f, 5.0f, -9.0f); //emmitter postion
	particle[index].position.w = 1.0f; //age
	particle[index].velocity = vec4(0.69f, 0.69f, 0.69f, 0.69f);
}


