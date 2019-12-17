#include "divCommon.h"
#include "gl_Image.h"
#include "../abstraction.h"
#include <SDL_video.h>

typedef struct {
	/*Context and main window*/
	SDL_Window* pMainSDLWindow;
	SDL_GLContext pSDLGlContext;
	RenderCtxInfo info;

	/*STB hash-map of textures*/
	struct { GpuImage key; GL_Texture value; } *pHM_Images;
	/*STB hash-map of buffers*/
	struct { GpuBuffer key; void* value; } *pHM_Buffers;
} GL_RenderCtx;