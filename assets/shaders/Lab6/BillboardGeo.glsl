#version 460 core

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

layout (std140, binding = 1) uniform b_camera
{
	uniform mat4 u_view;
	uniform mat4 u_projection;
	uniform vec3 u_viewPos;
};

out vec2 texCoord;
out vec3 vertexNormal;
out vec3 fragmentPos;

void main()
{
	vec3 pos = gl_in[0].gl_Position.xyz; //Get position of point
	fragmentPos = pos;
	vec3 toCamera = normalize(u_viewPos - pos); //From camera to billboard position
	vec3 up = vec3(0.0f, 1.0f, 0.0f);
	vec3 right = normalize(cross(toCamera, up)); //Perpendicular to the up vector and the vector from camera to point

	float size = 12.0f;
	vec3 corner0 = pos - right * size - up * size; //Bottom Left
	vec3 corner1 = pos + right * size - up * size; //Bottom Right
	vec3 corner2 = pos - right * size + up * size; //Top-Left
	vec3 corner3 = pos + right * size + up * size; //Top-Right

	mat4 VP = u_projection * u_view;
	vertexNormal = toCamera;

	//Emit Vertices
	texCoord = vec2(0.0, 0.0);
	gl_Position = VP * vec4(corner0, 1.0);
	EmitVertex();

	texCoord = vec2(1.0, 0.0);
	gl_Position = VP * vec4(corner1, 1.0);
	EmitVertex();

	texCoord = vec2(0.0, 1.0);
	gl_Position = VP * vec4(corner2, 1.0);
	EmitVertex();

	texCoord = vec2(1.0, 1.0);
	gl_Position = VP * vec4(corner3, 1.0);
	EmitVertex();

	EndPrimitive();

}
