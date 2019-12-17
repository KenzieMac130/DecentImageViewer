#pragma once

#include "divCommon.h"

/**
* @file
* @brief Application lifecycle
*/

/**
* @brief Renderer description
*/
typedef struct {
	const char* pFileToOpen;
} DecentImageViewerInfo;

/**
* @brief Enter the application's main loop
*/
DIV_EXPORT int DecentImageViewer_EnterApplication(DecentImageViewerInfo info);

DIV_EXPORT void DecentImageViewer_Shutdown(void);