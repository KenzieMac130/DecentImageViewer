#include "../abstraction.h"
#include "gl_Device.h"

#include "TinyImageFormat/tinyimageformat_query.h"
#include "TinyImageFormat/tinyimageformat_apis.h"

#ifdef _WIN32
#include <Windows.h>
#endif

#include <GL/glew.h>
#include <GL/GL.h>

GLenum nativizeTextureType(GPUTextureType type)
{
	switch (type)
	{
	default:
		DIVASSERT(1);
	case GPU_TEXTURE_2D:
		return GL_TEXTURE_2D;
		break;
	case GPU_TEXTURE_2D_ARRAY:
		return GL_TEXTURE_2D_ARRAY;
		break;
	case GPU_TEXTURE_CUBE:
		return GL_TEXTURE_CUBE_MAP;
		break;
	case GPU_TEXTURE_CUBE_ARRAY:
		return GL_TEXTURE_CUBE_MAP_ARRAY;
		break;
	case GPU_TEXTURE_3D:
		return GL_TEXTURE_3D;
		break;
	}
}

GLenum nativizeTextureFilter(GPUTextureFilter filter)
{
	switch (filter)
	{
	default:
		return GL_LINEAR;
	case GPU_TEXFILTER_NEAREST:
		return GL_NEAREST;
	}
}

GLenum nativizeTextureWrap(GPUTextureWrap wrap)
{
	switch (wrap)
	{
	default:
		return GL_REPEAT;
	case GPU_TEXWRAP_CLAMP_EDGE:
		return GL_CLAMP_TO_EDGE;
	}
}

