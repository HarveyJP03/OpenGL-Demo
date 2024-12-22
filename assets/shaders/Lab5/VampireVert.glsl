#version 460 core
			
layout(location = 0) in vec3 a_vertexPosition;
layout(location = 1) in vec3 a_vertexNormal;
layout(location = 2) in vec2 a_texCoord;
layout(location = 3) in vec3 a_tangent;


layout (std140, binding = 1) uniform b_camera
{
	uniform mat4 u_view;
	uniform mat4 u_projection;
	uniform vec3 u_viewPos;
};

out vec3 fragmentPos;
out vec2 texCoord;
out mat3 TBN; //matrix to change tangents from tangent space to world space (in line with object surface)
out vec4 fragmentPosLightSpace;

uniform mat4 u_model;

uniform mat4 u_lightSpaceTransform;

void main()
{
	fragmentPos = vec3(u_model * vec4(a_vertexPosition, 1.0));
	 
	vec3 vertexNormal = normalize(mat3(transpose(inverse(u_model))) * a_vertexNormal);
	texCoord = a_texCoord;
	gl_Position = u_projection * u_view * vec4(fragmentPos,1.0);

	vec3 T = (u_model * vec4(a_tangent, 0.0)).xyz; //Convert tangent to model space, so its in correct cordiante system (but still not facing right way)
	T = normalize(T - dot(T, vertexNormal) * vertexNormal); //Make sure TBN is still orthogonal, since tangents are averaged they TBN can be slightly off
	vec3 B = normalize(cross(vertexNormal, T));
	TBN = mat3(T, B, vertexNormal);
	
	//Shadow Mapping
	fragmentPosLightSpace = u_lightSpaceTransform * vec4(fragmentPos, 1.0);

}
