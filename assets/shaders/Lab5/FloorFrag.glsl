#version 460 core

layout(location = 0) out vec4 gPosition;
layout(location = 1) out vec4 gNormal;
layout(location = 2) out vec4 gDiffSpec;
layout(location = 3) out vec4 gSpec;

uniform vec3 u_albedo;


in vec3 fragmentPos;
in vec3 vertexNormal;
in vec2 texCoord;
in float heightScale;

//global vars
vec3 normal ;
float specularStrength  = 0.25f ;

void main()
{	
	vec3 result = vec3(0.0, 0.0, 0.0); 
	normal = normalize(vertexNormal) ;

	vec3 colour;

	float mid = (heightScale * 0.5f);
	vec3 col1 = vec3(0.2, 0.6, 0.1);
	vec3 col2 = vec3(0.6, 0.2, 0.1);

	colour = mix(col1, col2, smoothstep(0.0f, mid + mid, fragmentPos.y));

	      
	gPosition = vec4(fragmentPos, 1.0);
	gNormal = vec4(normal, 0.0);
	gDiffSpec = vec4( colour, 1.0);
	gSpec = vec4(specularStrength);
}


