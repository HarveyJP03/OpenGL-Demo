#version 460 core

layout(location = 0) out vec4 ppColour;

in vec2 texCoord;

uniform sampler2D u_inputTexture;
uniform sampler2D u_forwardTexture;

uniform sampler2D u_inputDepthTexture;
uniform sampler2D u_forwardDepthTexture;

vec3 aces(vec3 x);

void main()
{
	vec3 texColour = vec3(0.0f);
	
	float inputDepth = texture(u_inputDepthTexture, texCoord).r;
	float forwardDepth = texture(u_forwardDepthTexture, texCoord).r;


	if( inputDepth < forwardDepth )
	{
		texColour = texture(u_inputTexture, texCoord).rgb;
	}
	
	else
	{
		//Becuase of the fact I need to do two seperate passes, the pixles behind forward render for transparency are black, since its a seperate scene, so manually blend here
		texColour = mix(texture(u_inputTexture, texCoord).rgb, texture(u_forwardTexture, texCoord).rgb,  texture(u_forwardTexture, texCoord).a );
	}

	if(forwardDepth > 0.9999f && inputDepth > 0.9999f)
	{
		texColour = texture(u_inputTexture, texCoord).rgb;
	}

	
	ppColour = vec4(texColour, 1.0f);
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
