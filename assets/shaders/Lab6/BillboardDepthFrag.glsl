#version 460 core

uniform sampler2D u_billboardImage;

in vec2 texCoord;

void main()
{	
	vec4 colour = texture(u_billboardImage, texCoord);	
	if(colour.rgb == vec3(0.0, 0.0, 0.0)) discard;
}