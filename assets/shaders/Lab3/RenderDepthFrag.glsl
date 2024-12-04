#version 460 core

layout(location = 0) out vec4 ppColour;

in vec2 texCoord;

uniform sampler2D u_inputTexture;

float LineariseDepth(float depth);

void main()
{
	float colour = texture(u_inputTexture, texCoord).r;
	float linearDepth = LineariseDepth(colour);
	float depthNormalised = linearDepth / 250.0f;
	
	ppColour = vec4(vec3(depthNormalised), 1.0);
}

float LineariseDepth(float depth)
{
	float z = depth * 2.0 - 1.0; //Back to clip space [-1, 1] from Normalised Device Coorinates [0,1]
	return (2.0 * 0.1f * 250.0f) / (250.0f + 0.1f - z * (250.0f - 0.1f));
}
