#include "Engine/Graphics/Include/Graphics.h"
#include "Engine/Utilities/Include/String.h"
#include <optional>

rave::ApplicationData rave::static_application = {};

#if 1

#ifndef NDEBUG

const char* validationLayers[] = {
	"VK_LAYER_KHRONOS_validation"
};

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
	std::optional<uint32_t> presentFamily;

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

void rave::Graphics::createInstance()
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

	createInfo.enabledExtensionCount = (uint32_t)glfwExtensions.size();
	createInfo.ppEnabledExtensionNames = glfwExtensions.data();
	createInfo.enabledLayerCount = 0;

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
#ifndef NDEBUG
	{
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

	rave_check_vkr(vkCreateInstance(&createInfo, nullptr, &instance));

	isListening = true;

#ifndef NDEBUG
	rave_check_vkr(CreateDebugUtilsMessengerEXT(instance, &debugCreateInfo, nullptr, &debugMessenger));
#endif
}

QueueFamilyIndices findQueueFamilies(const VkPhysicalDevice d, const VkSurfaceKHR surface = VK_NULL_HANDLE)
{
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(d, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(d, &queueFamilyCount, queueFamilies.data());

	int i = 0;
	for (const auto& queueFamily : queueFamilies) 
	{
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) 
		{
			indices.graphicsFamily = i;
		}

		if (surface)
		{
			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(d, i, surface, &presentSupport);

			if (presentSupport)
			{
				indices.presentFamily = i;
			}
		}

		if (indices.IsComplete()) 
		{
			break;
		}

		i++;
	}

	return indices;
}

void rave::Graphics::pickPhysicalDevice()
{
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

		QueueFamilyIndices indices = findQueueFamilies(d);

		if (!indices.IsComplete())
			continue;

		if (score && score >= max)
		{
			physicalDevice = d;
			max = score;
		}
	}

	if (physicalDevice == VK_NULL_HANDLE)
	{
		rave_throw_message(L"Failed to find a suitable GPU!");
	}
}

void rave::Graphics::createLogicalDevice()
{
	VKR vkr;

	QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

	VkDeviceQueueCreateInfo queueCreateInfo{};
	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
	queueCreateInfo.queueCount = 1;
	float queuePriority = 1.0f;
	queueCreateInfo.pQueuePriorities = &queuePriority;

	VkPhysicalDeviceFeatures deviceFeatures{};

	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pQueueCreateInfos = &queueCreateInfo;
	createInfo.queueCreateInfoCount = 1;
	createInfo.pEnabledFeatures = &deviceFeatures;

	createInfo.enabledExtensionCount = 0;

#ifndef NDEBUG
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(std::size(validationLayers));
		createInfo.ppEnabledLayerNames = validationLayers;
	}
#else
	{
		createInfo.enabledLayerCount = 0;
	}
#endif

	rave_check_vkr(vkCreateDevice(physicalDevice, &createInfo, nullptr, &device));
	vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
}

rave::Graphics::Graphics()
{
	createInstance();
	pickPhysicalDevice();
	createLogicalDevice();

	static_application.device = device;
	static_application.instance = instance;
}

rave::Graphics::~Graphics()
{
	vkDestroyDevice(device, nullptr);
#ifndef NDEBUG
		DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
#endif
    vkDestroyInstance(instance, nullptr);

	static_application.device = NULL;
	static_application.instance = NULL;
}

void rave::Graphics::ValidateDevice(const VkSurfaceKHR surface) const
{
	QueueFamilyIndices indices = findQueueFamilies(physicalDevice, surface);
	if (!indices.presentFamily.has_value())
		rave_throw_message(L"Device not supported");
}

#endif

rave::GraphicsData rave::GraphicsFriend::Expose(const Graphics& graphics) noexcept
{
	GraphicsData data;
	data.instance = graphics.instance;
	data.device = graphics.device;
	data.graphicsQueue = graphics.graphicsQueue;
	return data;
}
