#include "Engine/Include/ImageLoader.h"
#include <fstream>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <stdio.h>
#include <stdint.h>


#define rave_bail_png() return rave::Result((std::wstring(L"Something went wrong while trying to read png file \"") + Widen(filename) + L"\"").c_str(), RE_FAIL, RE_IMAGE_LOAD_FAIL)
#define RETURN_ERROR(message) return rave::Result(message, RE_FAIL, RE_IMAGE_LOAD_FAIL)
#define RETURN_FNF() return rave::Result((L"Unable to open file \"" + rave::Widen(std::string(filename)) + L"\"").c_str(), RE_FAIL, RE_FILE_NOT_FOUND)

struct JpegErrorManager
{
	jpeg_error_mgr defaultErrorManager;
	jmp_buf jumpBuffer;
};

#pragma pack(push, 1)
struct BMPFileHeader {
	uint16_t file_type{ 0x4D42 };          // File type always BM which is 0x4D42 (stored as hex uint16_t in little endian)
	uint32_t file_size{ 0 };               // Size of the file (in bytes)
	uint16_t reserved1{ 0 };               // Reserved, always 0
	uint16_t reserved2{ 0 };               // Reserved, always 0
	uint32_t offset_data{ 0 };             // Start position of pixel data (bytes from the beginning of the file)
};

struct BMPInfoHeader {
	uint32_t size{ 0 };                      // Size of this header (in bytes)
	int32_t width{ 0 };                      // width of bitmap in pixels
	int32_t height{ 0 };                     // width of bitmap in pixels
											 //       (if positive, bottom-up, with origin in lower left corner)
											 //       (if negative, top-down, with origin in upper left corner)
	uint16_t planes{ 1 };                    // No. of planes for the target device, this is always 1
	uint16_t bit_count{ 0 };                 // No. of bits per pixel
	uint32_t compression{ 0 };               // 0 or 3 - uncompressed. THIS PROGRAM CONSIDERS ONLY UNCOMPRESSED BMP images
	uint32_t size_image{ 0 };                // 0 - for uncompressed images
	int32_t x_pixels_per_meter{ 0 };
	int32_t y_pixels_per_meter{ 0 };
	uint32_t colors_used{ 0 };               // No. color indexes in the color table. Use 0 for the max number of colors allowed by bit_count
	uint32_t colors_important{ 0 };          // No. of colors used for displaying the bitmap. If 0 all colors are required
};

struct BMPColorHeader {
	uint32_t red_mask{ 0x00ff0000 };         // Bit mask for the red channel
	uint32_t green_mask{ 0x0000ff00 };       // Bit mask for the green channel
	uint32_t blue_mask{ 0x000000ff };        // Bit mask for the blue channel
	uint32_t alpha_mask{ 0xff000000 };       // Bit mask for the alpha channel
	uint32_t color_space_type{ 0x73524742 }; // Default "sRGB" (0x73524742)
	uint32_t unused[16]{ 0 };                // Unused data for sRGB color space
};
#pragma pack(pop)


void check_color_header(BMPColorHeader& bmp_color_header)
{
	BMPColorHeader expected_color_header;
	if (expected_color_header.red_mask != bmp_color_header.red_mask ||
		expected_color_header.blue_mask != bmp_color_header.blue_mask ||
		expected_color_header.green_mask != bmp_color_header.green_mask ||
		expected_color_header.alpha_mask != bmp_color_header.alpha_mask)
	{
		rave_throw_message(L"Unexpected color mask format! The program expects the pixel data to be in the BGRA format");
	}
	if (expected_color_header.color_space_type != bmp_color_header.color_space_type)
	{
		rave_throw_message(L"Unexpected color space type! The program expects sRGB values");
	}
}

uint32_t make_stride_aligned(const uint32_t align_stride, const uint32_t row_stride)
{
	uint32_t new_stride = row_stride;
	while (new_stride % align_stride != 0)
	{
		new_stride++;
	}
	return new_stride;
}

struct ColorRGB
{
	unsigned char r, g, b;
};

