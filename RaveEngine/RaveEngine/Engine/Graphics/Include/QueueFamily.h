#pragma once
#include "Engine/Include/CommonIncludes.h"
#include "Engine/Utilities/Include/ArrayView.h"
#include <array>

namespace rave
{
	enum QueueFamilyType
	{
		RE_QF_GRAPHICS = 0,
		RE_QF_NELEMENTS
	};

	class QueueFamily
	{
	public:
		void Load(const VkPhysicalDevice device, const_array_view<QueueFamilyType> view)
		{
			uint32_t queueFamilyCount = 0;
			vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

			std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
			vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

			int i = 0;
			for (const auto& queueFamily : queueFamilies)
			{
				indices[queueFamily.queueFlags] = i;

//				if (indices.IsComplete())
				{
					break;
				}

				i++;
			}
		}

		unsigned char Get(const QueueFamilyType& flag) const noexcept
		{
			return indices[flag];
		}

	private:
		std::array<unsigned char, RE_QF_NELEMENTS> indices;
	};
}