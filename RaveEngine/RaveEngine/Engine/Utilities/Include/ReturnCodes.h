#pragma once
#include <stdint.h>

namespace rave
{
	enum ReturnType : uint32_t
	{
		RE_SUCCESS = 0,
		RE_FAIL,
		RE_WARNING
	};

	enum ReturnCode : uint32_t
	{
		RE_NULL = 0,
		RE_FILE_NOT_FOUND,
		RE_IMAGE_LOAD_FAIL
	};
}