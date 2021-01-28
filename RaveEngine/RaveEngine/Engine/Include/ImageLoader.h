#pragma once
#include "Engine/Utilities/Include/String.h"
#include "Engine/Utilities/Include/Exception.h"
#include "Engine/Utilities/Include/Result.h"
#include "Engine/Utilities/Include/Color.h"
#include <string_view>
#include <vector>
#include <setjmp.h>
#include "Libraries/libjpg/jpeglib.h"

namespace rave
{
	Result ReadGIF(const char* filename, std::vector<Color>& data, unsigned int frame = 0, unsigned int* pWidth = nullptr, unsigned int* pHeight = nullptr);
	Result ReadBMP(const char* filename, std::vector<Color>& data, unsigned int* pWidth = nullptr, unsigned int* pHeight = nullptr);
	Result ReadPNG(const char* filename, std::vector<Color>& data, unsigned int* pWidth = nullptr, unsigned int* pHeight = nullptr);
	Result ReadJPEG(const char* filename, std::vector<Color>& data, unsigned int* pWidth = nullptr, unsigned int* pHeight = nullptr);
	Result ReadImage(std::string_view filename, std::vector<Color>& data, unsigned int* pWidth = nullptr, unsigned int* pHeight = nullptr);

	static void JpegErrorExit(j_common_ptr cinfo);
	static void JpegOutputMessage(j_common_ptr cinfo);
}