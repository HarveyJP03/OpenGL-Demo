#version 460 core
			
layout(triangles, fractional_odd_spacing, ccw) in;

in vec3 tcs_fragmentPos[];
in vec2 tcs_texCoord[];

out vec3 tes_fragmentPos;
out vec2 tes_texCoord;

vec2 interpolate2D(vec2 v0, vec2 v1, vec2 v2);
vec3 interpolate3D(vec3 v0, vec3 v1, vec3 v2);

void main()
{
	tes_fragmentPos = interpolate3D(tcs_fragmentPos[0], tcs_fragmentPos[1], tcs_fragmentPos[2]);
	tes_texCoord = interpolate2D(tcs_texCoord[0], tcs_texCoord[1], tcs_texCoord[2]);
}

vec2 interpolate2D(vec2 v0, vec2 v1, vec2 v2)
{
	return vec2(gl_TessCoord.x) * v0 + vec2(gl_TessCoord.y) * v1 + vec2(gl_TessCoord.z) * v2;
}

vec3 interpolate3D(vec3 v0, vec3 v1, vec3 v2)
{
	return vec3(gl_TessCoord.x) * v0 + vec3(gl_TessCoord.y) * v1 + vec3(gl_TessCoord.z) * v2;
}