void rave::JpegErrorExit(j_common_ptr cinfo)
{
	// cinfo->err is actually a pointer to my_error_mgr.defaultErrorManager, since pub
// is the first element of my_error_mgr we can do a sneaky cast
	JpegErrorManager* pErrorManager = (JpegErrorManager*)cinfo->err;
	(*cinfo->err->output_message)(cinfo); // print error message (actually disabled below)
	longjmp(pErrorManager->jumpBuffer, 1);
}

void rave::JpegOutputMessage(j_common_ptr cinfo)
{
	char buffer[JMSG_LENGTH_MAX];
	(*cinfo->err->format_message) (cinfo, buffer);
	rave_throw_message(Widen(buffer).c_str());
}

rave::Result rave::ReadImage(std::string_view filename, std::vector<Color>& data, unsigned int* pWidth, unsigned int* pHeight)
{
	size_t dotpos = filename.rfind('.');
	if (dotpos == filename.npos)
		return L"Invalid file name";
	std::string_view formatstr = filename.substr(dotpos);

	if (!FileExists(filename.data()))
		RETURN_FNF();

	switch (HashString(formatstr.data()))
	{
		case HashString(".png"):  return ReadPNG(filename.data(), data, pWidth, pHeight);
		case HashString(".bmp"):  return ReadBMP(filename.data(), data, pWidth, pHeight);
		case HashString(".jpg"):
		case HashString(".jpe"):
		case HashString(".jpeg"): return ReadJPEG(filename.data(), data, pWidth, pHeight);
		case HashString(".gif"):  return ReadGIF(filename.data(), data, 0, pWidth, pHeight);

		default: return L"File format not recognised";
	}
}

rave::OptionalResult<rave::Size> rave::ImageSizeGIF(const char* filename)
{
	gd_GIF* pGif = gd_open_gif(filename);
	if (!pGif)
		RETURN_FNF();

	Size size = Size(pGif->width, pGif->height);
	gd_close_gif(pGif);

	return size;
}
rave::OptionalResult<rave::Size> rave::ImageSizeBMP(const char* filename)
{
	BMPFileHeader file_header;
	BMPInfoHeader bmp_info_header;
	BMPColorHeader bmp_color_header;

	std::ifstream inp{ filename, std::ios_base::binary };
	if (!inp)
		RETURN_FNF();

	inp.read((char*)&file_header, sizeof(file_header));
	if (file_header.file_type != 0x4D42)
	{
		RETURN_ERROR(L"Unrecognized file format");
	}
	inp.read((char*)&bmp_info_header, sizeof(bmp_info_header));

	return Size(bmp_info_header.width, bmp_info_header.height);
}
rave::OptionalResult<rave::Size> rave::ImageSizePNG(const char* filename)
{
	FILE* fp = fopen(filename, "rb");

	png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png) RETURN_FNF();

	png_infop info = png_create_info_struct(png);
	if (!info) { png_destroy_read_struct(&png, NULL, NULL); fclose(fp); rave_bail_png(); }

	if (setjmp(png_jmpbuf(png))) { png_destroy_read_struct(&png, NULL, NULL); fclose(fp); rave_bail_png(); }

	png_init_io(png, fp);

	png_read_info(png, info);

	unsigned int width = png_get_image_width(png, info);
	unsigned int height = png_get_image_height(png, info);

	fclose(fp);
	png_destroy_read_struct(&png, &info, NULL);

	return Size(width, height);
}
rave::OptionalResult<rave::Size> rave::ImageSizeJPEG(const char* filename)
{
	jpeg_decompress_struct cinfo;
	JpegErrorManager errorManager;

	FILE* pFile = fopen(filename, "rb");
	if (!pFile)
		RETURN_FNF();

	// set our custom error handler
	cinfo.err = jpeg_std_error(&errorManager.defaultErrorManager);
	errorManager.defaultErrorManager.error_exit = JpegErrorExit;
	errorManager.defaultErrorManager.output_message = JpegOutputMessage;
	if (setjmp(errorManager.jumpBuffer))
	{
		// We jump here on errors
		jpeg_destroy_decompress(&cinfo);
		fclose(pFile);
		RETURN_ERROR(L"Error occurred trying to read jpeg file");
	}

	jpeg_create_decompress(&cinfo);
	jpeg_stdio_src(&cinfo, pFile);
	jpeg_read_header(&cinfo, TRUE);
	jpeg_start_decompress(&cinfo);

	unsigned int width = cinfo.image_width;
	unsigned int height = cinfo.image_height;

	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	fclose(pFile);

	return Size(width, height);
}
rave::OptionalResult<rave::Size> rave::ImageSize(std::string_view filename)
{
	size_t dotpos = filename.rfind('.');
	if (dotpos == filename.npos)
		RETURN_ERROR(L"Invalid file name");
	std::string_view formatstr = filename.substr(dotpos);

	if (!FileExists(filename.data()))
		RETURN_FNF();

	switch (HashString(formatstr.data()))
	{
		case HashString(".png"):  return ImageSizePNG(filename.data());
		case HashString(".bmp"):  return ImageSizeBMP(filename.data());
		case HashString(".jpg"):
		case HashString(".jpe"):
		case HashString(".jpeg"): return ImageSizeJPEG(filename.data());
		case HashString(".gif"):  return ImageSizeGIF(filename.data());

		default: RETURN_ERROR( L"File format not recognised" );
	}
}

