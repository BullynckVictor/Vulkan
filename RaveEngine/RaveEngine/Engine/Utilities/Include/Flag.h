#pragma once
#include <cassert>

namespace rave
{
	template<typename T>
	struct Flag
	{
		constexpr Flag() : id(0) {}
		constexpr Flag(const unsigned int id) : id(id) {}
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

	template<typename T>
	static constexpr Flag<T> NewFlag(const size_t n)
	{
		assert(n < sizeof Flag<T> * 8);
		return (1 << n);
	}
}