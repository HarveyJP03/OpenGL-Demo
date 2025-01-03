#version 460 core
			
layout(location = 0) in vec3 a_vertexPosition;
layout(location = 1) in vec2 a_texCoord;


layout (std140, binding = 1) uniform b_camera
{
	uniform mat4 u_view;
	uniform mat4 u_projection;
	uniform vec3 u_viewPos;
};

out vec3 vs_fragmentPos;
out vec3 vs_vertexNormal;
out vec2 vs_texCoord;

uniform mat4 u_model;

void main()
{
	vs_fragmentPos = vec3(u_model * vec4(a_vertexPosition, 1.0));
	vs_vertexNormal = vec3(0.0f, 1.0f, 0.0f);
	vs_texCoord = a_texCoord;
	gl_Position = u_projection * u_view * vec4(vs_fragmentPos,1.0);
}