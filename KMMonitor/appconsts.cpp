#include "appconsts.h"

namespace app_const {
#ifdef RELEASE_VERSION
	LPCWSTR APP_NAME = TEXT("KMMonitor");
#elif defined(DEBUG_VERSION)
	LPCWSTR APP_NAME = TEXT("KMMonitor_DEBUG");
#endif
#ifdef RELEASE_VERSION
	const wchar_t* SHORT_CUT_NAME = L"KMMonitor.lnk";
#elif defined(DEBUG_VERSION)
	const wchar_t* SHORT_CUT_NAME = L"KMMonitor_DEBUG.lnk";
#endif
	//std::string ALLOWED_HOST = "localhost";
}