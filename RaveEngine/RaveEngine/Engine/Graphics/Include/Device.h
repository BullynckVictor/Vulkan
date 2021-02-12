#pragma once
#include "Engine/Include/CommonIncludes.h"
#include "Engine/Utilities/Include/VulkanPointer.h"
#include "Engine/Utilities/Include/Result.h"
#include "Engine/Graphics/Include/Instance.h"

namespace rave
{
	class Device
	{
	public:
		Device();

		Result Load(const VulkanInstance& instance)
		{
			uint32_t deviceCount = 0;
			vkEnumeratePhysicalDevices(instance.Get(), &deviceCount, nullptr);

			if (deviceCount == 0)
				return Result(L"Failed to find GPUs with Vulkan support!", RE_FAIL, RE_DEVICE_PICK_FAIL);

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

				//QueueFamilyIndices indices = findQueueFamilies(d);

				//if (!indices.IsComplete())
				//	continue;

				if (score && score >= max)
				{
					physicalDevice = d;
					max = score;
				}
			}

			if (physicalDevice == VK_NULL_HANDLE)
				return Result(L"Failed to find a suitable GPU!", RE_FAIL, RE_DEVICE_PICK_FAIL);
		}

		VkDevice				Get()			noexcept;
		VkPhysicalDevice		GetPhysicale()	noexcept;
		const VkDevice			Get()			const noexcept;
		const VkPhysicalDevice	GetPhysicale()	const noexcept;

	private:
		VkDevice device = VK_NULL_HANDLE;
		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	};
}