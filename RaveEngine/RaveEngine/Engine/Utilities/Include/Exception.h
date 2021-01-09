#pragma once
#include <exception>
#include <sstream>
#include <deque>
#include "Engine/Utilities/Include/String.h"
#include "Engine/Include/CommonIncludes.h"

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
		const std::wstring wideWhatBuffer;
		const std::string whatBuffer;
	};

	class HrException : public Exception
	{
	public:
		HrException(const HRESULT hr, const char* file, const unsigned int line) noexcept;

	private:
		static std::wstring FormatHr(const HRESULT hr) noexcept;
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
		static GLFWErrorStream* GetStaticInstance() noexcept;

	private:
		static void errorFunction(const int i, const char* str);
		std::deque<std::string> queue;
	};

	extern GLFWErrorStream errors;

	bool FileExists(const char* file) noexcept;
	bool FileExists(const wchar_t* file) noexcept;
}

#define rave_throw_hr(hr) throw rave::HrException(hr, __FILE__, __LINE__)
#define rave_throw_file(file) throw rave::FileException(file, __FILE__, __LINE__)
#define rave_throw_message(info) throw rave::InfoException(info, __FILE__, __LINE__)
#define rave_throw_last() throw rave::HrException((HRESULT)GetLastError(), __FILE__, __LINE__)

#define rave_check_hr(hrCall) if(FAILED(hr = (hrCall))) throw rave::HrException(hr, __FILE__, __LINE__)
#define rave_check_file(file) if(!rave::FileExists(file)) throw rave::FileException(file, __FILE__, __LINE__)

#ifndef NDEBUG
#	define rave_assert(expression) if(!(expression)) throw rave::Assertion(#expression, __FILE__, __LINE__)
#	define rave_assert_info(expression, info) if(!(expression)) throw rave::Assertion(#expression, info, __FILE__, __LINE__)
#else
#	define rave_assert(expression)
#	define rave_assert_info(expression, info)
#endif

#define rave_catch()																				\
catch (rave::Exception& e)																		    \
{																								    \
	MessageBox(NULL, e.whide_what(), L"rave::Exception", MB_OK | MB_ICONEXCLAMATION);			    \
}																								    \
catch (std::exception& e)																		    \
{																								    \
	MessageBox(NULL, rave::Widen(e.what()).c_str(), L"std::exception", MB_OK | MB_ICONEXCLAMATION); \
}																								    \
catch (...)																						    \
{																								    \
	MessageBox(NULL, L"Unknown type caught", L"Unknown exception", MB_OK | MB_ICONEXCLAMATION);	    \
}																								    