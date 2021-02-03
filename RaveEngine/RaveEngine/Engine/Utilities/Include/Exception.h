#pragma once
#include <exception>
#include <sstream>
#include <deque>
#include "Engine/Utilities/Include/String.h"
#include "Engine/Include/CommonIncludes.h"
#include "Libraries/stacktrace/call_stack.hpp"

namespace rave
{
	class Exception : public std::exception
	{
	public:
		Exception(const std::string& what) noexcept;
		Exception(const std::wstring& what) noexcept;

		const char* what() const noexcept override;
		const wchar_t* whide_what() const noexcept;

	protected:
		static std::wstring Format(const wchar_t* type, const char* file, const unsigned int line) noexcept;

	private:
		void DumpFile() const;

		const std::wstring wideWhatBuffer;
		const std::string whatBuffer;
		const stacktrace::call_stack callstack;
	};

	class VkException : public Exception
	{
	public:
		VkException(const VkResult result, const char* file, const unsigned int line) noexcept;

	private:
		static std::wstring FormatVkResult(const VkResult result) noexcept;
	};

	class FileException : public Exception
	{
	public:
		FileException(const char* file, const char* codefile, const unsigned int line) noexcept;
		FileException(const wchar_t* file, const char* codefile, const unsigned int line) noexcept;

	private:
		static std::wstring FormatFile(const char* file) noexcept;
		static std::wstring FormatFile(const wchar_t* file) noexcept;
	};

	class InfoException : public Exception
	{
	public:
		InfoException(const wchar_t* info, const char* file, const unsigned int line) noexcept;
	};

	class Assertion : public Exception
	{
	public:
		Assertion(const char* assertion, const char* file, const unsigned int line) noexcept;
		Assertion(const char* assertion, const wchar_t* info, const char* file, const unsigned int line) noexcept;

	private:
		static std::wstring FormatAssert(const char* assertion, const wchar_t* info) noexcept;
	};

	class GLFWErrorStream
	{
	public:
		GLFWErrorStream() noexcept;
		void Push(const int code, const char* message);
		void Throw();
		bool IsEmpty() const noexcept;
		void Clear() noexcept;
		static GLFWErrorStream* GetStaticsource() noexcept;

	private:
		static void errorFunction(const int i, const char* str);
		std::deque<std::string> queue;
	};

	extern GLFWErrorStream errors;

	bool FileExists(const char* file) noexcept;
	bool FileExists(const wchar_t* file) noexcept;

	static constexpr bool Succeeded(const VkResult result) noexcept
	{
		constexpr bool strict = true;
		if constexpr (!strict)
		{
			switch (result)
			{
			case VK_SUCCESS:
			case VK_NOT_READY:
			case VK_TIMEOUT:
			case VK_EVENT_SET:
			case VK_EVENT_RESET:
			case VK_INCOMPLETE:
			case VK_SUBOPTIMAL_KHR:
			case VK_THREAD_IDLE_KHR:
			case VK_THREAD_DONE_KHR:
			case VK_OPERATION_DEFERRED_KHR:
			case VK_OPERATION_NOT_DEFERRED_KHR:
			case VK_PIPELINE_COMPILE_REQUIRED_EXT:
				return true;
			}
			return false;
		}
		return result == VK_SUCCESS;
	}

	typedef VkResult VKR;
}

#define rave_throw_vk(vk) throw rave::VkException(vk, __FILE__, __LINE__)
#define rave_throw_file(file) throw rave::FileException(file, __FILE__, __LINE__)
#define rave_throw_message(info) throw rave::InfoException(info, __FILE__, __LINE__)
#define rave_throw_last() throw rave::HrException((HRESULT)GetLastError(), __FILE__, __LINE__)

#define rave_check_vkr(vkCall) if(!Succeeded(vkr = (vkCall))) throw rave::VkException(vkr, __FILE__, __LINE__)
#define rave_check_file(file) if(!rave::FileExists(file)) throw rave::FileException(file, __FILE__, __LINE__)

#ifndef NDEBUG
#	define rave_assert(expression) if(!(expression)) throw rave::Assertion(#expression, __FILE__, __LINE__)
#	define rave_assert_info(expression, info) if(!(expression)) throw rave::Assertion(#expression, info, __FILE__, __LINE__)
#else
#	define rave_assert(expression)
#	define rave_assert_info(expression, info)
#endif
