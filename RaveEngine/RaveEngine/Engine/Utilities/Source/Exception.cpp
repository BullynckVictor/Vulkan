#include "Engine/Utilities/Include/Exception.h"
#include <fstream>
#include <iostream>

rave::GLFWErrorStream rave::errors;

rave::Exception::Exception(const std::string& what_) noexcept
	:
	whatBuffer(what_),
	wideWhatBuffer(Widen(what_))
{
	DumpFile();
}

rave::Exception::Exception(const std::wstring& what_) noexcept
	:
	whatBuffer(Narrow(what_)),
	wideWhatBuffer(what_)
{
	DumpFile();
}

const char* rave::Exception::what() const noexcept
{
	return whatBuffer.c_str();
}

const wchar_t* rave::Exception::whide_what() const noexcept
{
	return wideWhatBuffer.c_str();
}

std::wstring rave::Exception::Format(const wchar_t* type, const char* file, const unsigned int line) noexcept
{
	std::wstringstream wss;

	wss << type << L" occurred!\n\n"
		<< L"File: " << file << L"\n"
		<< L"Line: " << line << L"\n\n";

	return wss.str();
}

void rave::Exception::DumpFile() const
{
	std::wstringstream wss;
	wss << L"<************************************>\n\n" << wideWhatBuffer << "\n\nStack Trace:\n" << Widen(callstack.to_string()) << L"\n\n<************************************>\n";

	std::wofstream file("exceptions.txt");
	auto str = wss.str();
	file << str;

	if constexpr(System::windows)
		OutputDebugString(wss.str().c_str());
}

rave::VkException::VkException(const VkResult result, const char* file, const unsigned int line) noexcept
	:
	Exception(Format(L"rave::VkException", file, line).append(FormatVkResult(result)))
{
}

static constexpr const wchar_t* StringFromVkResult(const VkResult result)
{
	switch (result)
	{
		case VK_SUCCESS: 									return L"Command successfully completed";
		case VK_NOT_READY: 									return L"A fence or query has not yet completed";
		case VK_TIMEOUT: 									return L"A wait operation has not completed in the specified time";
		case VK_EVENT_SET: 									return L"An event is signaled";
		case VK_EVENT_RESET: 								return L"An event is unsignaled";
		case VK_INCOMPLETE: 								return L"A return array was too small for the result";
		case VK_SUBOPTIMAL_KHR: 							return L"A swapchain no longer matches the surface properties exactly, but can still be used to present to the surface successfully";
		case VK_THREAD_IDLE_KHR: 							return L"A deferred operation is not complete but there is currently no work for this thread to do at the time of this call";
		case VK_THREAD_DONE_KHR: 							return L"A deferred operation is not complete but there is no work remaining to assign to additional threads";
		case VK_OPERATION_DEFERRED_KHR:						return L"A deferred operation was requested and at least some of the work was deferred";
		case VK_OPERATION_NOT_DEFERRED_KHR: 				return L"A deferred operation was requested and no operations were deferred";
		case VK_PIPELINE_COMPILE_REQUIRED_EXT:				return L"A requested pipeline creation would have required compilation, but the application requested compilation to not be performed";
		case VK_ERROR_OUT_OF_HOST_MEMORY: 					return L"A host memory allocation has failed";
		case VK_ERROR_OUT_OF_DEVICE_MEMORY: 				return L"A device memory allocation has failed";
		case VK_ERROR_INITIALIZATION_FAILED: 				return L"Initialization of an object could not be completed for implementation-specific reasons";
		case VK_ERROR_DEVICE_LOST: 							return L"The logical or physical device has been lost. See Lost Device";
		case VK_ERROR_MEMORY_MAP_FAILED: 					return L"Mapping of a memory object has failed";
		case VK_ERROR_LAYER_NOT_PRESENT: 					return L"A requested layer is not present or could not be loaded";
		case VK_ERROR_EXTENSION_NOT_PRESENT: 				return L"A requested extension is not supported";
		case VK_ERROR_FEATURE_NOT_PRESENT: 					return L"A requested feature is not supported";
		case VK_ERROR_INCOMPATIBLE_DRIVER: 					return L"The requested version of Vulkan is not supported by the driver or is otherwise incompatible for implementation-specific reasons";
		case VK_ERROR_TOO_MANY_OBJECTS: 					return L"Too many objects of the type have already been created";
		case VK_ERROR_FORMAT_NOT_SUPPORTED: 				return L"A requested format is not supported on this device";
		case VK_ERROR_FRAGMENTED_POOL: 						return L"A pool allocation has failed due to fragmentation of the pool�s memory. This must only be returned if no attempt to allocate host or device memory was made to accommodate the new allocation. This should be returned in preference to VK_ERROR_OUT_OF_POOL_MEMORY, but only if the implementation is certain that the pool allocation failure was due to fragmentation";
		case VK_ERROR_SURFACE_LOST_KHR: 					return L"A surface is no longer available";
		case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR: 			return L"The requested window is already in use by Vulkan or another API in a manner which prevents it from being used again";
		case VK_ERROR_OUT_OF_DATE_KHR: 						return L"A surface has changed in such a way that it is no longer compatible with the swapchain, and further presentation requests using the swapchain will fail. Applications must query the new surface properties and recreate their swapchain if they wish to continue presenting to the surface";
		case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR: 			return L"The display used by a swapchain does not use the same presentable image layout, or is incompatible in a way that prevents sharing an image";
		case VK_ERROR_INVALID_SHADER_NV:					return L"One or more shaders failed to compile or link. More details are reported back to the application via VK_EXT_debug_report if enabled";
		case VK_ERROR_OUT_OF_POOL_MEMORY: 					return L"A pool memory allocation has failed. This must only be returned if no attempt to allocate host or device memory was made to accommodate the new allocation. If the failure was definitely due to fragmentation of the pool, VK_ERROR_FRAGMENTED_POOL should be returned instead";
		case VK_ERROR_INVALID_EXTERNAL_HANDLE: 				return L"An external handle is not a valid handle of the specified type";
		case VK_ERROR_FRAGMENTATION: 						return L"A descriptor pool creation has failed due to fragmentation";
		case VK_ERROR_INVALID_DEVICE_ADDRESS_EXT: 			return L"A buffer creation failed because the requested address is not available";
		case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT: 	return L"An operation on a swapchain created with VK_FULL_SCREEN_EXCLUSIVE_APPLICATION_CONTROLLED_EXT failed as it did not have exlusive full-screen access. This may occur due to implementation-dependent reasons, outside of the application�s control";
		case VK_ERROR_UNKNOWN: 								return L"An unknown error has occurred; either the application has provided invalid input, or an implementation failure has occurred";
	}
	return L"__NULL__";
}

