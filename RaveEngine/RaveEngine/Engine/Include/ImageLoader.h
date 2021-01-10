#pragma once
#include "Libraries/libpng/png.h"
#include "Engine/Utilities/Include/String.h"
#include "Engine/Utilities/Include/Exception.h"
#include <string_view>
#include <vector>

#define rave_bail_png() rave_throw_message((std::wstring(L"Something went wrong while trying to read png file \"") + Widen(filename) + L"\"").c_str())

namespace rave
{
	enum class ImageType
	{
		png,
		bmp,
		gif,
		ico,
		jpeg
	};

	void ReadPNG(const char* filename, std::vector<unsigned char>& data, unsigned int* pWidth = nullptr, unsigned int* pHeight = nullptr)
	{
		FILE* fp = fopen(filename, "rb");

		png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
		if (!png) rave_bail_png();

		png_infop info = png_create_info_struct(png);
		if (!info) { png_destroy_read_struct(&png, NULL, NULL); rave_bail_png(); }

		if (setjmp(png_jmpbuf(png))) { png_destroy_read_struct(&png, NULL, NULL); rave_bail_png(); }

		png_init_io(png, fp);

		png_read_info(png, info);

		unsigned int width = png_get_image_width(png, info);
		unsigned int height = png_get_image_height(png, info);
		unsigned int color_type = png_get_color_type(png, info);
		unsigned int bit_depth = png_get_bit_depth(png, info);

		// Read any color_type into 8bit depth, RGBA format.
		// See http://www.libpng.org/pub/png/libpng-manual.txt

		if (bit_depth == 16)
			png_set_strip_16(png);

		if (color_type == PNG_COLOR_TYPE_PALETTE)
			png_set_palette_to_rgb(png);

		// PNG_COLOR_TYPE_GRAY_ALPHA is always 8 or 16bit depth.
		if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
			png_set_expand_gray_1_2_4_to_8(png);

		if (png_get_valid(png, info, PNG_INFO_tRNS))
			png_set_tRNS_to_alpha(png);

		// These color_type don't have an alpha channel then fill it with 0xff.
		if (color_type == PNG_COLOR_TYPE_RGB ||
			color_type == PNG_COLOR_TYPE_GRAY ||
			color_type == PNG_COLOR_TYPE_PALETTE)
			png_set_filler(png, 0xFF, PNG_FILLER_AFTER);

		if (color_type == PNG_COLOR_TYPE_GRAY ||
			color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
			png_set_gray_to_rgb(png);

		png_read_update_info(png, info);

		data.resize((size_t)width * 4 * (size_t)height);
		std::vector<png_bytep> row_pointers(height);
		for (unsigned int y = 0; y < height; y++) 
		{
			row_pointers[y] = &data[(size_t)y * (size_t)width * 4];
		}

		png_read_image(png, row_pointers.data());
		fclose(fp);
		png_destroy_read_struct(&png, &info, NULL);

		if (pWidth)
			*pWidth = width;
		if (pHeight)
			*pHeight = height;
	}

	static void ReadImage(std::string_view filename, std::vector<unsigned char>& data, unsigned int* pWidth = nullptr, unsigned int* pHeight = nullptr)
	{
		size_t dotpos = filename.rfind('.');
		rave_assert_info(dotpos != filename.npos, L"Invalid file name");
		std::string_view formatstr = filename.substr(dotpos);

		ImageType type;
		rave_check_file(filename.data());

		switch (HashString(formatstr.data()))
		{
			case HashString(".png"):  ReadPNG(filename.data(), data, pWidth, pHeight);	break;
			case HashString(".gif"):  type = ImageType::gif;	break;
			case HashString(".bmp"):  type = ImageType::bmp;	break;
			case HashString(".ico"):  type = ImageType::ico;	break;
			case HashString(".jpg"):
			case HashString(".jpe"):
			case HashString(".jpeg"): type = ImageType::jpeg;	break;

			default: rave_throw_message(L"File format not recognised");
		}
	}
}


#undef rave_bail_png