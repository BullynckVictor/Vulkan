#pragma once
#include <array>
#include <stdexcept>
#include "RandomAccessIterator.h"

namespace rave
{
	template<typename T>
	static constexpr bool is_char()
	{
		if constexpr (
			std::is_same<T, char>				    ::value ||
			std::is_same<T, unsigned char>		    ::value ||
			std::is_same<T, wchar_t>			    ::value ||
			std::is_same<T, const char>			    ::value ||
			std::is_same<T, const unsigned char>    ::value ||
			std::is_same<T, const wchar_t>		    ::value
		)
		return true;
		return false;
	}

	template<typename T>
	class array_view
	{
	public:
		using value_type				= T;
		using reference					= T&;
		using pointer					= T*;
		using size_type					= std::size_t;
		using difference_type			= std::ptrdiff_t;
		using iterator					= RandomAccessIterator<T>;
		using const_iterator			= RandomAccessIterator<const T>;
		using reverse_iterator			= std::reverse_iterator<iterator>;
		using const_reverse_iterator	= std::reverse_iterator<const_iterator>;

		array_view() noexcept
			:
			m_data(nullptr),
			m_size(0)
		{}
		array_view(pointer ptr)
			:
			m_data(ptr),
			m_size(0)
		{
			static_assert(is_char<T>());
			if constexpr (sizeof T == 1)
				m_size = strlen(reinterpret_cast<const char*>(ptr));
			else
				m_size = wcslen(reinterpret_cast<const wchar_t*>(ptr));
		}
		array_view(pointer ptr, const size_type size) noexcept
			:
			m_data(ptr),
			m_size(size)
		{}
		array_view(const array_view& av)
			:
			m_data(av.m_data),
			m_size(av.m_size)
		{}
		array_view(array_view&& av)
			:
			m_data(av.m_data),
			m_size(av.m_size)
		{}
		array_view(const const_iterator b, const const_iterator e)
			:
			m_data(&*b),
			m_size(e - b)
		{}

		array_view& operator= (const array_view& av)
		{
			m_data = av.m_data;
			m_size = av.m_size;
		}
		array_view& operator= (array_view&& av)
		{
			m_data = av.m_data;
			m_size = av.m_size;
		}
		array_view& operator= (pointer ptr)
		{
			static_assert(is_char<T>());
			if constexpr (std::is_same_v<value_type, const char>)
				m_size = strlen(ptr);
			else
				m_size = wcslen(ptr);
		}

		reference at(size_type pos)
		{
			if (pos >= m_size)
				throw(std::out_of_range("cannot access element outside of bounds"));
			return m_data[pos];
		}
		const reference at(size_type pos) const
		{
			if (pos >= m_size)
				throw(std::out_of_range("cannot access element outside of bounds"));
			return m_data[pos];
		}

		reference operator[] (size_type pos)				{ return m_data[pos]; }
		const reference operator[] (size_type pos) const	{ return m_data[pos]; }

		pointer data() noexcept								{ return m_data; }
		const pointer data() const noexcept					{ return m_data; }

		reference front()									{ return *m_data; }
		reference back()									{ return  m_data[m_size - 1]; }

		const reference front() const						{ return *m_data; }
		const reference back()  const						{ return  m_data[m_size - 1]; }

		iterator begin() noexcept							{ return m_data; }
		iterator end()	 noexcept							{ return m_data + m_size; }
		const_iterator begin()	const noexcept				{ return m_data; }
		const_iterator end()	const noexcept				{ return m_data + m_size; }
		const_iterator cbegin()	const noexcept				{ return m_data; }
		const_iterator cend()	const noexcept				{ return m_data + m_size; }

		reverse_iterator rbegin() noexcept					{ return reverse_iterator{ m_data + m_size }; }
		reverse_iterator rend()	  noexcept					{ return reverse_iterator{ m_data }; }
		const_reverse_iterator rbegin()	const noexcept		{ return const_reverse_iterator{ m_data + m_size }; }
		const_reverse_iterator rend()	const noexcept		{ return const_reverse_iterator{ m_data }; }

		size_type size() const noexcept { return m_size; }

	private:
		pointer		m_data;
		size_type	m_size;
	};

	template<typename T>
	using const_array_view = const array_view<const T>;
}