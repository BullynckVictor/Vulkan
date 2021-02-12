#include <iostream>
#include <fstream>

#include "Engine/Include/RaveEngine.h"
#include <unordered_map>

#include "Application/Include/VulkanApp.h"

void gameMain()
{
    rave::Graphics gfx;
    rave::Window wnd = rave::Window(gfx, "Rave Engine: Vulkan Edition", 800, 600, "Assets/icon.gif");

    while (wnd.IsOpen())
    {
        rave::Window::HandleMessages();
    }
}

void vulkanMain()
{
	vka::HelloTriangleApplication{}.run();
}

int rave_main()
{
    rave::Try<vulkanMain>();
}