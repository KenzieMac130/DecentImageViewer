#include "../abstraction.h"
#include "gl_Device.h"

#ifdef _WIN32
#include <Windows.h>
#endif

#include <SDL.h>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/GL.h>

/*There will never be a reason to create more than one GL context per-appication.*/
GL_RenderCtx globalGlCtx;
bool bCreatedCtx = false;

void resizeViewport(GL_RenderCtx* glCtx)
{
	DIVASSERT(glCtx);
	if (!glCtx->pMainSDLWindow)
		return;

	int w, h;
	SDL_GL_GetDrawableSize(glCtx->pMainSDLWindow, &w, &h);
	glViewport(0, 0, w, h);
}

DIV_EXPORT DivResults Renderer_CreateCtx(RenderCtx* ctx, RenderCtxInfo info)
{
	DIVASSERT(!bCreatedCtx); /*There can only be one...*/
	DIVASSERT(ctx);
	DIVASSERT(info.windowInfo.pName);

	/*"Allocate Context"*/
	GL_RenderCtx* glCtx = &globalGlCtx;
	*ctx = glCtx;

	/*Assign Info*/
	glCtx->info = info;

	SDL_InitSubSystem(SDL_INIT_VIDEO);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS,
	SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG | /*Disable depreciated*/
	SDL_GL_CONTEXT_RESET_ISOLATION_FLAG | /*Avoid crashing everthing*/
	SDL_GL_CONTEXT_ROBUST_ACCESS_FLAG /*Memory safety*/
#ifndef NDEBUG
	| SDL_GL_CONTEXT_DEBUG_FLAG /*OpenGL debugging*/
#endif
	);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	/*Generate window position for center of monitor*/
	if (info.windowInfo.x < 0 || info.windowInfo.y < 0)
	{
		info.windowInfo.x = SDL_WINDOWPOS_CENTERED_DISPLAY(info.windowInfo.monitor);
		info.windowInfo.y = SDL_WINDOWPOS_CENTERED_DISPLAY(info.windowInfo.monitor);
	}

	/*Get local window area*/
	else if (SDL_GetNumVideoDisplays() < info.windowInfo.monitor)
	{
		SDL_Rect displayBounds;
		SDL_GetDisplayBounds(info.windowInfo.monitor, &displayBounds);
		info.windowInfo.x += displayBounds.x;
		info.windowInfo.y += displayBounds.y;
	}

	/*Create SDL Window*/
	glCtx->pMainSDLWindow = SDL_CreateWindow(
		info.windowInfo.pName,
		info.windowInfo.x,
		info.windowInfo.y,
		info.windowInfo.width,
		info.windowInfo.height,
		SDL_WINDOW_RESIZABLE |
		/*OpenGL Flags*/
		SDL_WINDOW_OPENGL);
	if (!glCtx->pMainSDLWindow)
	{
		return DIV_FAILURE_FAILED_TO_CREATE_WINDOW;
	}


	if (!glCtx->pMainSDLWindow)
	{
		return DIV_FAILURE_FAILED_TO_CREATE_WINDOW;
	}

	/*Create GL Context*/
	glCtx->pSDLGlContext = SDL_GL_CreateContext(glCtx->pMainSDLWindow);
	if (!glCtx->pSDLGlContext)
	{
		SDL_DestroyWindow(glCtx->pMainSDLWindow);
		return DIV_FAILURE_FAILED_TO_CREATE_GPU_DEVICE;
	}

	/*GLEW*/
	if (glewInit() != GLEW_OK)
	{
		return DIV_FAILURE_FAILED_TO_CREATE_GPU_DEVICE;
	}

	/*Required Extensions*/
	if (!(true
#if EXPECT_PACKED_PIXEL_EXT
		&& GLEW_EXT_packed_pixels
#endif
#if EXPECT_PACKED_FLOAT_EXT
		&& GLEW_EXT_packed_float
#endif
#if EXPECT_S3TC_EXT
		&& GLEW_EXT_texture_compression_s3tc
#endif
#if EXPECT_BCPT_EXT
		&& GLEW_EXT_texture_compression_bptc
#endif
		)) {
		return DIV_FAILURE_DEVICE_INSUFFICIENT;
	}

	/*Initialize Image and Buffer Manager*/
	{
		glCtx->pHM_Images = NULL;
		glCtx->pHM_Buffers = NULL;

		GL_Texture defTex = (GL_Texture) {
			.texture = UINT64_MAX,
			.imageInfo = { 0 },
		};
		stbds_hmdefault(glCtx->pHM_Images, defTex);
	}

	/*Setup GL Viewport*/
	resizeViewport(glCtx);

	/*GL Global State*/
	glClearColor(0, 0, 0, 1);

	return DIV_SUCCESS;
}

DIV_EXPORT void Renderer_DestroyCtx(RenderCtx* ctx)
{
	DIVASSERT(ctx);
	GL_RenderCtx* glCtx = (GL_RenderCtx*)*ctx;

	/*Free Images*/
	stbds_hmfree(glCtx->pHM_Images);

	/*Free Buffers*/
	stbds_hmfree(glCtx->pHM_Buffers);

	/*Cleanup Window*/
	if (glCtx->pMainSDLWindow)
		SDL_DestroyWindow(glCtx->pMainSDLWindow);

	/*Cleanup Context*/
	if (glCtx->pSDLGlContext)
		SDL_GL_DeleteContext(glCtx->pSDLGlContext);

	/*"Deallocate Context"*/
	memset(*ctx, 0, sizeof(RenderCtx));
	bCreatedCtx = false;
}

DIV_EXPORT void Renderer_Update(RenderCtx ctx)
{
	DIVASSERT(ctx);
	GL_RenderCtx* glCtx = (GL_RenderCtx*)ctx;

	if (!glCtx->pMainSDLWindow)
		return;

	/* Render Here */
	glClear(GL_COLOR_BUFFER_BIT);

	SDL_GL_SwapWindow(glCtx->pMainSDLWindow);
}

DIV_EXPORT void Renderer_OnResize(RenderCtx ctx, uint32_t windowID)
{
	DIVASSERT(ctx);
	GL_RenderCtx* glCtx = (GL_RenderCtx*)ctx;
	resizeViewport(glCtx);
}