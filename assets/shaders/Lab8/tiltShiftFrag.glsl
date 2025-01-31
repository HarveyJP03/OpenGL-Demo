#version 460 core

layout(location = 0) out vec4 ppColour;

in vec2 texCoord;
in vec2 screenSize;

uniform sampler2D u_inputTexture;
uniform sampler2D u_blurTexture;  
uniform float u_intensity;

void main()
{
	vec3 result = vec3(0.0f);
	vec3 colour = texture(u_inputTexture, texCoord).rgb;
	vec3 blurredColour = texture(u_blurTexture, texCoord).rgb;

	float lowerBound = 0.25f;
	float upperBound = 1.0f - lowerBound;

	float intensity = 0.15f;

	result = colour;

	if(texCoord.y < lowerBound)
	{
		float blurAmount = smoothstep(lowerBound, 0.0f + u_intensity ,texCoord.y);
		result = mix(colour, blurredColour, blurAmount);
	}
	else if(texCoord.y > upperBound)
	{
		float blurAmount = smoothstep(upperBound, 1.0f - u_intensity ,texCoord.y);
		result = mix(colour, blurredColour, blurAmount);
	}

	ppColour = vec4(result, 1.0);
}

