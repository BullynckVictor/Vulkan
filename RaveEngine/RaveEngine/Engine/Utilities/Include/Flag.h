#pragma once
#include <cassert>
#include <type_traits>

namespace rave
{
	template<typename T>
	struct Flag
	{
		constexpr Flag() : id(0) {}
		constexpr Flag(const unsigned int id) : id(id) {}
		constexpr Flag(const T& id) : id((unsigned int)id) { static_assert(std::is_enum_v<T>); }
		constexpr Flag(const Flag& other) : id(other.id) {}
		const Flag& operator= (const Flag& rhs) { id = rhs.id; return *this; }
		const bool operator== (const Flag& rhs) { return id == rhs.id; }
		constexpr bool Contains(const Flag& rhs) const { return id & rhs.id; };
		constexpr Flag operator| (const Flag& rhs) const
		{
			return id | rhs.id;
		}
		constexpr Flag operator+ (const Flag& rhs) const
		{
			return id | rhs.id;
		}
		constexpr Flag operator- (const Flag& rhs) const
		{
			return id xor (id & rhs.id);
		}
		Flag& operator|= (const Flag& rhs)
		{
			*this = *this | rhs;
			return*this;
		}
		Flag& operator+= (const Flag& rhs)
		{
			*this = *this + rhs;
			return*this;
		}
		
		Flag& operator-= (const Flag& rhs)
		{
			*this = *this - rhs;
			return*this;
		}
	private:
		unsigned int id;
	};
}