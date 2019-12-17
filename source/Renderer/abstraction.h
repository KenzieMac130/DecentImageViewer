#pragma once

/**
* @file
* @brief "Low-level" Rendering abstraction layer
* MUST NOT CONTAIN API SPECIFIC VARIABLES/FUNCTIONS
*/

#ifdef __cplusplus
extern "C" {
#endif

#include "divCommon.h"
#include "TinyImageFormat/tinyimageformat_base.h"

/** @brief Rendering context (device/window/draw management)*/
typedef void* RenderCtx;
/** @brief Rendering Image (texture ~ VkImageView+VkImage+VkSampler)*/
typedef uint64_t GpuImage;
/** @brief Rendering Buffer (constant/vertex/index buffer ~ VkBufferView+VkBuffer)*/
typedef uint64_t GpuBuffer;
/** @brief Rendering Pipeline (shaders/blending/etc ~ VkPipeline)*/
typedef void* GpuPipeline;
/** @brief Rendering Bindings (shader variables: constants, textures ~ VkDescriptorSet)*/
typedef void* GpuBindings;

#define GPU_IMAGE_INVALID UINT64_MAX
#define GPU_BUFFER_INVALID UINT64_MAX
#define GPU_PIPELINE_INVALID NULL
#define GPU_BINDING_INVALID NULL

/** @brief Callback to recreate the resouce if the device is lost
* feeds the needed render context and additional user data
*/
typedef int(*RecreateResourceCallback)(RenderCtx, void*);

/** @brief Forms to fill out in-case a resource is lost in a GPU driver incident*/
typedef struct {
	RecreateResourceCallback fpRecreateCallback;
	void* pRecreateData;
} RecreationInfo;

/**** Context ****/

/**
* @brief Window description
*/
typedef struct {
	const char* pName;
	int32_t width;
	int32_t height;
	int32_t x;
	int32_t y;
	int32_t monitor;
	int8_t antialiasing;
} WindowInfo;

/**
* @brief Renderer description
*/
typedef struct {
	int32_t desiredGPU;
	WindowInfo windowInfo;
} RenderCtxInfo;

/**
* @brief Create a rendering context
*/
DIV_EXPORT DivResults Renderer_CreateCtx(RenderCtx* ctx, RenderCtxInfo info);

/**
* @brief Destroy a rendering context
*/
DIV_EXPORT void Renderer_DestroyCtx(RenderCtx* ctx);

/**
* @brief Update render context
*/
DIV_EXPORT void Renderer_Update(RenderCtx ctx);

/**
* @brief Update screen on resize
*/
DIV_EXPORT void Renderer_OnResize(RenderCtx ctx, uint32_t windowID);

/**** Image ****/

/**
* @brief Types of textures
*/
typedef enum
{
	GPU_TEXTURE_2D,
	GPU_TEXTURE_2D_ARRAY,
	GPU_TEXTURE_CUBE,
	GPU_TEXTURE_CUBE_ARRAY,
	GPU_TEXTURE_3D,
	GPU_TEXTURE_MAX = UINT32_MAX
} GPUTextureType;

/**
* @brief Filtering mode
*/
typedef enum
{
	GPU_TEXFILTER_NEAREST,
	GPU_TEXFILTER_LINEAR,
	GPU_TEXFILTER_MAX = UINT32_MAX
} GPUTextureFilter;

/**
* @brief Filtering mode
*/
typedef enum
{
	GPU_TEXWRAP_REPEAT,
	GPU_TEXWRAP_CLAMP_EDGE,
	GPU_TEXWRAP_MAX = UINT32_MAX
} GPUTextureWrap;

/**
* @brief GPU Image contents
*/
struct S_GPUImageContents {
	uint32_t width;
	uint32_t height;
	uint32_t depth;
	uint32_t slice;
	uint32_t miplevel;

	void* pData;
	size_t size;

	/**@brief next image contents (multiple slices/mips)*/
	struct S_GPUImageContents* pNextContents;
};

typedef struct S_GPUImageContents GPUImageContents;

/**
* @breif Image contents itterator macro
*/
#define ITTERATE_IMAGE_CONTENTS(VARNAME, INITIAL) \
GPUImageContents* VARNAME = INITIAL; \
while (VARNAME)

/**
* @breif Progress Image contents itterator macro
* (at end of loop)
*/
#define ITTERATE_IMAGE_CONTENTS_NEXT(VARNAME) VARNAME = (GPUImageContents*)VARNAME->pNextContents;

/**
* @brief GPU Image description
*/
typedef struct {
	uint32_t width;
	uint32_t height;
	uint32_t depth;
	uint32_t miplevels;
	TinyImageFormat format;
	GPUTextureType type;

	bool generateMipmaps;
	GPUTextureFilter minFilter;
	GPUTextureFilter magFilter;
	GPUTextureWrap wrapX;
	GPUTextureWrap wrapY;
	GPUTextureWrap wrapZ;

	RecreationInfo recreation;

	GPUImageContents* pImageContents;
} GpuImageInfo;

/**
* @brief Create an image
*/
DIV_EXPORT DivResults Renderer_CreateImage(RenderCtx ctx, GpuImage* img, GpuImageInfo info);

/**
* @brief Destroy an image
*/
DIV_EXPORT void Renderer_DestroyImage(RenderCtx ctx, GpuImage img);

#ifdef __cplusplus
}
#endif