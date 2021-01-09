#pragma once
#include <glm/common.hpp>

namespace rave
{
	template<size_t D, typename T>
	using Vector = glm::vec<D, T>;

	typedef Vector<2, float> Vector2;
	typedef Vector<3, float> Vector3;
	typedef Vector<4, float> Vector4;

	typedef Vector<2, unsigned int> Size;
}