#version 460 core

layout(location = 0) out vec4 gPosition;
layout(location = 1) out vec4 gNormal;
layout(location = 2) out vec4 gDiffSpec;
layout(location = 3) out vec4 gSpec;

in vec3 fragmentPos;
in vec2 texCoord;
in mat3 TBN;

uniform sampler2D u_albedoMap;

uniform sampler2D u_specularMap;
uniform sampler2D u_normalMap;


//global vars
vec3 normal ;
vec3 albedoColour ;
float specularStrength;


void main()
{	
	albedoColour = texture(u_albedoMap, texCoord).rgb;
	specularStrength = texture(u_specularMap, texCoord).r;

	vec3 n = texture(u_normalMap, texCoord).rgb;
	n = n * 2.0 - 1.0;
	n = normalize(TBN * n);
	normal = n;
 
	gPosition = vec4(fragmentPos, 1.0);
	gNormal = vec4(normal, 0.0);
	gDiffSpec = vec4(albedoColour, specularStrength);
	gSpec = vec4(specularStrength);
}


