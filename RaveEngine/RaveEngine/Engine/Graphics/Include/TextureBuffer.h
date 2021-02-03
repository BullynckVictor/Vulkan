#pragma once
#include <vector>
#include "Engine/Utilities/Include/Vector.h"
#include "Engine/Utilities/Include/RandomAccessIterator.h"
#include "Engine/Include/ImageLoader.h"
#include <algorithm>

namespace rave
{
	template<typename T>
	class TextureBuffer
	{
	public:
		TextureBuffer() = default;
		TextureBuffer(const int width, const int height)
		{
			Load(width, height);
		}
		TextureBuffer(const TextureBuffer& rhs)
			:
			data(rhs.IsActive() ? new T[rhs.size.x * rhs.size.y] : nullptr),
			size(rhs.IsActive() ? rhs.size : Size(0, 0))
		{
			if (rhs.IsActive())
				std::copy(rhs.begin(), rhs.end(), begin());
		}
		TextureBuffer(TextureBuffer&& rhs)
			:
			data(rhs.IsActive() ? new T[rhs.size.x * rhs.size.y] : nullptr),
			size(rhs.IsActive() ? rhs.size : Size(0, 0))
		{
			if (rhs.IsActive())
				std::move(rhs.begin(), rhs.end(), begin());
		}

		TextureBuffer& operator= (const TextureBuffer& rhs)
		{
			CleanUp();
			if (rhs.IsActive())
			{
				size = rhs.size;
				data = new T[size.x * size.y];
				std::copy(rhs.begin(), rhs.end(), begin());
			}
		}
		TextureBuffer& operator= (TextureBuffer&& rhs)
		{
			CleanUp();
			if (rhs.IsActive())
			{
				size = rhs.size;
				data = new T[size.x * size.y];
				std::move(rhs.begin(), rhs.end(), begin());
			}
		}

		void Load(const int width, const int height)
		{
			CleanUp();
			size.x = static_cast<unsigned int>(width);
			size.y = static_cast<unsigned int>(height);
			data = new Color[(size_t)size.x * (size_t)size.y];
		}
		void Load(const int width, const int height, const T& value)
		{
			Load(width, height);
			std::fill(begin(), end(), value);
		}

		Size GetSize() const noexcept
		{
			return size;
		}
		unsigned int GetLength() const noexcept
		{
			return size.x * size.y;
		}

		RandomAccessIterator<T> begin() noexcept
		{
			return data;
		}
		RandomAccessIterator<T> end() noexcept
		{
			return data + GetLength();
		}

		RandomAccessIterator<const T> begin() const noexcept
		{
			return data;
		}
		RandomAccessIterator<const T> end() const noexcept
		{
			return data + GetLength();
		}

		void Clear() noexcept
		{
			CleanUp();
		}

		bool IsActive() const noexcept
		{
			return data;
		}
		T* Data() noexcept
		{
			return data;
		}
		const T* Data() const noexcept
		{
			return data;
		}

		template<typename I>
		T& At(const I& x, const I& y)
		{
			rave_assert_info(x >= 0,	 L"Cannot get element with a negative x-value");
			rave_assert_info(x < size.x, L"Cannot get element with a x-value larger than or equal to width");
			rave_assert_info(y >= 0,	 L"Cannot get element with a negative y-value");
			rave_assert_info(y < size.y, L"Cannot get element with a y-value larger than or equal to height");

			static_assert(std::is_integral_v<I>);
			return (unsigned int)y * size.x + (unsigned int)x;
		}
		template<typename I>
		T& At(const Vector<2, I>& p)
		{
			return At(p.x, p.y);
		}

		template<typename I>
		const T& At(const I& x, const I& y) const
		{
			rave_assert_info(x >= 0,	 L"Cannot get element with a negative x-value");
			rave_assert_info(x < size.x, L"Cannot get element with a x-value larger than or equal to width");
			rave_assert_info(y >= 0,	 L"Cannot get element with a negative y-value");
			rave_assert_info(y < size.y, L"Cannot get element with a y-value larger than or equal to height");

			static_assert(std::is_integral_v<I>);
			return (unsigned int)y * size.x + (unsigned int)x;
		}
		template<typename I>
		const T& At(const Vector<2, I>& p) const
		{
			return At(p.x, p.y);
		}

		~TextureBuffer()
		{
			CleanUp();
		}

	protected:
		void CleanUp()
		{
			if (data)
				delete[] data;

			Size size = { 0, 0 };
			data = nullptr;
		}

		T* data = nullptr;
		Size size = { 0, 0 };
	};
}