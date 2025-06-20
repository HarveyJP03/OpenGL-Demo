#version 460 core
			
layout(location = 0) in vec3 a_vertexPosition;
layout(location = 1) in vec2 a_texCoord;


layout (std140, binding = 1) uniform b_camera
{
	uniform mat4 u_view;
	uniform mat4 u_projection;
	uniform vec3 u_viewPos;
};

uniform mat4 u_lightSpaceTransform;

out vec3 fragmentPos;
out vec4 fragPosClipSpace;
out vec3 vertexNormal;
out vec2 texCoord;
out vec4 fragmentPosLightSpace;


uniform mat4 u_model;

void main()
{
	fragmentPos = vec3(u_model * vec4(a_vertexPosition, 1.0));
	fragPosClipSpace = u_projection * u_view * u_model * vec4(a_vertexPosition, 1.0);
	vertexNormal = vec3(0.0f, 1.0f, 0.0f);
	texCoord = a_texCoord;
	gl_Position = u_projection * u_view * vec4(fragmentPos,1.0);

	//Shadow Mapping
	fragmentPosLightSpace = u_lightSpaceTransform * vec4(fragmentPos, 1.0);

}