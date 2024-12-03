#version 460 core

layout(location = 0) out vec4 ppColour;

in vec2 texCoord;

uniform sampler2D u_inputTexture;

void main()
{
	vec3 colour = texture(u_inputTexture, texCoord).rgb;

	vec2 screenCentre = vec2( 0.5, 0.5 );
	float distance = distance(texCoord, screenCentre);
	float vignette = smoothstep(0.5, 0.7, distance);

	ppColour = vec4(colour * (1.0 - vignette), 1.0);
}