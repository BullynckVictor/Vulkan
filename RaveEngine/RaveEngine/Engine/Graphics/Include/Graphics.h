#pragma once
#include "Engine/Include/CommonIncludes.h"

namespace rave
{
	class Graphics
	{
	public:
		Graphics();
		~Graphics();

		Graphics(const Graphics& gfx) = delete;
		Graphics& operator=(const Graphics& gfx) = delete;

	private:
		VkInstance instance = {};
		VkPhysicalDevice physicaldevice = VK_NULL_HANDLE;
#ifndef NDEBUG
		VkDebugUtilsMessengerEXT debugMessenger = {};
		static bool isListening;

	private:
		static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
		void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
#endif
	};
}