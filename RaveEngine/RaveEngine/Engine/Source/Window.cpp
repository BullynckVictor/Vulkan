#include "Engine/Include/Window.h"
#include "Engine/Include/ImageLoader.h"

rave::WindowManager rave::windowManager;

rave::Window::Window(const char* title, const unsigned int width, const unsigned int height, const char* iconName)
	:
	size(width, height)
{
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	pWindow = glfwCreateWindow(width, height, title, nullptr, nullptr);
	errors.Throw();

	mouse.pWindow = pWindow;
	kbd  .pWindow = pWindow;

	windowManager.Add(pWindow, this);

	glfwSetScrollCallback(pWindow, ScrollCallback);
	
	if (iconName)
	{
		GLFWimage icon;
		std::vector<Color> data;
		unsigned int w, h;
		ReadImage(iconName, data, &w, &h);
		icon.pixels = reinterpret_cast<unsigned char*>(data.data());
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

std::string rave::Window::GetTitle()
{
	return title;
}

void rave::Window::SetTitle(const char* title_c)
{
	title = title_c;
	glfwSetWindowTitle(pWindow, title_c);
}

void rave::Window::SetTitle(const std::string& title_c)
{
	title = title_c;
	glfwSetWindowTitle(pWindow, title_c.c_str());
}

void rave::Window::ScrollCallback(GLFWwindow* pWindow, double xoffset, double yoffset)
{
	windowManager.Get(pWindow)->mouse.scroll += (int)floor(yoffset);
}

void rave::WindowManager::Add(GLFWwindow* pWindow, Window* pWnd) noexcept
{
	map[pWindow] = pWnd;
}

rave::Window* rave::WindowManager::Get(GLFWwindow* pWindow) noexcept
{
	return map[pWindow];
}

void rave::WindowManager::Cleanup() noexcept
{
	map.clear();
}
