#pragma once
#include "Engine/Utilities/Include/String.h"
#include "Engine/Utilities/Include/Exception.h"
#include "Engine/Utilities/Include/Result.h"
#include "Engine/Utilities/Include/Color.h"
#include "Engine/Utilities/Include/Vector.h"
#include <string_view>
#include <vector>
#include <setjmp.h>
#include "Libraries/libjpg/jpeglib.h"
#include "Libraries/libpng/png.h"
#include "Libraries/cgif/gifdec.h"

namespace rave
{
	Result ReadGIF  (const char* filename, std::vector<Color>& data, unsigned int frame = 0, unsigned int* pWidth = nullptr, unsigned int* pHeight = nullptr);
	Result ReadBMP  (const char* filename, std::vector<Color>& data, unsigned int* pWidth = nullptr, unsigned int* pHeight = nullptr);
	Result ReadPNG  (const char* filename, std::vector<Color>& data, unsigned int* pWidth = nullptr, unsigned int* pHeight = nullptr);
	Result ReadJPEG (const char* filename, std::vector<Color>& data, unsigned int* pWidth = nullptr, unsigned int* pHeight = nullptr);
	Result ReadImage(std::string_view filename, std::vector<Color>& data, unsigned int* pWidth = nullptr, unsigned int* pHeight = nullptr);

	OptionalResult<Size> ImageSizeGIF (const char* filename);
	OptionalResult<Size> ImageSizeBMP (const char* filename);
	OptionalResult<Size> ImageSizePNG (const char* filename);
	OptionalResult<Size> ImageSizeJPEG(const char* filename);
	OptionalResult<Size> ImageSize(std::string_view filename);

	Result ReadGIFRaw  (const char* filename, Color*, unsigned int frame = 0);
	Result ReadBMPRaw  (const char* filename, Color*);
	Result ReadPNGRaw  (const char* filename, Color*);
	Result ReadJPEGRaw (const char* filename, Color*);
	Result ReadImageRaw(std::string_view  filename, Color* data);

	static void JpegErrorExit(j_common_ptr cinfo);
	static void JpegOutputMessage(j_common_ptr cinfo);
}