#version 460 core

struct Particle
{
	vec4 position;
	vec4 velocity;
};

layout(std430, binding = 0) buffer particles //read SSBO
{
	Particle particle[];
};

uniform mat4 u_model;
out vec4 pos;

void main()
{
	pos = particle[gl_VertexID].position;
	gl_Position = u_model * vec4(pos.xyz, 1.0);
}
