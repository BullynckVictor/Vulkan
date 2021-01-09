#pragma once
#include "Engine/Include/CommonIncludes.h"
#include "Engine/Utilities/Include/Exception.h"
#include "Engine/Utilities/Include/Vector.h"

namespace rave
{
	class Window
	{
	public:
		Window(const char* title, const unsigned int width, const unsigned int height, const char* icon = nullptr);
		~Window() noexcept;

		static void HandleMessages();
		bool IsOpen() const;

		Size GetSize() const noexcept;
		

	private:
		Size size;
		GLFWwindow* pWindow = nullptr;
	};
}