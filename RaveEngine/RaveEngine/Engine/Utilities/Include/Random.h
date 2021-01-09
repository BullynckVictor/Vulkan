#pragma once
#include <random>
#include <type_traits>

namespace rave
{
	static std::mt19937 rng{ std::random_device()() };

	template<typename T>
	T Random(const T from, const T to)
	{
		if constexpr (std::is_integral_v<T>)
		{
			if constexpr (sizeof (T) == 1)
			{
				std::uniform_int_distribution<short> dist(from, to);
				return static_cast<T>(dist(rng) & 0xFF);
			}
			else
			{
				std::uniform_int_distribution<T> dist(from, to);
				return dist(rng);
			}
		}
		else
		{
			std::uniform_real_distribution<T> dist(from, to);
			return dist(rng);
		}
	}

	template<typename T>
	T Random(const T to = std::numeric_limits<T>::max())
	{
		return Random<T>(0, to);
	}

	template<typename C>
	void Randomise(C& container, const typename C::value_type& from, const typename C::value_type& to)
	{
		for (auto& element : container)
			element = Random<typename C::value_type>(from, to);
	}

	template<typename C>
	void Randomise(C& container, const typename C::value_type& to = std::numeric_limits<typename C::value_type>::max())
	{
		Randomise(container, 0, to);
	}
}