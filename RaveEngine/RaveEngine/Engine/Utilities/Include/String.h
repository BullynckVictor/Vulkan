#pragma once
#include <locale>
#include <string>

namespace rave
{
	static std::string Narrow(const std::wstring& string) noexcept
	{
		std::locale loc;
		const size_t length = string.size();

		std::string ret;
		ret.resize(length);
		std::use_facet<std::ctype<wchar_t>>(loc).narrow(string.c_str(), string.c_str() + length + 1, '?', ret.data());

		return ret;
	}

	static std::wstring Widen(const std::string& string) noexcept
	{
		std::locale loc;
		std::wstring ret;
		const size_t len = string.size();
		ret.resize(len);

		std::use_facet<std::ctype<wchar_t>>(loc).widen(string.c_str(),
			string.c_str() + len + 1,
			ret.data()
		);

		return ret;
	}

	constexpr size_t fnv1a_32(char const* s, size_t count)
	{
		return ((count ? fnv1a_32(s, count - 1) : 2166136261u) ^ s[count]) * 16777619u;
	}

	constexpr size_t HashString(std::string_view s)
	{
		return fnv1a_32(s.data(), s.length());
	}

	template<typename T>
	std::string ToString(const T& t)
	{
		std::stringstream ss;
		ss << t;
		return ss.str();
	}
}