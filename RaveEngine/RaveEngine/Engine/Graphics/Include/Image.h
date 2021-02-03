#pragma once
#include "Engine/Graphics/Include/TextureBuffer.h"
#include "Engine/Utilities/Include/VulkanPointer.h"

namespace rave
{
	class Image
	{
	public:
		Image() = default;
		Image(const char* filename, const bool throws = false);
		Image(const int width, const int height);
		Image(const int width, const int height, const Color& background);

		Result Load(const char* filename);

		void Load(const int width, const int height);
		void Load(const int width, const int height, const Color& background);

	private:
		TextureBuffer<Color> buffer;
		vk::SurfaceKHR surface;
	};
}