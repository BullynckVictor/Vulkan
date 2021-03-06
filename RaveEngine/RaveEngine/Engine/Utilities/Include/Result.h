#pragma once
#include <string>
#include "Engine/Utilities/Include/Exception.h"
#include "Engine/Utilities/Include/ReturnCodes.h"

namespace rave
{
	static void CopyWString(wchar_t** out, const wchar_t* in)
	{
		if (!out)
			return;

		if (*out)
			delete[] *out;

		if (in)
		{
			size_t size = wcslen(in) + 1;
			*out = new wchar_t[size];
			memcpy(*out, in, size * sizeof (wchar_t));
		}
	}

	class Result
	{
	public:
		Result(const ReturnType& type = RE_SUCCESS, const ReturnCode& code = RE_NULL)
			:
			info(nullptr),
			flags(MakeReturnCode(type, code))
		{}
		Result(const wchar_t* message, const ReturnType& type = RE_FAIL, const ReturnCode& code = RE_NULL)
			:
			info(nullptr),
			flags(MakeReturnCode(type, code))
		{
			CopyWString(&info, message);
		}
		Result(const Result& result)
			:
			info(nullptr),
			flags(result.flags)
		{
			CopyWString(&info, result.info);
		}

		Result& operator= (const Result& rhs)
		{
			flags = rhs.flags;
			CopyWString(&info, rhs.info);
		}

		bool Succeeded() const noexcept
		{
			return GetType() != RE_FAIL;
		}
		bool Failed() const noexcept
		{
			return GetType() == RE_FAIL;
		}

		ReturnType GetType() const noexcept
		{
			return ReturnType(flags >> 30);
		}
		ReturnCode GetCode() const noexcept
		{
			return ReturnCode(flags & 0x3FFFFFFF);
		}
		const wchar_t* GetErrorMessage() const noexcept
		{
			return info;
		}

		std::wstring Format() const
		{
			std::wstringstream wss;
			ReturnType type = GetType();

			wss << L"Type: ";

			if (type == RE_SUCCESS)
				wss << L"[Success]\n";
			else if (type == RE_FAIL)
				wss << L"[Error]\n";
			else if (type == RE_WARNING)
				wss << L"[Warning]\n";
			else
				wss << L"[___UNINPLEMENTED___]\n";

			wss << L"Return Code: 0x" << std::hex << GetCode() << L'\n';
			wss << L"Message: " << (info ? info : L"No further information");

			return wss.str();
		}
		void Throw() const
		{
			if (Failed())
				rave_throw_message(Format().c_str());
		}

		~Result()
		{
			if (info)
				delete[] info;
		}

	private:
		static uint32_t MakeReturnCode(const ReturnType& type, const ReturnCode& code)
		{
			return (type << 30) | (code & 0x3FFFFFFF);
		}

		wchar_t* info;
		uint32_t flags;
	};

	template<typename T>
	class OptionalResult
	{
	public:
		OptionalResult()
			:
			value({}),
			result({})
		{
		}
		OptionalResult(const T& value)
			:
			value(value),
			result()
		{}
		OptionalResult(T&& value)
			:
			value(std::move(value)),
			result()
		{}
		OptionalResult(const OptionalResult& rhs)
			:
			value(rhs.value),
			result(rhs.result)
		{
		}
		OptionalResult(const Result& result)
			:
			value(),
			result(result)
		{
		}

		OptionalResult& operator= (const OptionalResult& rhs)
		{
			value = rhs.value;
			result = rhs.result;
		}
		T& GetAndCheck()
		{
			result.Throw();
			return value;
		}
		const T& GetAndCheck() const
		{
			result.Throw();
			return value;
		}
		T& Get() noexcept
		{
			return value;
		}
		const T& Get() const noexcept
		{
			return value;
		}


		const Result& GetResult() const noexcept
		{
			return result;
		}

	private:
		T value;
		Result result;
	};
}