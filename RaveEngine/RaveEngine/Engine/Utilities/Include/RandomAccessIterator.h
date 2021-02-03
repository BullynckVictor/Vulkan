#pragma once

namespace rave
{
	template<typename T>
	class RandomAccessIterator
	{
	public:
		using iterator_category = std::random_access_iterator_tag;
		using difference_type = std::ptrdiff_t;
		using value_type = T;
		using pointer = T*;
		using reference = T&;

		RandomAccessIterator(pointer data = nullptr) noexcept
			:
			m_data(data)
		{}
		RandomAccessIterator(const RandomAccessIterator& iterator) noexcept
			:
			m_data(iterator.m_data)
		{
		}
		RandomAccessIterator(RandomAccessIterator&& iterator) noexcept
			:
			m_data(iterator.m_data)
		{}

		RandomAccessIterator& operator= (const RandomAccessIterator& rhs) { m_data = rhs.m_data; return *this; }
		RandomAccessIterator& operator= (RandomAccessIterator&& rhs) { m_data = rhs.m_data; return *this; }

		RandomAccessIterator& operator+= (difference_type offset) noexcept
		{
			m_data += offset;
			return *this;
		}
		RandomAccessIterator& operator-= (difference_type offset) noexcept
		{
			m_data -= offset;
			return *this;
		}

		RandomAccessIterator operator+ (difference_type offset) const noexcept
		{
			return m_data + offset;
		}
		RandomAccessIterator operator- (difference_type offset) const noexcept
		{
			return m_data - offset;
		}

		RandomAccessIterator& operator++ () noexcept
		{
			++m_data;
			return *this;
		}
		RandomAccessIterator& operator-- () noexcept
		{
			--m_data;
			return *this;
		}

		RandomAccessIterator  operator++ (int) noexcept
		{
			RandomAccessIterator temp = m_data++;
			return temp;
		}
		RandomAccessIterator  operator-- (int) noexcept
		{
			RandomAccessIterator temp = m_data--;
			return temp;
		}


		difference_type operator- (RandomAccessIterator rhs) const noexcept { return m_data - rhs.m_data; }

		reference operator[] (difference_type offset) { return *(m_data + offset); }
		reference operator* () { return *m_data; }
		pointer operator->	() { return m_data; }

		bool operator<  (RandomAccessIterator rhs) const noexcept { return m_data < rhs.m_data; }
		bool operator>  (RandomAccessIterator rhs) const noexcept { return m_data > rhs.m_data; }
		bool operator<= (RandomAccessIterator rhs) const noexcept { return m_data <= rhs.m_data; }
		bool operator>= (RandomAccessIterator rhs) const noexcept { return m_data >= rhs.m_data; }

		bool operator== (RandomAccessIterator rhs) const noexcept { return m_data == rhs.m_data; }
		bool operator!= (RandomAccessIterator rhs) const noexcept { return m_data != rhs.m_data; }

	private:
		pointer m_data;
	};

	template<typename T>
	static RandomAccessIterator<T> operator+ (typename RandomAccessIterator<T>::difference_type offset, RandomAccessIterator<T> it) { return it + offset; }
}