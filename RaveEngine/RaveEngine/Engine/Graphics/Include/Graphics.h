#pragma once
#include "Engine/Include/CommonIncludes.h"
#include "Engine//Utilities/Include/VulkanPointer.h"

namespace rave
{
	class Graphics
	{
	public:
		Graphics();
		~Graphics();

		Graphics(const Graphics& gfx) = delete;
		Graphics& operator=(const Graphics& gfx) = delete;

		void ValidateDevice(const VkSurfaceKHR surface) const;

	private:
		VkInstance instance = VK_NULL_HANDLE;
		VkDevice device = VK_NULL_HANDLE;
		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
		VkQueue graphicsQueue = VK_NULL_HANDLE;

#ifndef NDEBUG
		VkDebugUtilsMessengerEXT debugMessenger = {};
		static bool isListening;

	private:
		static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
		void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
#endif

		void createInstance();
		void pickPhysicalDevice();
		void createLogicalDevice();

		friend class GraphicsFriend;
	};

	struct GraphicsData
	{
		VkInstance instance = VK_NULL_HANDLE;
		VkDevice device = VK_NULL_HANDLE;
		VkQueue graphicsQueue = VK_NULL_HANDLE;
	};

	class GraphicsFriend
	{
	protected:
		static GraphicsData Expose(const Graphics& graphics) noexcept;
	};
}