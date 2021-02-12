#pragma once
#include "Engine/Include/CommonIncludes.h"
#include "Engine/Utilities/Include/VulkanPointer.h"

namespace rave
{
	class VulkanInstance
	{
	public:
		VulkanInstance(const char** validationLayers = nullptr, unsigned int size = 0);

		VkInstance operator->();
		VkInstance Get() noexcept;
		VkInstance* operator& () noexcept;
		const VkInstance operator->() const noexcept;
		const VkInstance Get() const noexcept;
		const VkInstance* operator& () const noexcept;

		~VulkanInstance() noexcept;

	private:
		VkInstance instance = VK_NULL_HANDLE;

#ifndef NDEBUG
		VkDebugUtilsMessengerEXT debugMessenger = {};
		static bool isListening;

	private:
		static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
		void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
#endif
	};
}