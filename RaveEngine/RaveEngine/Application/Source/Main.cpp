#include <iostream>

#include "Engine/Include/GLFWManager.h"
#include "Engine/Include/Window.h"

class HelloTriangleApplication
{
public:
    void run()
    {
        initWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }

    const uint32_t WIDTH = 800;
    const uint32_t HEIGHT = 600;

private:
    void initWindow()
    {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
    }

    void createInstance()
    {
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Hello Triangle";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;

        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        createInfo.enabledExtensionCount = glfwExtensionCount;
        createInfo.ppEnabledExtensionNames = glfwExtensions;
        createInfo.enabledLayerCount = 0;

        if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
            throw std::runtime_error("failed to create instance!");
    }

    void initVulkan()
    {
        createInstance();
    }

    void mainLoop()
    {
        while (!glfwWindowShouldClose(window))
        {
            glfwPollEvents();
            update();
        }
    }

    void cleanup()
    {
        vkDestroyInstance(instance, nullptr);

        glfwDestroyWindow(window);
    }

    void update()
    {

    }

    GLFWwindow* window = nullptr;
    VkInstance instance = {};
};

int rave_main()
{
    try
    {
        rave::GLFWManager glfwMan;

        rave::Window wnd = rave::Window("ello", 800, 600);

        while (wnd.IsOpen())
        {
            rave::Window::HandleMessages();
        }
    }
    rave_catch();
}