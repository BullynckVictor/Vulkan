#pragma once
#include <array>

namespace rave
{
	namespace System
	{
		static constexpr struct
		{
#			ifdef _WIN32

#define RE_PLATFORM_WINDOWS

				constexpr operator bool() noexcept
				{
					return true;
				}
#				ifdef _WIN64
					static constexpr unsigned int bitlevel = 64u;
#				else
					static constexpr unsigned int bitlevel = 32u;
#				endif
#			else
				constexpr operator bool() noexcept
				{
					return false;
				}
#			endif
		} 
		windows;

#		ifdef __gnu_linux__
#define RE_PLATFORM_LINUX
			static constexpr bool linux = true;
#		else
			static constexpr bool linux = false;
#		endif

#		ifdef macintosh
#define RE_PLATFORM_MACOS
			static constexpr bool macOS = true;
#		else
			static constexpr bool macOS = false;
#		endif


#		ifdef NDEBUG
			static constexpr bool debug = false;
			static constexpr bool release = true;
#		else
			static constexpr bool debug = true;
			static constexpr bool release = false;
#		endif
	}
}