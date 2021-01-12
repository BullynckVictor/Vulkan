#include "Engine/Graphics/Include/Graphics.h"
#include "Engine/Utilities/Include/String.h"
#include <optional>

#if 1

#ifndef NDEBUG

bool rave::Graphics::isListening = false;

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) 
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func) 
    {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
    else 
    {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) 
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func) 
    {
        func(instance, debugMessenger, pAllocator);
    }
}

VKAPI_ATTR VkBool32 VKAPI_CALL rave::Graphics::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) 
{
	if(isListening)
		rave_throw_message( rave::Widen(pCallbackData->pMessage).c_str() );
    return VK_FALSE;
}
#endif

struct QueueFamilyIndices 
{
    std::optional<uint32_t> graphicsFamily;

    bool IsComplete() const noexcept
    {
        return graphicsFamily.has_value();
    }
};

std::vector<const char*> getRequiredExtensions() 
{
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	if constexpr(rave::System::debug) 
	{
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return extensions;
}

bool checkValidationLayerSupport(const std::vector<const char*>& validationLayers) 
{
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char* layerName : validationLayers) 
	{
		bool layerFound = false;

		for (const auto& layerProperties : availableLayers) 
		{
			if (strcmp(layerName, layerProperties.layerName) == 0) 
			{
				layerFound = true;
				break;
			}
		}

		if (!layerFound) 
		{
			return false;
		}
	}

	return true;
}

void rave::Graphics::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) 
{
	createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = debugCallback;
}

rave::Graphics::Graphics()
{
    VKR vkr;

	isListening = false;

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "RaveEngine Application";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    auto glfwExtensions = getRequiredExtensions();

    createInfo.enabledExtensionCount = glfwExtensions.size();
    createInfo.ppEnabledExtensionNames = glfwExtensions.data();
    createInfo.enabledLayerCount = 0;

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
#ifndef NDEBUG
    {
        const char* validationLayers[] = {
            "VK_LAYER_KHRONOS_validation"
        };

        createInfo.enabledLayerCount = static_cast<uint32_t>(std::size(validationLayers));
        createInfo.ppEnabledLayerNames = validationLayers;

		populateDebugMessengerCreateInfo(debugCreateInfo);

        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
    }
#else
    {
        createInfo.enabledLayerCount = 0;

        createInfo.pNext = nullptr;
    }
#endif

    rave_check_vk(vkCreateInstance(&createInfo, nullptr, &instance));

	isListening = true;

#ifndef NDEBUG
        rave_check_vk(CreateDebugUtilsMessengerEXT(instance, &debugCreateInfo, nullptr, &debugMessenger));
#endif

    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

    if (deviceCount == 0) 
    {
        rave_throw_message(L"Failed to find GPUs with Vulkan support!");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    unsigned int max = 0;

    for (const auto& d : devices) 
    {
        unsigned int score = 0;

        VkPhysicalDeviceProperties deviceProperties;
        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceProperties(d, &deviceProperties);
        vkGetPhysicalDeviceFeatures(d, &deviceFeatures);

        // Discrete GPUs have a significant performance advantage
        if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) 
        {
            score += 1000;
        }

        // Maximum possible size of textures affects graphics quality
        score += deviceProperties.limits.maxImageDimension2D;

        if (!deviceFeatures.geometryShader)
            continue;

        QueueFamilyIndices indices;
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(d, &queueFamilyCount, nullptr);

		if (!queueFamilyCount)
			continue;

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(d, &queueFamilyCount, queueFamilies.data());

        int i = 0;
        for (const auto& queueFamily : queueFamilies)
        {
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                indices.graphicsFamily = i;
                if (indices.IsComplete())
                    break;
            }
            i++;
        }
		
        if (!indices.IsComplete())
            continue;

        if (score && score >= max)
        {
            physicaldevice = d;
            max = score;
        }
    }

    if (physicaldevice == VK_NULL_HANDLE) 
	{
        rave_throw_message(L"Failed to find a suitable GPU!");
    }
}

rave::Graphics::~Graphics()
{
#ifndef NDEBUG
		DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
#endif
    vkDestroyInstance(instance, nullptr);
}

#endif