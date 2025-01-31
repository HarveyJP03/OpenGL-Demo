#version 460 core

layout(location = 0) out vec4 ppColour;

in vec2 texCoord;

uniform sampler2D u_inputTexture;
uniform sampler2D u_forwardTexture;

uniform sampler2D u_inputDepthTexture;
uniform sampler2D u_forwardDepthTexture;

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
		//Becuase of the fact I need to do two seperate passes with two seperate scenes, the pixels behind forward render for transparency are black, 
		//since its a seperate scene, so manually blend here with deffered scene texture 
		texColour = mix(texture(u_inputTexture, texCoord).rgb, texture(u_forwardTexture, texCoord).rgb,  texture(u_forwardTexture, texCoord).a );
	}

	//Nothing here, just display skybox from deffered rendering scene
	if(forwardDepth > 0.9999f && inputDepth > 0.9999f)
	{
		texColour = texture(u_inputTexture, texCoord).rgb;
	}

	ppColour = vec4(texColour, 1.0f);
}
