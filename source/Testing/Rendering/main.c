#include <stdio.h>
#include <math.h>

#include "divCommon.h"
#include "../Renderer/abstraction.h"

RenderCtx renderCtx;

typedef struct {
	union 
	{
		struct
		{
			uint8_t r;
			uint8_t g;
			uint8_t b;
			uint8_t a;
		};
		uint8_t data[4];
	};
} rgba8;

rgba8* image;
GpuImage gpuImage;

void OnExit(void)
{
	Renderer_DestroyImage(renderCtx, gpuImage);
	Renderer_DestroyCtx(&renderCtx);
}

int main(int argc, char* argv[])
{
	/*Get Properties*/
	uint32_t width = 0;
	uint32_t height = 0;
	if (argc >= 3)
	{
		width = (uint32_t)atoi(argv[1]);
		height = (uint32_t)atoi(argv[2]);
	}
	if (!width || !height)
	{
		width = 512;
		height = 512;
	}

	atexit(OnExit);

	/*Start Renderer*/
	DivResults error;
	error = Renderer_CreateCtx(&renderCtx, (RenderCtxInfo)
	{
		.desiredGPU = 0,
			(WindowInfo) {
			.pName = "DIV Render Test",
			.width = 640,
			.height = 480,
			.x = -1,
			.y = -1,
			.monitor = 0,
			.antialiasing = 0,
		}
	});
	DIV_EXIT_ON_ERROR(error);

	/*Image Test*/
	{
		image = calloc(width*height, sizeof(rgba8));

		srand(0);
		for (uint32_t iy = 0; iy < height; iy++)
		{
			for (uint32_t ix = 0; ix < width; ix++)
			{
				image[(iy * width) + ix].r = (uint8_t)(((float)(ix) / width) * 255);
				image[(iy * width) + ix].g = (uint8_t)(((float)(iy) / height) * 255);
				image[(iy * width) + ix].b = (uint8_t)(((float)(rand()) / RAND_MAX) * 255);
				image[(iy * width) + ix].a = 255;
			}
		}

		/*Fill out image contents*/
		GPUImageContents contents = (GPUImageContents)
		{
			.width = width,
			.height = height,
			.depth = 1,
			.slice = 0,
			.miplevel = 0,

			.pData = image,
			.size = width * height * sizeof(rgba8),

			.pNextContents = NULL
		};

		/*Fill out image info*/
		GpuImageInfo imageInfo = (GpuImageInfo)
		{
			.width = width,
			.height = height,
			.depth = 1,
			.miplevels = 8,
			.format = TinyImageFormat_R8G8B8A8_UNORM,
			.type = GPU_TEXTURE_2D,

			.generateMipmaps = true,
			.minFilter = GPU_TEXFILTER_LINEAR,
			.magFilter = GPU_TEXFILTER_LINEAR,
			.wrapX = GPU_TEXWRAP_REPEAT,
			.wrapY = GPU_TEXWRAP_REPEAT,
			.wrapZ = GPU_TEXWRAP_REPEAT,

			.recreation = (RecreationInfo) {
				.fpRecreateCallback = NULL,
				.pRecreateData = NULL
			},
			
			.pImageContents = &contents
		};

		Renderer_CreateImage(renderCtx, &gpuImage, imageInfo);

		free(image);
	}

	Renderer_Update(renderCtx);

	div_Sleep(10000);
	return 0;
}