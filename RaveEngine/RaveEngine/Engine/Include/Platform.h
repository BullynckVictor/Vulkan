#pragma once
#include "Engine/Utilities/Include/SystemInfo.h"

#define rave_main() main()

#ifdef RE_PLATFORM_WINDOWS

#define NOMINMAX
#include <Windows.h>

#undef rave_main
#define rave_main() WINAPI wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int)
#endif