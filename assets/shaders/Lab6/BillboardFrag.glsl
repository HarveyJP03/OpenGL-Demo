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

uniform sampler2D u_billboardImage;

void main()
{	
	vec3 result = vec3(0.0, 0.0, 0.0); 
	normal = normalize(vertexNormal) ;
	vec4 colour = texture(u_billboardImage, texCoord);
	
	if(colour.rgb == vec3(0.0, 0.0, 0.0)) discard;

	//float luminance = 0.2126 * colour.r + 0.7152 * colour.g + 0.0722 * colour.b;
	//vec3 tintedCol = mix(colour.rgb, colour.rgb * vec3(1.0, 1.0, 1.0), luminance);
	      
	gPosition = vec4(fragmentPos, 1.0);
	gNormal = vec4(normal, 1.0);
	gDiffSpec = colour;
	gSpec = vec4(specularStrength);
}