std::wstring rave::VkException::FormatVkResult(const VkResult result) noexcept
{
	std::wstringstream wss;
	wss << L"VkResult: 0x" << std::hex << result << L"\n"
		<< L"Description:\n"
		<< StringFromVkResult(result);

	return wss.str();
}

rave::FileException::FileException(const char* file, const char* codefile, const unsigned int line) noexcept
	:
	Exception(Format(L"rave::FileException", codefile, line) + FormatFile(file))
{
}

rave::FileException::FileException(const wchar_t* file, const char* codefile, const unsigned int line) noexcept
	:
	Exception(Format(L"rave::FileException", codefile, line) + FormatFile(file))
{
}

std::wstring rave::FileException::FormatFile(const char* file) noexcept
{
	std::wstringstream wss;
	wss << L"Unable to open file ["
		<< file
		<< L"]\n";
	return wss.str();
}

std::wstring rave::FileException::FormatFile(const wchar_t* file) noexcept
{
	std::wstringstream wss;
	wss << L"Unable to open file ["
		<< file
		<< L"]\n";
	return wss.str();
}

rave::InfoException::InfoException(const wchar_t* info, const char* file, const unsigned int line) noexcept
	:
	Exception(Format(L"rave::InfoException", file, line) + info)
{
}

rave::Assertion::Assertion(const char* assertion, const char* file, const unsigned int line) noexcept
	:
	Exception(Format(L"rave::Assertion", file, line) + FormatAssert(assertion, nullptr))
{
}

rave::Assertion::Assertion(const char* assertion, const wchar_t* info, const char* file, const unsigned int line) noexcept
	:
	Exception(Format(L"rave::Assertion", file, line) + FormatAssert(assertion, info))
{
}

std::wstring rave::Assertion::FormatAssert(const char* assertion, const wchar_t* info) noexcept
{
	std::wstringstream wss;
	if(info)
		wss << L"Assertion [" << assertion << "] failed!\n" << info;
	else
		wss << L"Assertion [" << assertion << "] failed!\n";
	return wss.str();
}

bool rave::FileExists(const char* file) noexcept
{
	return std::ifstream(file).good();
}

bool rave::FileExists(const wchar_t* file) noexcept
{
	return std::wifstream(file).good();
}

rave::GLFWErrorStream::GLFWErrorStream() noexcept
{
	glfwSetErrorCallback(errorFunction);
}

void rave::GLFWErrorStream::Push(const int code, const char* message)
{
	std::stringstream ss;
	ss << "GLFWError occurred!\n\n";
	ss << "Error code: 0x" << std::hex << code << "\n";
	ss << "Description:\n";
	ss << message;

	queue.push_back(ss.str());
}

void rave::GLFWErrorStream::Throw()
{
	if (IsEmpty())
		return;

	std::string errormessage;
	for (const auto& e : queue)
		errormessage += e + '\n';
	
	errormessage.pop_back();

	throw(Exception(errormessage));
}

bool rave::GLFWErrorStream::IsEmpty() const noexcept
{
	return queue.empty();
}

void rave::GLFWErrorStream::Clear() noexcept
{
	queue.clear();
}

rave::GLFWErrorStream* rave::GLFWErrorStream::GetStaticsource() noexcept
{
	return &errors;
}

void rave::GLFWErrorStream::errorFunction(const int i, const char* str)
{
	GetStaticsource()->Push(i, str);
}