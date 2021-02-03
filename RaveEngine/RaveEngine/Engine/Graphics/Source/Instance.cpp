#include "Engine/Graphics/Include/Instance.h"
#include "Engine/Graphics/Include/VulkanFunctions.h"

#ifndef NDEBUG

bool rave::VulkanInstance::isListening = false;

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

VKAPI_ATTR VkBool32 VKAPI_CALL rave::VulkanInstance::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
	if (isListening)
		rave_throw_message(rave::Widen(pCallbackData->pMessage).c_str());
	return VK_FALSE;
}

void rave::VulkanInstance::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
	createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = debugCallback;
}

#endif

rave::VulkanInstance::VulkanInstance(const char** validationLayers, unsigned int size)
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
		createInfo.enabledLayerCount = static_cast<uint32_t>(size);
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

VkInstance rave::VulkanInstance::operator->()
{
	return instance;
}

VkInstance rave::VulkanInstance::Get() noexcept
{
	return instance;
}

const VkInstance rave::VulkanInstance::operator->() const noexcept
{
	return instance;
}

const VkInstance rave::VulkanInstance::Get() const noexcept
{
	return instance;
}

const VkInstance* rave::VulkanInstance::operator&() const noexcept
{
	return &instance;
}

rave::VulkanInstance::~VulkanInstance() noexcept
{
#ifndef NDEBUG
	DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
#endif
	vkDestroyInstance(instance, nullptr);
}

VkInstance* rave::VulkanInstance::operator&() noexcept
{
	return &instance;
}
