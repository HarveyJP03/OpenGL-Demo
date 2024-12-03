#version 460 core

layout(location = 0) out vec4 ppColour;

in vec2 texCoord;

uniform sampler2D u_inputTexture;
uniform vec3 u_tintColour;

vec3 GetRelativeLuminance(vec3 colour);

void main()
{
	vec3 colour = texture(u_inputTexture, texCoord).rgb;
	vec3 tintedColour = (vec3(mix(colour, u_tintColour, GetRelativeLuminance(colour))));

	if(u_tintColour != vec3(0.0f) )
	{
		ppColour = vec4(tintedColour, 1.0);
	}
	else
	{
		ppColour = vec4(colour, 1.0);
	}
}

vec3 GetRelativeLuminance(vec3 colour)
{
	return vec3((0.2126 * colour.r) + (0.7152 * colour.g) + (0.0722 * colour.b));
}
