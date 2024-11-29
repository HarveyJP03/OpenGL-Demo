#version 460 core

layout(location = 0) out vec4 ppColour;

in vec2 texCoord;

uniform sampler2D u_inputTexture;

void main()
{
	vec3 texColour = texture(u_inputTexture, texCoord).rgb;
	ppColour = vec4(texColour, 1.0f);
}