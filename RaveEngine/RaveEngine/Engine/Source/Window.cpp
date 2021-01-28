#include "Engine/Include/Window.h"
#include "Engine/Include/ImageLoader.h"

rave::WindowManager rave::windowManager;

rave::Window::Window(const Graphics& gfx, const char* title, const unsigned int width, const unsigned int height, const char* iconName)
	:
	size(width, height)
{
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	window = glfwCreateWindow(width, height, title, nullptr, nullptr);
	errors.Throw();

	mouse.pWindow = window;
	kbd  .pWindow = window;

	windowManager.Add(window, this);

	glfwSetScrollCallback(window, ScrollCallback);
	
	if (iconName)
	{
		GLFWimage icon;
		std::vector<Color> data;
		unsigned int w, h;
		auto result = ReadImage(iconName, data, &w, &h);
		result.Throw();
		icon.pixels = reinterpret_cast<unsigned char*>(data.data());
		icon.width = (int)w;
		icon.height = (int)h;
		glfwSetWindowIcon(window, 1, &icon);
		errors.Throw();
	}

	VKR vkr;
	auto graphics = Expose(gfx);
	rave_check_vkr( glfwCreateWindowSurface(graphics.instance, window, nullptr, &surface) );
	gfx.ValidateDevice(surface.Get());


}

rave::Window::~Window() noexcept
{
	glfwDestroyWindow(window);
}

void rave::Window::HandleMessages()
{
	glfwPollEvents();
	errors.Throw();
}

bool rave::Window::IsOpen() const
{
	return !glfwWindowShouldClose(window);
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
	glfwSetWindowTitle(window, title_c);
}

void rave::Window::SetTitle(const std::string& title_c)
{
	title = title_c;
	glfwSetWindowTitle(window, title_c.c_str());
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
