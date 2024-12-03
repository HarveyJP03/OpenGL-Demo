#version 460 core

layout(location = 0) out vec4 ppColour;

in vec2 texCoord;

uniform sampler2D u_inputTexture;

vec3 GetRelativeLuminance(vec3 colour);

void main()
{
	vec3 colour = texture(u_inputTexture, texCoord).rgb;
	
	ppColour = vec4(GetRelativeLuminance(colour), 1.0);
}

vec3 GetRelativeLuminance(vec3 colour)
{
	return vec3((0.2126 * colour.r) + (0.7152 * colour.g) + (0.0722 * colour.b));
}
