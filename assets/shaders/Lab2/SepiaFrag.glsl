#version 460 core

layout(location = 0) out vec4 ppColour;

in vec2 texCoord;

uniform sampler2D u_inputTexture;

void main()
{
	vec3 colour = texture(u_inputTexture, texCoord).rgb;
	
	vec3 sepia;
	sepia.r = dot(colour, vec3(0.393, 0.769, 0.189)); //How close is our colour to this sepia value? 
	sepia.g = dot(colour, vec3(0.394, 0.686, 0.168)); //This determines how string the colour is
	sepia.b = dot(colour, vec3(0.272, 0.534, 0.131));

	ppColour = vec4(sepia, 1.0);
}