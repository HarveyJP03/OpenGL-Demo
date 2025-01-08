#version 460 core

layout(location = 0) out vec4 colour;

in vec2 texCoord;

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

uniform vec3 u_viewPos;

uniform sampler2D u_albedoMap;
uniform vec3 u_albedo;

uniform sampler2D u_specularMap;
uniform sampler2D u_normalMap;

vec3 getDirectionalLight() ;
vec3 getPointLight(int idx) ;
vec3 getSpotLight(int idx) ;

uniform sampler2D u_positionTexture;
uniform sampler2D u_normalTexture;
uniform sampler2D u_albedoTexture;
uniform sampler2D u_specularTexture;

uniform sampler2D u_skyBoxTexture;
uniform sampler2D u_depthTexture;


//global vars
vec3 normal ;
vec3 fragmentPos;
vec3 albedoColour ;
float specularStrength  = 0.0f ;
vec3 viewDir ;

float ShadowCalculation();

uniform mat4 u_lightSpaceTransform;
uniform sampler2D u_shadowMap;

void main()
{	
	fragmentPos = texture(u_positionTexture, texCoord).rgb;
	normal = texture(u_normalTexture, texCoord).rgb;
	albedoColour = texture(u_albedoTexture, texCoord).rgb;
	specularStrength = texture(u_specularTexture, texCoord).r;
	
	vec3 result = vec3(0.0, 0.0, 0.0); 
    viewDir = normalize(u_viewPos - fragmentPos);

	// light casters
	result += getDirectionalLight();
	
	for(int i = 0; i < numPointLights; i++)
	{
		result += getPointLight(i);
	}
	
	for(int i = 0; i < numSpotLights; i++)
	{
		//result += getSpotLight(i);
	}

	float depth = texture(u_depthTexture, texCoord).r;
	if(depth >= 0.9999f)
	{
		colour = vec4(texture(u_skyBoxTexture, texCoord).rgb, 1.0);
	}
	else
	{
		colour = vec4(result, 1.0) * vec4(albedoColour, 1.0);
	}
	      
}

float ShadowCalculation()
{
	//Perspective divide
	vec4 fragmentPosLightSpace = u_lightSpaceTransform * vec4(fragmentPos, 1.0);
	vec3 projCoords = fragmentPosLightSpace.xyz / fragmentPosLightSpace.w;

	//Map to [0,1]
	projCoords = projCoords * 0.5 + 0.5;

	float closestDepth = texture(u_shadowMap, projCoords.xy).r;
	float currentDepth = projCoords.z;

	float shadow = 0.0f;
	float bias = 0.0015f;

	vec2 texelSize = 1.0 / textureSize(u_shadowMap, 0);
	float samplesTaken = 0.0f;
	for(int x = -1; x <= 1; ++x)
	{
		for(int y = -1; y <= 1; ++y)
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
	float ambientStrength = 0.1;
	vec3 ambient = ambientStrength * dLight.colour;

	float diffuseFactor = max(dot(normal, -dLight.direction), 0.0);
	vec3 diffuse = diffuseFactor * dLight.colour;
	
	vec3 H = normalize(-dLight.direction + viewDir );
	float specularFactor = pow(max(dot(normal, H) , 0.0), 64);
    vec3 specular = dLight.colour * specularFactor * specularStrength;

	float shadowAmount = ShadowCalculation();

	return ambient +(1.0 - shadowAmount)  * (diffuse + specular);
	//return ambient + diffuse + specular;
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
	float specularFactor = pow(max(dot(normal, H) , 0.0), 128) ;
	vec3 specular = pLights[idx].colour * specularFactor * specularStrength;
	specular = specular * attn;
	
	return ambient + diffuse + specular;
}

vec3 getSpotLight(int idx)
{	
	vec3 lightDir = normalize(sLights[idx].position - fragmentPos);
	float theta = dot(lightDir, normalize(-sLights[idx].direction));
	float ambientStrength = 0.0 ;
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

