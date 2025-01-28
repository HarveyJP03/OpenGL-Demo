#version 460 core

layout(location = 0) out vec4 gPosition;
layout(location = 1) out vec4 gNormal;
layout(location = 2) out vec4 gDiffSpec;
layout(location = 3) out vec4 gSpec;

uniform sampler2D u_terrainTexture;
uniform sampler2D u_terrainGrassTexture;

in vec3 fragmentPos;
in vec3 vertexNormal;
in vec2 texCoord;
in float heightScale;

uniform vec2 u_screenSize;
uniform float u_remapRange;

//global vars
vec3 normal ;
float specularStrength  = 0.25f ;

float remap(float currValue, float  inMin,float inMax,float outMin, float outMax);

void main()
{	
	normal = normalize(vertexNormal) ;

	vec3 colour;

	float mid = heightScale * 0.5f;
	vec3 col1 = vec3(0.36078f, 0.70980f, 0.88235f);
	vec3 col2 = vec3(0.25490f, 0.41961f, 0.87843f);

	//tri plannar
	vec3 blendPercent = abs(normalize((vertexNormal)));
	blendPercent = blendPercent / (blendPercent.x + blendPercent.y + blendPercent.z);

	vec2 texelSize = 1.0 / u_screenSize;

	float fragPosxRemap = remap(fragmentPos.x, -u_remapRange, u_remapRange, 0, 1 );
	float fragPosyRemap = remap(fragmentPos.y, -u_remapRange, u_remapRange, 0, 1 );
	float fragPoszRemap = remap(fragmentPos.z, -u_remapRange, u_remapRange, 0, 1 );

	vec4 xaxis = texture2D(u_terrainTexture, vec2(fragPosyRemap, fragPoszRemap));
	vec4 yaxis = texture2D(u_terrainTexture, vec2(fragPosxRemap, fragPoszRemap));
	vec4 zaxis = texture2D(u_terrainTexture, vec2(fragPosxRemap, fragPosyRemap));

	vec4 tpTexTerrain = xaxis*blendPercent.x + yaxis*blendPercent.y + zaxis*blendPercent.z;

	vec4 xaxisG = texture2D(u_terrainGrassTexture, vec2(fragPosyRemap, fragPoszRemap));
	vec4 yaxisG = texture2D(u_terrainGrassTexture, vec2(fragPosxRemap, fragPoszRemap));
	vec4 zaxisG = texture2D(u_terrainGrassTexture, vec2(fragPosxRemap, fragPosyRemap));
	vec4 tpGrassTerrain = xaxisG*blendPercent.x + yaxisG*blendPercent.y + zaxisG*blendPercent.z;

	col1 = tpGrassTerrain.rgb;
	col2 = tpTexTerrain.rgb;

	colour = mix(col1, col2, smoothstep(0.0f, mid + mid * 0.75f, fragmentPos.y));

	gPosition = vec4(fragmentPos, 1.0);
	gNormal = vec4(normal, 1.0);
	gDiffSpec = vec4( colour, 1.0f);
	gSpec = vec4(specularStrength);
}

float remap(float currValue, float  inMin,float inMax,float outMin, float outMax) 
{
	float t = (currValue - inMin) / (inMax - inMin);
	return mix(outMin, outMax, t);
}


