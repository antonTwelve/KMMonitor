#pragma once
#define RELEASE_VERSION
//#define DEBUG_VERSION

#ifdef RELEASE_VERSION
#define START_KM_LISTEN
#define HIDE_CMD_WINDOW
#elif defined(DEBUG_VERSION)
#define SHOWVKCODE			//在控制台print当前的keycode
#define START_KM_LISTEN
//#define VUE_DEBUG
#endif