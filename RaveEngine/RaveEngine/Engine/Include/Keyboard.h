#pragma once
#include "Engine/Include/CommonIncludes.h"
#include "Engine/Utilities/Include/Exception.h"
#include "Engine/Utilities/Include/Vector.h"

#include <deque>
#include <array>

namespace rave
{
	class Keyboard
	{
	public:
		bool KeyIsPressed(const int key);
		static int MapKey(const char key);

	private:
		static constexpr size_t kms = 256;
		static std::array<short, kms> keymap;
		GLFWwindow* pWindow = nullptr;
		friend class Window;
		friend class GLFWManager;
	};
}