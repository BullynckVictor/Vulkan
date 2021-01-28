#pragma once
#include <vector>
#include "Engine/Utilities/Include/Vector.h"
#include "Engine/Include/ImageLoader.h"

namespace rave
{
	template<typename T>
	class TextureBuffer
	{
	public:
		TextureBuffer() = default;
		TextureBuffer(const char* filename)
		{

		}
		TextureBuffer(const int width, const int height);

	private:
		T* data = nullptr;
		Vector<2, int> size = 0;
	};
}