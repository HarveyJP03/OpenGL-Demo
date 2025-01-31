#version 460 core

layout(local_size_x = 16, local_size_y = 16) in; //1024x1024 image, 64x64 work groups, so each work group should be 16x16.
layout(binding = 0, rgba8) uniform image2D outputImg;

uniform sampler2D u_heightMap;

void main()
{
	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy); //ID of the work item(thread) in relation to the whole dispatch, not just the work group
	vec2 uv = vec2(pixel_coords) / vec2(1024);
	vec4 pixelCol = texture2D(u_heightMap, uv);

	float height = texture(u_heightMap, uv).r;
	float heightScale = 10.0f;

	//Multiply by 8 aswell as heigh scale, as otherwise the differene in height between texels is too small
	float right = textureOffset(u_heightMap, uv, ivec2(1,0)).r * (heightScale * 8.0f);
	float left = textureOffset(u_heightMap, uv, ivec2(-1,0)).r * (heightScale * 8.0f);
	float up = textureOffset(u_heightMap, uv, ivec2(0,1)).r * (heightScale * 8.0f);
	float down = textureOffset(u_heightMap, uv, ivec2(0,-1)).r * (heightScale * 8.0f);

	float lr = left - right;
	float du = down - up;
	vec4 tes_CDMnormal = vec4(normalize(vec3(lr, 2.0, du)), pixelCol.r);

	imageStore(outputImg, pixel_coords, tes_CDMnormal); //write color to image at pixel coords
}