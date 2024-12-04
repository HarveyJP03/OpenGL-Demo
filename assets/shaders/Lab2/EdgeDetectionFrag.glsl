#version 460 core

layout(location = 0) out vec4 ppColour;

in vec2 texCoord;

uniform sampler2D u_inputTexture;
uniform vec2 u_screenSize;

bool isEdgeOfScreen();

void main()
{
	vec2 inverseScreen = 1.0 / u_screenSize; //Size of a pixel
	
	vec3 colour = texture(u_inputTexture, texCoord).rgb;
	vec3 colourLeft = texture(u_inputTexture, texCoord + vec2(-1.0, 0.0) * inverseScreen).rgb; //Get colour of pixel to the left of current etc etc
	vec3 colourRight = texture(u_inputTexture, texCoord + vec2(1.0, 0.0) * inverseScreen).rgb;
	vec3 colourUp = texture(u_inputTexture, texCoord + vec2(0.0, 1.0) * inverseScreen).rgb;
	vec3 colourDown = texture(u_inputTexture, texCoord + vec2(0.0, -1.0) * inverseScreen).rgb;

	//Luminance values (how close to the brightest possible colour is it)
	float lumCenter = dot(colour, vec3(0.2126, 0.7152, 0.0722));
	float lumLeft = dot(colourLeft, vec3(0.2126, 0.7152, 0.0722));
	float lumRight = dot(colourRight, vec3(0.2126, 0.7152, 0.0722));
	float lumUp = dot(colourUp, vec3(0.2126, 0.7152, 0.0722));
	float lumDown = dot(colourDown, vec3(0.2126, 0.7152, 0.0722));

	float edgeHorizontal = abs(lumLeft - lumRight); //Difference in brightness of texels
	float edgeVertical = abs(lumUp - lumDown); //Abs makes the number not negative
	float edgeStrength = max(edgeHorizontal, edgeVertical); //edgeStrength = largest from two values

	if(edgeStrength > 0.25f) //GL_CLAMP_TO_EDGE in texture.cpp init fixes, may need undoing if other textures break
	{
		colour = vec3(0.0f);
	}

	ppColour = vec4(colour, 1.0);
}

//If pixel is on the edge of screen, dont apply edgeStrength coluring, since I assume the UV wraps and assumes pixel at the top of screen is one above one at bottom
bool isEdgeOfScreen()
{
	if(texCoord.y >= 0.001f && texCoord.y <= 0.999f && texCoord.x >= 0.001f && texCoord.x <= 0.999f)
	{
		return false;
	}
	return true;
}