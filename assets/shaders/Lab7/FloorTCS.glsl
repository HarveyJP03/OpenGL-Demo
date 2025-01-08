#version 460 core
			
layout(vertices = 3) out; //triangle

in vec3 vs_fragmentPos[];
in vec2 vs_texCoord[];

out vec3 tcs_fragmentPos[];
out vec2 tcs_texCoord[];

void main()
{
	//Tess level is per patch, but this runs per control point(vertex) so just set it on the first run through to avoid rewriting same thing
	if(gl_InvocationID == 0)
	{
		float tessLevel = 1.0f;
		gl_TessLevelOuter[0] = tessLevel;
		gl_TessLevelOuter[1] = tessLevel;
		gl_TessLevelOuter[2] = tessLevel;
		gl_TessLevelInner[0] = tessLevel;
	}

	tcs_fragmentPos[gl_InvocationID] = vs_fragmentPos[gl_InvocationID];
	tcs_texCoord[gl_InvocationID] = vs_texCoord[gl_InvocationID]; //pass through vertex info unchanged to evaluation shader
}