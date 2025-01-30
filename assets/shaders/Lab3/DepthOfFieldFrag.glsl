#version 460 core

layout(location = 0) out vec4 ppColour;

in vec2 texCoord;

uniform sampler2D u_inputTexture; //Non blurred texture
uniform sampler2D u_blurTexture;  
uniform sampler2D u_depthTexture;
uniform float u_focusDistance;
uniform float u_active;

const float near = 0.1f;
const float far = 20.0f; //Value = distance that gets max blur

float focusDistance = 0.15f;

float LineariseDepth(float depth);

void main()
{
	if(u_active == 1.0f)
	{
		vec3 colour = texture(u_inputTexture, texCoord).rgb;
		vec3 blurredColour = texture(u_blurTexture, texCoord).rgb;
		float depth = texture(u_depthTexture, texCoord).r;

		focusDistance = u_focusDistance;
	
		float linearDepth = LineariseDepth(depth);
		float depthNormalised = linearDepth / far;
	
		depthNormalised = clamp(depthNormalised, 0, 1);

		float targetCloseness = depthNormalised - focusDistance; //diff between depth and target depth

		if(targetCloseness < 0.0f)
		{
			targetCloseness = abs(targetCloseness * (1 / focusDistance));
		}


		targetCloseness = clamp(targetCloseness, 0, 1);

		colour = mix(colour, blurredColour, targetCloseness * 2.0f);
	
		ppColour = vec4(colour, 1.0);
	}
	else
	{
		vec3 colour = texture(u_inputTexture, texCoord).rgb;
		ppColour = vec4(colour, 1.0);
	}
}

float LineariseDepth(float depth)
{
	float z = depth * 2.0 - 1.0; //Back to clip space [-1, 1] from Normalised Device Coorinates [0,1]
	return (2.0 * near * far) / (far + near - z * (far - near));
}
