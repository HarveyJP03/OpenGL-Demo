#version 460 core

layout(location = 0) out vec4 colour;

uniform sampler2D u_particleImage;
uniform vec3 u_particleColour;

in vec2 texCoord;
in vec4 fpos;

float remap(float currValue, float  inMin,float inMax,float outMin, float outMax);

void main()
{	
	colour = texture(u_particleImage, texCoord);
	if(colour.a <= 0.0f) discard;
	
	colour.a -= remap(fpos.w, 5.0, 0.0, 0.0, 1.0);
	colour.a = clamp(colour.a, 0.0f, 1.0f);
	colour.rgb *= u_particleColour;

}

float remap(float currValue, float  inMin,float inMax,float outMin, float outMax) 
{
	float t = (currValue - inMin) / (inMax - inMin);
	return mix(outMin, outMax, t);
}