struct glUploadFormatInfo { GLenum channels;  GLenum varType; };
/*Converts TinyImageFormats to what would be expected natively
OpenGL has some weird format management
this function is weird in return...
block compressed formats don't require this nonsense*/
struct glUploadFormatInfo nativizeFormatVarType(TinyImageFormat format, bool* failure)
{
	switch (format)
	{
	default:
		*failure = true; /*Don't have an option (try convert via TinyImage)*/
		return (struct glUploadFormatInfo) { GL_RGBA, GL_UNSIGNED_BYTE };
	/*R U8*/
	case TinyImageFormat_R8_UNORM:
	case TinyImageFormat_R8_UINT:
	case TinyImageFormat_R8_SRGB:
		return (struct glUploadFormatInfo){ GL_RED, GL_UNSIGNED_BYTE };
	/*R S8*/
	case TinyImageFormat_R8_SNORM:
	case TinyImageFormat_R8_SINT:
		return (struct glUploadFormatInfo) { GL_RED, GL_BYTE };
	/*RG U8*/
	case TinyImageFormat_R8G8_UNORM:
	case TinyImageFormat_R8G8_UINT:
	case TinyImageFormat_R8G8_SRGB:
		return (struct glUploadFormatInfo) { GL_RG, GL_UNSIGNED_BYTE };
	/*RG S8*/
	case TinyImageFormat_R8G8_SNORM:
	case TinyImageFormat_R8G8_SINT:
		return (struct glUploadFormatInfo) { GL_RG, GL_BYTE };
	/*R U16*/
	case TinyImageFormat_R16_UNORM:
	case TinyImageFormat_R16_UINT:
	case TinyImageFormat_D16_UNORM:
		return (struct glUploadFormatInfo) { GL_RED, GL_UNSIGNED_SHORT };
	/*R S16*/
	case TinyImageFormat_R16_SNORM:
	case TinyImageFormat_R16_SINT:
		return (struct glUploadFormatInfo) { GL_RED, GL_SHORT };
	/*R F16*/
	case TinyImageFormat_R16_SFLOAT:
		return (struct glUploadFormatInfo) { GL_RED, GL_HALF_FLOAT };
	/*RG U16*/
	case TinyImageFormat_R16G16_UNORM:
	case TinyImageFormat_R16G16_UINT:
		return (struct glUploadFormatInfo) { GL_RG, GL_UNSIGNED_SHORT };
	/*RG S16*/
	case TinyImageFormat_R16G16_SNORM:
	case TinyImageFormat_R16G16_SINT:
		return (struct glUploadFormatInfo) { GL_RG, GL_SHORT };
	/*RG F16*/
	case TinyImageFormat_R16G16_SFLOAT:
		return (struct glUploadFormatInfo) { GL_RG, GL_HALF_FLOAT };
	/*RGBA U8 (MOST COMMON FORMAT HERE)*/
	case TinyImageFormat_R8G8B8A8_UNORM:
	case TinyImageFormat_R8G8B8A8_UINT:
	case TinyImageFormat_R8G8B8A8_SRGB:
		return (struct glUploadFormatInfo) { GL_RGBA, GL_UNSIGNED_BYTE };
	/*RGBA S8*/
	case TinyImageFormat_R8G8B8A8_SNORM:
	case TinyImageFormat_R8G8B8A8_SINT:
		return (struct glUploadFormatInfo) { GL_RGBA, GL_BYTE };
	/*R S32*/
	case TinyImageFormat_R32_UINT:
	case TinyImageFormat_X8_D24_UNORM: /*better than nothing?*/
		return (struct glUploadFormatInfo) { GL_RED, GL_UNSIGNED_INT };
	/*R U32*/
	case TinyImageFormat_R32_SINT:
		return (struct glUploadFormatInfo) { GL_RED, GL_INT };
	/*R F32*/
	case TinyImageFormat_R32_SFLOAT:
	case TinyImageFormat_D32_SFLOAT:
		return (struct glUploadFormatInfo) { GL_RED, GL_FLOAT };
	/*RG S32*/
	case TinyImageFormat_R32G32_UINT:
		return (struct glUploadFormatInfo) { GL_RG, GL_UNSIGNED_INT };
	/*RG U32*/
	case TinyImageFormat_R32G32_SINT:
		return (struct glUploadFormatInfo) { GL_RG, GL_INT };
	/*RG F32*/
	case TinyImageFormat_R32G32_SFLOAT:
		return (struct glUploadFormatInfo) { GL_RG, GL_FLOAT };
	/*B10G11R11 (common for framebuffer/gbuffer/hdr compression)*/
#if EXPECT_PACKED_FLOAT_EXT
	case TinyImageFormat_B10G11R11_UFLOAT: /*should be reasonable to assume modern drivers support it*/
		return (struct glUploadFormatInfo) { GL_RGB, GL_UNSIGNED_INT_10F_11F_11F_REV };
#endif
	/*A2R10G10B10... and all that nonsense... convert it on CPU...*/
	/*RGBA U16*/
	case TinyImageFormat_R16G16B16A16_UNORM:
	case TinyImageFormat_R16G16B16A16_UINT:
		return (struct glUploadFormatInfo) { GL_RGBA, GL_UNSIGNED_SHORT };
	/*RGBA S16*/
	case TinyImageFormat_R16G16B16A16_SNORM:
	case TinyImageFormat_R16G16B16A16_SINT:
		return (struct glUploadFormatInfo) { GL_RGBA, GL_SHORT };
	/*RGBA F16*/
	case TinyImageFormat_R16G16B16A16_SFLOAT:
		return (struct glUploadFormatInfo) { GL_RGBA, GL_HALF_FLOAT };
	/*RGBA U32*/
	case TinyImageFormat_R32G32B32A32_UINT:
		return (struct glUploadFormatInfo) { GL_RGBA, GL_UNSIGNED_SHORT };
	/*RGBA S32*/
	case TinyImageFormat_R32G32B32A32_SINT:
		return (struct glUploadFormatInfo) { GL_RGBA, GL_SHORT };
	/*RGBA F32*/
	case TinyImageFormat_R32G32B32A32_SFLOAT:
		return (struct glUploadFormatInfo) { GL_RGBA, GL_HALF_FLOAT };
#if ALLOW_LARGE_FORMATS
	/*R U64*/
	case TinyImageFormat_R64_UINT:
		return (struct glUploadFormatInfo) { GL_RED, GL_UNSIGNED_INT };
	/*R S64*/
	case TinyImageFormat_R64_SINT:
		return (struct glUploadFormatInfo) { GL_RED, GL_INT };
	/*R F64*/
	case TinyImageFormat_R64_SFLOAT:
		return (struct glUploadFormatInfo) { GL_RED, GL_FLOAT };
	/*RG U64*/
	case TinyImageFormat_R64G64_UINT:
		return (struct glUploadFormatInfo) { GL_RG, GL_UNSIGNED_INT };
	/*RG S64*/
	case TinyImageFormat_R64G64_SINT:
		return (struct glUploadFormatInfo) { GL_RG, GL_INT };
	/*RG F64*/
	case TinyImageFormat_R64G64_SFLOAT:
		return (struct glUploadFormatInfo) { GL_RG, GL_FLOAT };
	/*RGBA U64*/
	case TinyImageFormat_R64G64B64A64_UINT:
		return (struct glUploadFormatInfo) { GL_RGBA, GL_UNSIGNED_INT };
	/*RGBA S64*/
	case TinyImageFormat_R64G64B64A64_SINT:
		return (struct glUploadFormatInfo) { GL_RGBA, GL_INT };
	/*RGBA F64*/
	case TinyImageFormat_R64G64B64A64_SFLOAT:
		return (struct glUploadFormatInfo) { GL_RGBA, GL_FLOAT };
#endif
	/*Anything else that has only 3 channels... convert on CPU because drivers suck...*/
	}
}

