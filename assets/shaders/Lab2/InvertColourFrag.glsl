#version 460 core

layout(location = 0) out vec4 ppColour;

in vec2 texCoord;

uniform sampler2D u_inputTexture;

void main()
{
	vec3 colour = texture(u_inputTexture, texCoord).rgb;
	colour = 1.0 - colour;
	ppColour = vec4(colour, 1.0);
}