rave::Result rave::ReadGIFRaw(const char* filename, Color* data, unsigned int frame)
{
	gd_GIF* pGif = gd_open_gif(filename);
	if (!pGif)
		RETURN_FNF();

	std::vector<ColorRGB> intermediate((size_t)pGif->width * (size_t)pGif->height);

	for (unsigned int i = 0; i < frame + 1 && gd_get_frame(pGif); i++);

	gd_render_frame(pGif, reinterpret_cast<unsigned char*>(intermediate.data()));
	for (const auto& color : intermediate)
	{
		data->r = color.r;
		data->g = color.g;
		data->b = color.b;
		data->a = gd_is_bgcolor(pGif, reinterpret_cast<const unsigned char*>(&color)) ? 0 : 255;

		data++;
	}

	gd_close_gif(pGif);

	return RE_SUCCESS;
}
rave::Result rave::ReadBMPRaw(const char* filename, Color* data)
{
	std::ifstream inp{ filename, std::ios_base::binary };
	if (inp)
	{
		BMPFileHeader file_header;
		BMPInfoHeader bmp_info_header;
		BMPColorHeader bmp_color_header;

		inp.read((char*)&file_header, sizeof(file_header));
		if (file_header.file_type != 0x4D42)
		{
			RETURN_ERROR(L"Unrecognized file format");
		}
		inp.read((char*)&bmp_info_header, sizeof(bmp_info_header));

		// The BMPColorHeader is used only for transparent images
		if (bmp_info_header.bit_count == 32)
		{
			// Check if the file has bit mask color information
			if (bmp_info_header.size >= (sizeof(BMPInfoHeader) + sizeof(BMPColorHeader)))
			{
				inp.read((char*)&bmp_color_header, sizeof(bmp_color_header));
				// Check if the pixel data is stored as BGRA and if the color space type is sRGB
				check_color_header(bmp_color_header);
			}
			else
			{
				RETURN_ERROR(L"Unrecognized file format");
			}
		}

		// Jump to the pixel data location
		inp.seekg(file_header.offset_data, inp.beg);

		// Adjust the header fields for output.
		// Some editors will put extra info in the image file, we only save the headers and the data.
		if (bmp_info_header.bit_count == 32)
		{
			bmp_info_header.size = sizeof(BMPInfoHeader) + sizeof(BMPColorHeader);
			file_header.offset_data = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader) + sizeof(BMPColorHeader);
		}
		else
		{
			bmp_info_header.size = sizeof(BMPInfoHeader);
			file_header.offset_data = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader);
		}
		file_header.file_size = file_header.offset_data;

		if (bmp_info_header.height < 0)
		{
			RETURN_ERROR(L"The program can treat only BMP images with the origin in the bottom left corner!");
		}

		// Here we check if we need to take into account row padding
		if (bmp_info_header.bit_count == 32)
		{
			if (bmp_info_header.width % 4 == 0)
			{
				unsigned int row_stride = (unsigned int)bmp_info_header.width * (unsigned int)bmp_info_header.bit_count / 8;

				for (int y = bmp_info_header.height - 1; y >= 0; --y)
				{
					inp.read((char*)(&data[(size_t)bmp_info_header.width * (size_t)y]), row_stride);
				}
			}
			else
			{
				unsigned int row_stride = (unsigned int)bmp_info_header.width * (unsigned int)bmp_info_header.bit_count / 8;
				uint32_t new_stride = make_stride_aligned(4, row_stride);
				std::vector<uint8_t> padding_row(new_stride - row_stride);

				for (int y = bmp_info_header.height - 1; y >= 0; --y)
				{
					inp.read((char*)(&data[(size_t)bmp_info_header.width * (size_t)y]), row_stride);
					inp.read((char*)padding_row.data(), padding_row.size());
				}
			}
			for (int i = 0; i < bmp_info_header.width * bmp_info_header.height; i++)
			{
				const auto temp = data[i];
				data[i].r = temp.b;
				data[i].b = temp.r;
			}
		}
		else
		{
			std::vector<ColorRGB> intermed((size_t)bmp_info_header.width * (size_t)bmp_info_header.height);
			if (bmp_info_header.width % 4 == 0)
			{
				unsigned int row_stride = (unsigned int)bmp_info_header.width * (unsigned int)bmp_info_header.bit_count / 8;

				for (int y = bmp_info_header.height - 1; y >= 0; --y)
				{
					inp.read((char*)(&intermed[(size_t)bmp_info_header.width * (size_t)y]), row_stride);
				}
			}
			else
			{
				unsigned int row_stride = (unsigned int)bmp_info_header.width * (unsigned int)bmp_info_header.bit_count / 8;
				uint32_t new_stride = make_stride_aligned(4, row_stride);
				std::vector<uint8_t> padding_row(new_stride - row_stride);

				for (int y = bmp_info_header.height - 1; y >= 0; --y)
				{
					inp.read((char*)(&data[(size_t)bmp_info_header.width * (size_t)y]), row_stride);
					inp.read((char*)padding_row.data(), padding_row.size());
				}
			}
			for (const auto& color : intermed)
			{
				data->r = color.b;
				data->g = color.g;
				data->b = color.r;
				data->a = 255;

				data++;
			}
		}

		return RE_SUCCESS;
	}
	else
	{
		RETURN_FNF();
	}
}
rave::Result rave::ReadPNGRaw(const char* filename, Color* data)
{
	FILE* fp = fopen(filename, "rb");

	png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png) RETURN_FNF();

	png_infop info = png_create_info_struct(png);
	if (!info) { png_destroy_read_struct(&png, NULL, NULL); fclose(fp); rave_bail_png(); }

	if (setjmp(png_jmpbuf(png))) { png_destroy_read_struct(&png, NULL, NULL); fclose(fp); rave_bail_png(); }

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

	std::vector<png_bytep> row_pointers(height);
	for (unsigned int y = 0; y < height; y++)
	{
		row_pointers[y] = reinterpret_cast<png_bytep>(&data[(size_t)y * (size_t)width]);
	}

	png_read_image(png, row_pointers.data());
	fclose(fp);
	png_destroy_read_struct(&png, &info, NULL);

	return RE_SUCCESS;
}
rave::Result rave::ReadJPEGRaw(const char* filename, Color* data)
{
	jpeg_decompress_struct cinfo;
	JpegErrorManager errorManager;

	FILE* pFile = fopen(filename, "rb");
	if (!pFile)
		RETURN_FNF();

	// set our custom error handler
	cinfo.err = jpeg_std_error(&errorManager.defaultErrorManager);
	errorManager.defaultErrorManager.error_exit = JpegErrorExit;
	errorManager.defaultErrorManager.output_message = JpegOutputMessage;
	if (setjmp(errorManager.jumpBuffer))
	{
		// We jump here on errors
		jpeg_destroy_decompress(&cinfo);
		fclose(pFile);
		RETURN_ERROR(L"Error occurred trying to read jpeg file");
	}

	jpeg_create_decompress(&cinfo);
	jpeg_stdio_src(&cinfo, pFile);
	jpeg_read_header(&cinfo, TRUE);
	jpeg_start_decompress(&cinfo);

	unsigned int width = cinfo.image_width;
	unsigned int height = cinfo.image_height;
	unsigned int colorchanels = cinfo.num_components;

	if (colorchanels != 4)
	{
		std::vector<ColorRGB> d((size_t)width * (size_t)height);

		while (cinfo.output_scanline < cinfo.image_height)
		{
			uint8_t* p = (uint8_t*)d.data() + (size_t)cinfo.output_scanline * (size_t)cinfo.image_width * (size_t)cinfo.num_components;
			jpeg_read_scanlines(&cinfo, &p, 1);
		}

		for (const auto& color : d)
		{
			data->r = color.r;
			data->g = color.g;
			data->b = color.b;
			data->a = 255;

			data++;
		}
	}
	else
	{
		while (cinfo.output_scanline < cinfo.image_height)
		{
			uint8_t* p = (uint8_t*)data + (size_t)cinfo.output_scanline * (size_t)cinfo.image_width * (size_t)cinfo.num_components;
			jpeg_read_scanlines(&cinfo, &p, 1);
		}
	}

	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	fclose(pFile);

	return RE_SUCCESS;
}
rave::Result rave::ReadImageRaw(std::string_view filename, Color* data)
{
	size_t dotpos = filename.rfind('.');
	if (dotpos == filename.npos)
		RETURN_ERROR(L"Invalid file name");
	std::string_view formatstr = filename.substr(dotpos);

	if (!FileExists(filename.data()))
		RETURN_FNF();

	switch (HashString(formatstr.data()))
	{
		case HashString(".png"):  return ReadPNGRaw(filename.data(), data);
		case HashString(".bmp"):  return ReadBMPRaw(filename.data(), data);
		case HashString(".jpg"):
		case HashString(".jpe"):
		case HashString(".jpeg"): return ReadJPEGRaw(filename.data(), data);
		case HashString(".gif"):  return ReadGIFRaw (filename.data(), data, 0);

		default: RETURN_ERROR( L"File format not recognised" );
	}
}

