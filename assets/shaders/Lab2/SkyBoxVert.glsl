#version 460 core
			
layout(location = 0) in vec3 a_vertexPosition;

layout (std140, binding = 1) uniform b_camera
{
	uniform mat4 u_view;
	uniform mat4 u_projection;
	uniform vec3 u_viewPos;
};

out vec3 fragmentPos;
out vec3 vertexNormal;
out vec3 texCoords;

uniform mat4 u_model;

uniform mat4 u_skyBoxView;

void main()
{
	texCoords = a_vertexPosition;
	gl_Position = u_projection * u_skyBoxView * u_model * vec4(a_vertexPosition, 1.0);

}