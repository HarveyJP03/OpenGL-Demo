#version 460 core

layout(location = 0) out vec4 ppColour;

in vec2 texCoord;

uniform sampler2D u_inputTexture;
uniform sampler2D u_depthTexture;
uniform float u_expSquared;

float LineariseDepth(float depth);

float near = 0.01f;
float far = 1000.0f;

float fogFactor = 0.0f;

void main()
{
	vec3 colour = texture(u_inputTexture, texCoord).rgb;
	float depth = texture(u_depthTexture, texCoord).r;

	vec3 fogColour = vec3(1.0f);
	float linearDepth = LineariseDepth(depth);

	float fogDensity = 0.1f;
	if( u_expSquared == 0)
	{
		float depthNormalised = (linearDepth - near) / (far - near); 
		fogFactor = depthNormalised;
	}
	else if(u_expSquared == 1)
	{
		fogFactor = 1.0 - exp(-fogDensity * linearDepth); //We invert here, since without doing this FogFactor is higher the closer to the camera we are
														  // exp is like curve on graph, starts off faster then slows down the larger depth is (fog scales with depth slower the higher depth is)
	}
	else if(u_expSquared == 2)
	{
		fogFactor = 1.0 - exp(-fogDensity * linearDepth * fogDensity * linearDepth); //Starts of slower than regular exp, but once its going fog scales with depth more strongly for longer before starting to scale slower
	}

	
	ppColour = vec4(mix(colour, fogColour, fogFactor), 1.0f);
	
}

float LineariseDepth(float depth)
{
	float z = depth * 2.0 - 1.0; //Back to clip space [-1, 1] from Normalised Device Coorinates [0,1]
	return (2.0 * near * far) / (far + near - z * (far - near));
}
