#pragma once
#include "Engine/Include/CommonIncludes.h"
#include <memory>

namespace rave
{
	struct ApplicationData
	{
		VkInstance	instance;
		VkDevice	device;
	};

	extern ApplicationData static_application;

	template<typename T, typename T2, void (_stdcall *F)(T2, T, const VkAllocationCallbacks*)>
	struct VulkanPointer
	{
	private:
		typedef typename std::decay<decltype(*std::declval<T>())>::type base_type;
		typedef typename base_type* ptr_type;
		typedef typename base_type& ref_type;

	public:
		VulkanPointer()
			: 
			ptr(VK_NULL_HANDLE)
		{
		}
		VulkanPointer(const ptr_type ptr) 
			: 
			ptr(ptr)
		{
		}
		VulkanPointer(const VulkanPointer& obj) = delete;
		VulkanPointer& operator=(const VulkanPointer& obj) = delete;
		~VulkanPointer()
		{
			Release();
		}

		VulkanPointer(VulkanPointer&& dyingObj)
		{
			ptr = dyingObj.ptr;
			dyingObj.ptr = VK_NULL_HANDLE;
		}
		void operator=(VulkanPointer&& dyingObj)
		{
			ptr = dyingObj.ptr;
			dyingObj.ptr = VK_NULL_HANDLE;
		}

		operator bool() const
		{
			return ptr;
		}
		ptr_type Get()
		{
			return ptr;
		}

		ptr_type operator->()
		{
			return ptr;
		}
		ref_type operator*()
		{
			return *ptr;
		}

		ptr_type* operator& ()
		{
			Release();
			return &ptr;
		}
		ptr_type* GetAddress()
		{
			return &ptr;
		}

		void Release()
		{
			bool i = static_application.instance != (VkInstance)VK_NULL_HANDLE;
			bool j = ptr != VK_NULL_HANDLE;

			if constexpr (std::is_same_v<T2, VkInstance>)
				if (static_application.instance != VK_NULL_HANDLE && ptr != VK_NULL_HANDLE)
					F(static_application.instance, ptr, VK_NULL_HANDLE);

			if constexpr (std::is_same_v<T2, VkDevice>)
				if (static_application.device != VK_NULL_HANDLE && ptr != VK_NULL_HANDLE)
					F(static_application.device, ptr, VK_NULL_HANDLE);

			ptr = VK_NULL_HANDLE;
		}

	private:
		ptr_type ptr = VK_NULL_HANDLE;
	};

	namespace vk
	{
		typedef VulkanPointer<VkSurfaceKHR, VkInstance, vkDestroySurfaceKHR> SurfaceKHR;
	}
}