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
		texColour = mix(texture(u_inputTexture, texCoord).rgb, texture(u_forwardTexture, texCoord).rgb,  texture(u_forwardTexture, texCoord).a );
		//texColour =  texture(u_forwardTexture, texCoord).rgb;
	}

	if(forwardDepth > 0.9999f && inputDepth > 0.9999f)
	{
		texColour = texture(u_inputTexture, texCoord).rgb;
		//texColour = vec3(0.0, 0.25, 0.75);
	}

	
	//texColour = aces(texColour); //tonemap
	//texColour = pow(texColour, vec3(1.0 / 2.2)); //gamma correct
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
