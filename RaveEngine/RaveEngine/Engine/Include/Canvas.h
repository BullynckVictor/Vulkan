#pragma once
#include "Engine/Utilities/Include/Flag.h"
#include "Engine/Utilities/Include/Color.h"
#include "Engine/Graphics/Include/Image.h"

namespace rave
{
	enum CanvasOptions
	{
		CANVAS_RESIZEABLE = 0,
		CANVAS_CPU
	};


	class Canvas
	{
	public:
		Canvas(const char* name, const int& width, const int& height, const Flag<CanvasOptions>& flags = CANVAS_CPU);

	private:
		Image cpuTexture = {};

		GLFWwindow* window = nullptr;
		VkInstance instance = nullptr;
	};
}