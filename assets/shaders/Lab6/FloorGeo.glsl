#version 460 core

layout(triangles) in ;
layout(triangle_strip, max_vertices = 3) out ;

in vec3 vs_fragmentPos[]; //Array as this takes in the primitive 3 points
in vec2 vs_texCoord[];

out vec3 fragmentPos;
out vec3 vertexNormal;
out vec2 texCoord;

vec3 getNormal();

void main()
{
	for(int i = 0; i < 3; i++) //Loop through vertices, and pass them through to frag shader unchanged
	{
		texCoord = vs_texCoord[i];
		fragmentPos = vs_fragmentPos[i];
		vertexNormal = getNormal();
		gl_Position = gl_in[i].gl_Position; //gl_in stores gl_Position o each vertex (built in)
		
		EmitVertex(); //Finished with vertex, emmitting it down the pipeline
	}
	EndPrimitive();
}

vec3 getNormal()
{
	vec3 a = vs_fragmentPos[0] - vs_fragmentPos[1];
	vec3 b = vs_fragmentPos[0] - vs_fragmentPos[2];
	return normalize(cross(a,b)); //Vector perpendicular to two line of triangle
}
