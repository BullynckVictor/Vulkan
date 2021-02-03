#include "Engine/Graphics/Include/Graphics.h"
#include "Engine/Utilities/Include/String.h"
#include <optional>

rave::ApplicationData rave::static_application = {};

#ifndef NDEBUG

const char* validationLayers[] = {
	"VK_LAYER_KHRONOS_validation"
};

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
	vkEnumeratePhysicalDevices(instance.Get(), &deviceCount, nullptr);

	if (deviceCount == 0)
	{
		rave_throw_message(L"Failed to find GPUs with Vulkan support!");
	}

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(instance.Get(), &deviceCount, devices.data());

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
	:
	instance(validationLayers, (unsigned int)std::size(validationLayers))
{
	pickPhysicalDevice();
	createLogicalDevice();

	static_application.device = device;
	static_application.instance = instance.Get();
}

rave::Graphics::~Graphics()
{
	vkDestroyDevice(device, nullptr);

	static_application.device = NULL;
	static_application.instance = NULL;
}

void rave::Graphics::ValidateDevice(const VkSurfaceKHR surface) const
{
	QueueFamilyIndices indices = findQueueFamilies(physicalDevice, surface);
	if (!indices.presentFamily.has_value())
		rave_throw_message(L"Device not supported");
}

rave::GraphicsData rave::GraphicsFriend::Expose(const Graphics& graphics) noexcept
{
	GraphicsData data;
	data.instance = graphics.instance.Get();
	data.device = graphics.device;
	data.graphicsQueue = graphics.graphicsQueue;
	return data;
}
