#version 460 core
			
layout(vertices = 3) out; //triangle

in vec3 vs_fragmentPos[];
in vec2 vs_texCoord[];

out vec3 tcs_fragmentPos[];
out vec2 tcs_texCoord[];

layout (std140, binding = 1) uniform b_camera
{
	uniform mat4 u_view;
	uniform mat4 u_projection;
	uniform vec3 u_viewPos;
};

uniform float u_tessLevel;

float GetTessLevel(float Distance0, float Distance1);

void main()
{
	//Tess level is per patch, but this runs per control point(vertex) so just set it on the first run through to avoid rewriting same thing
	if(gl_InvocationID == 0)
	{
		float eyeToVertexDist0 = distance(u_viewPos, vs_fragmentPos[0]);
		float eyeToVertexDist1 = distance(u_viewPos, vs_fragmentPos[1]);
		float eyeToVertexDist2 = distance(u_viewPos, vs_fragmentPos[2]);

		vec3 centrePoint = (vs_fragmentPos[0] + vs_fragmentPos[1] + vs_fragmentPos[2]) / 3;
		float centreDist = distance(u_viewPos, centrePoint);
		
		gl_TessLevelOuter[0] = GetTessLevel(eyeToVertexDist1, eyeToVertexDist2);
		gl_TessLevelOuter[1] = GetTessLevel(eyeToVertexDist2, eyeToVertexDist0);
		gl_TessLevelOuter[2] = GetTessLevel(eyeToVertexDist0, eyeToVertexDist1);
		gl_TessLevelInner[0] = GetTessLevel(centreDist, centreDist);
	}

	tcs_fragmentPos[gl_InvocationID] = vs_fragmentPos[gl_InvocationID];
	tcs_texCoord[gl_InvocationID] = vs_texCoord[gl_InvocationID]; //pass through vertex info unchanged to evaluation shader
}

float GetTessLevel(float Distance0, float Distance1)
{
	float averageDistance = (Distance0 + Distance1) / 2.0;

	//Linear
	float a = 1.0f;
	float b = 50.0f;
	float linear = -a * averageDistance + b;
	if(linear < 1) linear = 1.0f;
	//return linear;

	//Step
	if(averageDistance >= u_tessLevel)
	{
		//return 1.0f;
	}
	else if( averageDistance > u_tessLevel / 2.0f )
	{
		//return 8.0f;
	}
	else
	{
		//return 64.0f;
	}

	//Exponential
	float tess = exp((averageDistance - 50.0f) * -0.1f);
	return tess;


}