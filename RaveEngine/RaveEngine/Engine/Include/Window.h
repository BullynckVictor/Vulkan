#pragma once
#include "Engine/Include/CommonIncludes.h"
#include "Engine/Utilities/Include/Exception.h"
#include "Engine/Utilities/Include/Vector.h"

#include "Engine/Include/Mouse.h"
#include "Engine/Include/Keyboard.h"

#include "Engine/Graphics/Include/Graphics.h"

#include <map>

namespace rave
{


	class Window : GraphicsFriend
	{
	public:
		Window(const Graphics& gfx, const char* title, const unsigned int width, const unsigned int height, const char* icon = nullptr);
		~Window() noexcept;

		static void HandleMessages();
		bool IsOpen() const;

		Size GetSize() const noexcept;

		std::string GetTitle();
		void SetTitle(const char* title);
		void SetTitle(const std::string& title);

	public:
		Mouse mouse;
		Keyboard kbd;
		std::string title;

	private:
		static void ScrollCallback(GLFWwindow* pWindow, double xoffset, double yoffset);

	private:
		Size size;
		GLFWwindow* window = nullptr;
		VkQueue presentQueue = VK_NULL_HANDLE;

		vk::SurfaceKHR surface;
	};

	struct WindowManager
	{
	public:
		void Add(GLFWwindow* pWindow, Window* pWnd) noexcept;
		Window* Get(GLFWwindow* pWindow) noexcept;

		void Cleanup() noexcept;

	private:
		std::map<GLFWwindow*, Window*> map;
	};

	extern WindowManager windowManager;
}