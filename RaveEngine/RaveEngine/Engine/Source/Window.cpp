#include "Engine/Include/Window.h"

rave::Window::Window(const char* title, const unsigned int width, const unsigned int height, const char* iconName)
	:
	size(width, height)
{
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	pWindow = glfwCreateWindow(width, height, title, nullptr, nullptr);
	errors.Throw();
	
	GLFWimage icon;

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
