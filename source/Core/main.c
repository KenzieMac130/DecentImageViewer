#include <SDL.h>
#include "loop.h"

int main(int argv, char* argc[])
{
	atexit(DecentImageViewer_Shutdown);

	return DecentImageViewer_EnterApplication((DecentImageViewerInfo) {
		.pFileToOpen = ""
	});
}