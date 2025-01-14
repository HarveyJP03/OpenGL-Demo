#version 460 core

layout(local_size_x = 16, local_size_y = 16) in; //512x512 image, 32x32 work groups, so each work group should be 16x16.
layout(binding = 0, rgba8) uniform image2D outputImg;

void main()
{
	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy); //ID of the work item(thread) in relation to the whole dispatch, not just the work group
	vec4 pixelCol = vec4(0.0, 0.0, 0.0, 1.0);

	if((gl_WorkGroupID.x + gl_WorkGroupID.y) % 2 == 0  )
	{
		pixelCol = vec4(1.0, 1.0, 1.0, 1.0);
	}
	imageStore(outputImg, pixel_coords, pixelCol); //write color to image at pixel coords
}