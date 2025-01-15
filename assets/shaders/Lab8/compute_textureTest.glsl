#version 460 core

layout(local_size_x = 16, local_size_y = 16) in; //512x512 image, 32x32 work groups, so each work group should be 16x16. 8 Warps per work group
layout(binding = 0, rgba8) uniform image2D outputImg;

uniform sampler2D u_image;

void main()
{
	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy); //ID of the work item(thread) in relation to the whole dispatch, not just the work group
	vec2 uv = vec2(pixel_coords) / vec2(512);
	vec4 pixelCol = texture2D(u_image, uv);

	vec4 colourSum = vec4(0.0);
	int sampleCount = 0;
	int radius = 0;

	for(int x = -radius; x <= radius; ++x)
	{
		for(int y = -radius; y <= radius; ++y)
		{
			vec2 samplePos = uv + vec2(x, y) / vec2(512);
			samplePos = clamp(samplePos, vec2(0.0), vec2(1.0));

			colourSum += texture(u_image, samplePos);
			sampleCount++;
		}
	}

	vec4 blurredColour = colourSum / float(sampleCount);


	imageStore(outputImg, pixel_coords, pixelCol); //write color to image at pixel coords
}