rave::Result rave::ReadGIF(const char* filename, std::vector<Color>& data, unsigned int frame, unsigned int* pWidth, unsigned int* pHeight)
{
	gd_GIF* pGif = gd_open_gif(filename);
	if(!pGif)
		RETURN_FNF();

	std::vector<ColorRGB> intermediate((size_t)pGif->width * (size_t)pGif->height);

	data.resize((size_t)pGif->width * (size_t)pGif->height);
	for (unsigned int i = 0; i < frame + 1 && gd_get_frame(pGif); i++);

	gd_render_frame(pGif, reinterpret_cast<unsigned char*>(intermediate.data()));
	auto it = data.begin();
	for (const auto& color : intermediate)
	{
		it->r = color.r;
		it->g = color.g;
		it->b = color.b;
		it->a = gd_is_bgcolor(pGif, reinterpret_cast<const unsigned char*>(&color)) ? 0 : 255;

		it++;
	}

	if (pWidth)
		*pWidth = pGif->width;
	if (pHeight)
		*pHeight = pGif->height;

	gd_close_gif(pGif);

	return RE_SUCCESS;
}
rave::Result rave::ReadBMP(const char* filename, std::vector<Color>& data, unsigned int* pWidth, unsigned int* pHeight)
{
    std::ifstream inp{ filename, std::ios_base::binary };
    if (inp) 
    {
        BMPFileHeader file_header;
        BMPInfoHeader bmp_info_header;
        BMPColorHeader bmp_color_header;

        inp.read((char*)&file_header, sizeof(file_header));
        if (file_header.file_type != 0x4D42) 
        {
            RETURN_ERROR(L"Unrecognized file format");
        }
        inp.read((char*)&bmp_info_header, sizeof(bmp_info_header));

        // The BMPColorHeader is used only for transparent images
        if (bmp_info_header.bit_count == 32) 
        {
            // Check if the file has bit mask color information
            if (bmp_info_header.size >= (sizeof(BMPInfoHeader) + sizeof(BMPColorHeader))) 
            {
                inp.read((char*)&bmp_color_header, sizeof(bmp_color_header));
                // Check if the pixel data is stored as BGRA and if the color space type is sRGB
                check_color_header(bmp_color_header);
            }
            else 
            {
				RETURN_ERROR(L"Unrecognized file format");
            }
        }

        // Jump to the pixel data location
        inp.seekg(file_header.offset_data, inp.beg);

        // Adjust the header fields for output.
        // Some editors will put extra info in the image file, we only save the headers and the data.
        if (bmp_info_header.bit_count == 32) 
        {
            bmp_info_header.size = sizeof(BMPInfoHeader) + sizeof(BMPColorHeader);
            file_header.offset_data = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader) + sizeof(BMPColorHeader);
        }
        else 
        {
            bmp_info_header.size = sizeof(BMPInfoHeader);
            file_header.offset_data = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader);
        }
        file_header.file_size = file_header.offset_data;

        if (bmp_info_header.height < 0) 
        {
			RETURN_ERROR(L"The program can treat only BMP images with the origin in the bottom left corner!");
        }

        data.resize((size_t)bmp_info_header.width * (size_t)bmp_info_header.height);

        // Here we check if we need to take into account row padding
		if (bmp_info_header.bit_count == 32)
		{
			if (bmp_info_header.width % 4 == 0)
			{
				unsigned int row_stride = (unsigned int)bmp_info_header.width * (unsigned int)bmp_info_header.bit_count / 8;

				for (int y = bmp_info_header.height - 1; y >= 0; --y)
				{
					inp.read((char*)(&data[(size_t)bmp_info_header.width * (size_t)y]), row_stride);
				}
			}
			else
			{
				unsigned int row_stride = (unsigned int)bmp_info_header.width * (unsigned int)bmp_info_header.bit_count / 8;
				uint32_t new_stride = make_stride_aligned(4, row_stride);
				std::vector<uint8_t> padding_row(new_stride - row_stride);

				for (int y = bmp_info_header.height - 1; y >= 0; --y)
				{
					inp.read((char*)(&data[(size_t)bmp_info_header.width * (size_t)y]), row_stride);
					inp.read((char*)padding_row.data(), padding_row.size());
				}
			}
			for (auto& color : data)
			{
				const auto temp = color;
				color.r = temp.b;
				color.b = temp.r;
			}
		}
		else
		{
			std::vector<ColorRGB> intermed((size_t)bmp_info_header.width * (size_t)bmp_info_header.height);
			if (bmp_info_header.width % 4 == 0)
			{
				unsigned int row_stride = (unsigned int)bmp_info_header.width * (unsigned int)bmp_info_header.bit_count / 8;

				for (int y = bmp_info_header.height - 1; y >= 0; --y)
				{
					inp.read((char*)(&intermed[(size_t)bmp_info_header.width * (size_t)y]), row_stride);
				}
			}
			else
			{
				unsigned int row_stride = (unsigned int)bmp_info_header.width * (unsigned int)bmp_info_header.bit_count / 8;
				uint32_t new_stride = make_stride_aligned(4, row_stride);
				std::vector<uint8_t> padding_row(new_stride - row_stride);

				for (int y = bmp_info_header.height - 1; y >= 0; --y)
				{
					inp.read((char*)(&data[(size_t)bmp_info_header.width * (size_t)y]), row_stride);
					inp.read((char*)padding_row.data(), padding_row.size());
				}
			}
			auto it = data.begin();
			for (const auto& color : intermed)
			{
				it->r = color.b;
				it->g = color.g;
				it->b = color.r;
				it->a = 255;

				it++;
			}
		}
		if (pWidth)
			*pWidth = (unsigned int)bmp_info_header.width;
		if (pHeight)
			*pHeight = (unsigned int)bmp_info_header.height;

		return RE_SUCCESS;
    }
    else
    {
        RETURN_FNF();
    }
}
rave::Result rave::ReadPNG(const char* filename, std::vector<Color>& data, unsigned int* pWidth, unsigned int* pHeight)
{
	FILE* fp = fopen(filename, "rb");

	png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png) RETURN_FNF();

	png_infop info = png_create_info_struct(png);
	if (!info) { png_destroy_read_struct(&png, NULL, NULL); fclose(fp); rave_bail_png(); }

	if (setjmp(png_jmpbuf(png))) { png_destroy_read_struct(&png, NULL, NULL); fclose(fp); rave_bail_png(); }

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

	data.resize((size_t)width * (size_t)height);
	std::vector<png_bytep> row_pointers(height);
	for (unsigned int y = 0; y < height; y++)
	{
		row_pointers[y] = reinterpret_cast<png_bytep>(&data[(size_t)y * (size_t)width]);
	}

	png_read_image(png, row_pointers.data());
	fclose(fp);
	png_destroy_read_struct(&png, &info, NULL);

	if (pWidth)
		*pWidth = width;
	if (pHeight)
		*pHeight = height;

	return RE_SUCCESS;
}
rave::Result rave::ReadJPEG(const char* filename, std::vector<Color>& data, unsigned int* pWidth, unsigned int* pHeight)
{
	jpeg_decompress_struct cinfo;
	JpegErrorManager errorManager;

	FILE* pFile = fopen(filename, "rb");
	if (!pFile)
		RETURN_FNF();

	// set our custom error handler
	cinfo.err = jpeg_std_error(&errorManager.defaultErrorManager);
	errorManager.defaultErrorManager.error_exit = JpegErrorExit;
	errorManager.defaultErrorManager.output_message = JpegOutputMessage;
	if (setjmp(errorManager.jumpBuffer))
	{
		// We jump here on errors
		jpeg_destroy_decompress(&cinfo);
		fclose(pFile);
		RETURN_ERROR(L"Error occurred trying to read jpeg file");
	}

	jpeg_create_decompress(&cinfo);
	jpeg_stdio_src(&cinfo, pFile);
	jpeg_read_header(&cinfo, TRUE);
	jpeg_start_decompress(&cinfo);

	unsigned int width = cinfo.image_width;
	unsigned int height = cinfo.image_height;
	unsigned int colorchanels = cinfo.num_components;

	data.resize((size_t)width * (size_t)height);

	if (colorchanels != 4)
	{
		std::vector<ColorRGB> d((size_t)width * (size_t)height);

		while (cinfo.output_scanline < cinfo.image_height)
		{
			uint8_t* p = (uint8_t*)d.data() + (size_t)cinfo.output_scanline * (size_t)cinfo.image_width * (size_t)cinfo.num_components;
			jpeg_read_scanlines(&cinfo, &p, 1);
		}

		auto it = data.begin();
		for (const auto& color : d)
		{
			it->r = color.r;
			it->g = color.g;
			it->b = color.b;
			it->a = 255;

			it++;
		}
	}
	else
	{
		while (cinfo.output_scanline < cinfo.image_height)
		{
			uint8_t* p = (uint8_t*)data.data() + (size_t)cinfo.output_scanline * (size_t)cinfo.image_width * (size_t)cinfo.num_components;
			jpeg_read_scanlines(&cinfo, &p, 1);
		}
	}

	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	fclose(pFile);

	if (pWidth)
		*pWidth = width;
	if (pHeight)
		*pHeight = height;

	return RE_SUCCESS;
}
