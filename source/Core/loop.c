#include "loop.h"

#include <SDL.h>

#include "../Renderer/abstraction.h"

RenderCtx renderCtx;

int MainLoop()
{
	while (1)
	{
		/*Event polling*/
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			/*Requested Quit*/
			if (event.type == SDL_QUIT){
				return 0;
			}

			/*Window events*/
			if (event.type == SDL_WINDOWEVENT) {
				if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
					Renderer_OnResize(renderCtx, event.window.windowID);
				}
			}
		}

		Renderer_Update(renderCtx);
	}
}

DIV_EXPORT int DecentImageViewer_EnterApplication(DecentImageViewerInfo info)
{
	DivResults error;
	/*Start loading image*/

	/*Startup renderer*/
	error = Renderer_CreateCtx(&renderCtx, (RenderCtxInfo)
	{
		.desiredGPU = 0,
		(WindowInfo) {
			.pName = "Decent Image Viewer",
			.width = 640,
			.height = 480,
			.x = -1,
			.y = -1,
			.monitor = 0,
			.antialiasing = 0,
		}
	});
	DIV_EXIT_ON_ERROR(error);

	return MainLoop();
}

DIV_EXPORT void DecentImageViewer_Shutdown(void)
{
	Renderer_DestroyCtx(&renderCtx);
}
