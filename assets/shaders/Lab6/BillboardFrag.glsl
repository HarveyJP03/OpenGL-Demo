#version 460 core

layout(location = 0) out vec4 gPosition;
layout(location = 1) out vec4 gNormal;
layout(location = 2) out vec4 gDiffSpec;
layout(location = 3) out vec4 gSpec;

in vec3 fragmentPos;
in vec3 vertexNormal;
in vec2 texCoord;

//global vars
vec3 normal ;
float specularStrength  = 0.25f ;

void main()
{	
	vec3 result = vec3(0.0, 0.0, 0.0); 
	normal = normalize(vertexNormal) ;
	      
	gPosition = vec4(fragmentPos, 1.0);
	gNormal = vec4(normal, 0.0);
	gDiffSpec = vec4( 1.0, 0.0, 1.0, 1.0);
	gSpec = vec4(specularStrength);
}

