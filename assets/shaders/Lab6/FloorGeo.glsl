#version 460 core

layout(triangles) in ;
layout(triangle_strip, max_vertices = 3) out ;

in vec3 tes_fragmentPos[]; //Array as this takes in the primitives 3 points
in vec2 tes_texCoord[];
in vec3 tes_CDMnormal[];
in float tes_heightScale[];

uniform float u_geoNormal;

out vec3 fragmentPos;
out vec3 vertexNormal;
out vec2 texCoord;
out float heightScale;

layout (std140, binding = 1) uniform b_camera
{
	uniform mat4 u_view;
	uniform mat4 u_projection;
	uniform vec3 u_viewPos;
};

vec3 getNormal();

void main()
{
	heightScale = tes_heightScale[0];
	for(int i = 0; i < 3; i++) //Loop through vertices, and pass them through to frag shader unchanged
	{
		texCoord = tes_texCoord[i];
		fragmentPos = tes_fragmentPos[i];
		vertexNormal = (u_geoNormal == 0) ?  getNormal() : tes_CDMnormal[i]; //CDM or per surface norms?
		gl_Position = u_projection * u_view * vec4(fragmentPos, 1.0);
		
		EmitVertex();
	}
	EndPrimitive();
}

vec3 getNormal()
{
	vec3 a = tes_fragmentPos[0] - tes_fragmentPos[1];
	vec3 b = tes_fragmentPos[0] - tes_fragmentPos[2];
	return normalize(cross(a,b)); //Vector perpendicular to two line of triangle
}
