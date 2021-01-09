#include "Engine/Utilities/Include/Exception.h"
#include <comdef.h>
#include <fstream>

rave::GLFWErrorStream rave::errors;

rave::Exception::Exception(const std::string& what_) noexcept
	:
	whatBuffer(what_),
	wideWhatBuffer(Widen(what_))
{
}

rave::Exception::Exception(const std::wstring& what_) noexcept
	:
	whatBuffer(Narrow(what_)),
	wideWhatBuffer(what_)
{
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

rave::HrException::HrException(const HRESULT hr, const char* file, const unsigned int line) noexcept
	:
	Exception(Format(L"rave::HrException", file, line) + FormatHr(hr))
{
}

std::wstring rave::HrException::FormatHr(const HRESULT hr) noexcept
{
	std::wstringstream wss;
	wss << L"HRESULT: " << std::hex << hr << L"\n"
		<< L"Description:\n"
		<< _com_error(hr).ErrorMessage();

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
	ss << "Error code: " << std::hex << code << "\n";
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

rave::GLFWErrorStream* rave::GLFWErrorStream::GetStaticInstance() noexcept
{
	return &errors;
}

void rave::GLFWErrorStream::errorFunction(const int i, const char* str)
{
	GetStaticInstance()->Push(i, str);
}
