#include "Engine/Graphics/Include/Image.h"

rave::Image::Image(const char* filename, const bool throws)
{
	auto result = Load(filename);
	if (throws)
		result.Throw();
}

rave::Image::Image(const int width, const int height)
{
}

rave::Image::Image(const int width, const int height, const Color& background)
{
	Load(width, height, background);
}

rave::Result rave::Image::Load(const char* filename)
{
	auto imgSize = ImageSize(filename);
	if (imgSize.GetResult().Failed())
		return imgSize.GetResult();

	buffer.Load(imgSize.Get().x, imgSize.Get().y);
	return ReadImageRaw(filename, buffer.Data());
}

void rave::Image::Load(const int width, const int height)
{
	buffer.Load(width, height);
}

void rave::Image::Load(const int width, const int height, const Color& background)
{
	buffer.Load(width, height, background);
}