DIV_EXPORT DivResults Renderer_CreateImage(RenderCtx ctx, GpuImage* img, GpuImageInfo info)
{
	DIVASSERT(ctx);
	DIVASSERT(img);
	GL_RenderCtx* glCtx = (GL_RenderCtx*)ctx;

	/*Generate GL texture*/
	GLuint glTex;
	glGenTextures(1, &glTex);

	/*Bind and set parameters*/
	GLenum textureType = nativizeTextureType(info.type);
	glBindTexture(textureType, glTex);

	glTexParameteri(textureType, GL_TEXTURE_WRAP_S, nativizeTextureWrap(info.wrapX));
	glTexParameteri(textureType, GL_TEXTURE_WRAP_T, nativizeTextureWrap(info.wrapY));
	glTexParameteri(textureType, GL_TEXTURE_WRAP_R, nativizeTextureWrap(info.wrapZ));

	glTexParameteri(textureType, GL_TEXTURE_MIN_FILTER, nativizeTextureFilter(info.minFilter));
	glTexParameteri(textureType, GL_TEXTURE_MAG_FILTER, nativizeTextureFilter(info.magFilter));

	/*2D Texture*/
	if (textureType == GL_TEXTURE_2D)
	{
		/*Compressed*/
		if (TinyImageFormat_IsCompressed(info.format))
		{
			/*todo: block-compressed formats (DDS/KTX relevant)*/
		}
		/*Uncompressed*/
		else
		{
			/*Get Native GL Format*/
			bool unrecognizedType = false;
			struct glUploadFormatInfo glInfo = nativizeFormatVarType(info.format, &unrecognizedType);
			DIVASSERT(!unrecognizedType); /*AN UNSUPPORTED TYPE SHOULD HAVE NEVER MADE IT THIS FAR*/

			/*todo: https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glTexStorage2D.xhtml*/

			/*Upload each slice/level/etc*/
			ITTERATE_IMAGE_CONTENTS(contents, info.pImageContents)
			{
				glTexImage2D(GL_TEXTURE_2D,
					contents->miplevel,
					glInfo.channels,
					contents->width,
					contents->height,
					0,
					glInfo.channels, /*I don't trust the driver's conversion, use TinyImageFormat*/
					glInfo.varType,
					contents->pData);

				if (glGetError() != GL_NO_ERROR)
				{
					return DIV_FAILURE_FAILED_GPU_UPLOAD;
				}

				ITTERATE_IMAGE_CONTENTS_NEXT(contents)
			}

			/*
			(IMPORTANT) Please don't load in a weird format that won't support this
			*/
			if (info.generateMipmaps)
			{
				glGenerateMipmap(GL_TEXTURE_2D);
				if (glGetError() != GL_NO_ERROR)
				{
					return DIV_FAILURE_FAILED_GPU_TEX_MIPGEN;
				}
			}
		}
	}
	/*todo: more texture types (DDS/KTX relevant)*/

	/*Add image to manager*/
	GL_Texture finalTexture = (GL_Texture) {
		.texture = glTex,
		.imageInfo = info
	};
	stbds_hmput(glCtx->pHM_Images, glTex, finalTexture);

	return DIV_SUCCESS;
}

DIV_EXPORT void Renderer_DestroyImage(RenderCtx ctx, GpuImage img)
{
	DIVASSERT(ctx);
	GL_RenderCtx* glCtx = (GL_RenderCtx*)ctx;

	/*Drop out if invalid*/
	if (img == GPU_IMAGE_INVALID)
		return;

	/*Delete GL Texture*/
	GLuint glTex = (GLuint)img;
	glDeleteTextures(1, &glTex);

	/*Remove from Manager*/
	stbds_hmdel(glCtx->pHM_Images, glTex);
}