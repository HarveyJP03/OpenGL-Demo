#version 460 core

layout(location = 0) out vec4 ppColour;

in vec2 texCoord;

uniform sampler2D u_inputTexture;

vec3 aces(vec3 x);

void main()
{
	vec3 texColour = texture(u_inputTexture, texCoord).rgb;
	texColour = aces(texColour); //tonemap
	texColour = pow(texColour, vec3(1.0 / 2.2)); //gamma correct  //TONEMAP AND GAMMA CORRECTION IN THE DEFFERED RENDERING + FORWARD RENDERING PASS (lab10/ FMmixFrag)
	ppColour = vec4(texColour, 0.25f);
}

vec3 aces(vec3 x)
{
	const float a = 2.51;
	const float b = 0.03;
	const float c = 2.43;
	const float d = 0.59;
	const float e = 0.14;
	return clamp((x *(a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}