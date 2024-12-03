#version 460 core

layout(location = 0) out vec4 ppColour;

in vec2 texCoord;
in vec2 screenSize;

uniform sampler2D u_inputTexture;
uniform vec2 u_screenSize;
uniform float u_blurRadius;


void main()
{
	vec2 texelSize = 1.0 / u_screenSize;
	
	vec3 colour = vec3(0.0f);
	float samplesTaken = 0.0f;

	//Go through texels in a 3x3 box, average the colour
	for(int x = -1; x <= 1; ++x)
	{
		for(int y = -1; y <= 1; ++y)
		{
			vec2 offset = vec2(float(x), float(y)) * texelSize * u_blurRadius; //Tex coord offset to get neighbouring texel
			vec3 sampleColour = texture(u_inputTexture, texCoord + offset).rgb; //read neighbouring texel using offset
			colour += sampleColour;
			samplesTaken++;
		}
	}

	colour /= samplesTaken;
	ppColour = vec4(colour, 1.0);
}

