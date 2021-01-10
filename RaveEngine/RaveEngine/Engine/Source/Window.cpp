#include "Engine/Include/Window.h"
#include "Engine/Include/ImageLoader.h"

rave::Window::Window(const char* title, const unsigned int width, const unsigned int height, const char* iconName)
	:
	size(width, height)
{
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	pWindow = glfwCreateWindow(width, height, title, nullptr, nullptr);
	errors.Throw();
	
	if (iconName)
	{
		GLFWimage icon;
		std::vector<unsigned char> data;
		unsigned int w, h;
		ReadImage(iconName, data, &w, &h);
		icon.pixels = data.data();
		icon.width = (int)w;
		icon.height = (int)h;
		glfwSetWindowIcon(pWindow, 1, &icon);
		errors.Throw();
	}

}

rave::Window::~Window() noexcept
{
	glfwDestroyWindow(pWindow);
}

void rave::Window::HandleMessages()
{
	glfwPollEvents();
	errors.Throw();
}

bool rave::Window::IsOpen() const
{
	return !glfwWindowShouldClose(pWindow);
}

rave::Size rave::Window::GetSize() const noexcept
{
	return size;
}
