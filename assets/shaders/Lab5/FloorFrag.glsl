#version 460 core

layout(location = 0) out vec4 gPosition;
layout(location = 1) out vec4 gNormal;
layout(location = 2) out vec4 gDiffSpec;
layout(location = 3) out vec4 gSpec;


in vec3 vertexNormal;
in vec3 fragmentPos;
in vec2 texCoord;
in vec4 fragmentPosLightSpace;


struct directionalLight
{
	vec3 colour;
	vec3 direction;
};

struct pointLight
{
	vec3 colour;
	vec3 position;
	vec3 constants;
};

struct spotLight
{
	vec3 colour;
	vec3 position;
	vec3 direction;
	vec3 constants;
	float cutOff;
	float outerCutOff;
};

const int numPointLights = 30;
const int numSpotLights = 1;

layout (std140, binding = 2) uniform b_lights
{
	uniform directionalLight dLight;
	uniform pointLight pLights[numPointLights];
	uniform spotLight sLights[numSpotLights];
};

layout (std140, binding = 1) uniform b_camera
{
	uniform mat4 u_view;
	uniform mat4 u_projection;
	uniform vec3 u_viewPos;
};


uniform sampler2D u_albedoMap;
uniform vec3 u_albedo;

uniform sampler2D u_shadowMap;

vec3 getDirectionalLight() ;
vec3 getPointLight(int idx) ;
vec3 getSpotLight(int idx) ;


//global vars
vec3 normal ;
vec3 albedoColour ;
float specularStrength  = 0.8 ;
vec3 viewDir ;


bool doesPassDepthTest();
float ShadowCalculation();


void main()
{	
	vec3 result = vec3(0.0, 0.0, 0.0); 
    viewDir = normalize(u_viewPos - fragmentPos);
	normal = normalize(vertexNormal) ;
	albedoColour = texture(u_albedoMap, texCoord).rgb;

	// light casters

	result += getDirectionalLight();
	
	for(int i = 0; i <numPointLights; i++)
	{
		result += getPointLight(i);
	}
	
	for(int i = 0; i <numSpotLights; i++)
	{
		//result += getSpotLight(i);
	}
	      
	gPosition = vec4(fragmentPos, 1.0);
	gNormal = vec4(normal, 0.0);
	gDiffSpec = vec4( u_albedo, 1.0);
	gSpec = vec4(0.0f);
}

float ShadowCalculation()
{
	//Perspective divide
	vec3 projCoords = fragmentPosLightSpace.xyz / fragmentPosLightSpace.w;

	//Map to [0,1]
	projCoords = projCoords * 0.5 + 0.5;

	float closestDepth = texture(u_shadowMap, projCoords.xy).r;
	float currentDepth = projCoords.z;

	float shadow = 0.0f;
	float bias = 0.0f;

	vec2 texelSize = 1.0 / textureSize(u_shadowMap, 0);
	float samplesTaken = 0.0f;
	for(int x = -2; x <= 2; ++x)
	{
		for(int y = -2; y <= 2; ++y)
		{
			float texelDepth = texture(u_shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
			shadow += currentDepth - bias > texelDepth ? 1.0 : 0.0;
			samplesTaken++;
		}
	}

	shadow /= samplesTaken;


	//if(currentDepth - bias > closestDepth) shadow = 1.0f;
	return shadow;
}


vec3 getDirectionalLight()
{
	float ambientStrength = 0.0;
	vec3 ambient = ambientStrength * dLight.colour ;

	float diffuseFactor = max(dot(normal, -dLight.direction), 0.0);
	vec3 diffuse = diffuseFactor * dLight.colour ;
	
	vec3 H = normalize(-dLight.direction + viewDir ) ;
	float specularFactor = pow(max(dot(normal, H) , 0.0), 64) ;
    vec3 specular = dLight.colour * specularFactor * specularStrength;

	float shadowAmount = ShadowCalculation();
	return ambient +(1.0 - shadowAmount) * (diffuse + specular);
}

vec3 getPointLight(int idx)
{
	float ambientStrength = 0.0;
	vec3 ambient = ambientStrength * pLights[idx].colour;

	float distance = length(pLights[idx].position - fragmentPos);
	float attn = 1.0/(pLights[idx].constants.x + (pLights[idx].constants.y* distance) + (pLights[idx].constants.z * (distance * distance)));
	vec3 lightDir = normalize(pLights[idx].position - fragmentPos);

	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = diff * attn * pLights[idx].colour;

	vec3 H = normalize(lightDir + viewDir ) ;
	float specularFactor = pow(max(dot(normal, H) , 0.0), 2) ;
	vec3 specular = pLights[idx].colour * specularFactor * specularStrength;
	specular = specular * attn;
	
	return ambient + diffuse + specular;
}

vec3 getSpotLight(int idx)
{	
	vec3 lightDir = normalize(sLights[idx].position - fragmentPos);
	float theta = dot(lightDir, normalize(-sLights[idx].direction));
	float ambientStrength = 0.4 ;
	vec3 ambient = ambientStrength * sLights[idx].colour;

	if(theta > sLights[idx].outerCutOff)
	{

		float distance = length(sLights[idx].position - fragmentPos);
		float attn = 1.0/(sLights[idx].constants.x + (sLights[idx].constants.y* distance) + (sLights[idx].constants.z * (distance * distance)));
		float diff = max(dot(normal, lightDir), 0.0);
		vec3 diffuse = diff * attn * sLights[idx].colour;

		float specularStrength = 0.8;

		vec3 H = normalize(-lightDir + viewDir ) ;
	    float specularFactor = pow(max(dot(normal, H) , 0.0), 64) ;
        vec3 specular = pLights[idx].colour * specularFactor * specularStrength; 
	
		float epsilon = sLights[idx].cutOff - sLights[idx].outerCutOff;
		float intensity = clamp((theta - sLights[idx].outerCutOff) / epsilon, 0.0, 1.0); 
	
		diffuse  *= intensity;
		specular *= intensity;
		
		return (ambient + diffuse + specular);
	}
	else
	{
		return ambient;
	}
}

