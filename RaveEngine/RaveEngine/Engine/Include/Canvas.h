#pragma once
#include "Engine/Utilities/Include/Flag.h"
#include "Engine/Utilities/Include/Color.h"
#include "Engine/Graphics/Include/TextureBuffer.h"

namespace rave
{
	class Canvas;

	enum CanvasOptions
	{
		CANVAS_RESIZEABLE = 0,
		CANVAS_CPU
	};

	class Canvas
	{
	public:
		Canvas(const char* name, const int& width, const int& height, const Flag<CanvasOptions>& flags = 0);

	private:
		TextureBuffer<Color> cpuTexture;

		GLFWwindow* window = nullptr;
		VkInstance instance = {};
	};
}