#include "abstract_system.h"

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

void div_Sleep(uint32_t milleseconds)
{
#ifdef _WIN32
	Sleep((DWORD)milleseconds);
#else
	usleep((float)(milleseconds) * 0.01f);
#endif
}
