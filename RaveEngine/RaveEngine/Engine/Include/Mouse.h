#pragma once
#include "Engine/Include/CommonIncludes.h"
#include "Engine/Utilities/Include/Exception.h"
#include "Engine/Utilities/Include/Vector.h"

namespace rave
{
	class Mouse
	{
	public:
		enum class State
		{
			Normal,
			Hidden,
			Disabled
		};
		enum class Button
		{
			Left,
			Right,
			Middle
		};
		enum class ButtonState
		{
			Pressed,
			Released
		};

		Point GetPosition() const noexcept;
		void Hide() const noexcept;
		void Disable() const noexcept;
		void Restore() const noexcept;
		State GetState() const noexcept;
		bool ButtonIsPressed(const Button button) const noexcept;

		int GetScroll() noexcept;
		int PeekScroll() const noexcept;

	private:
		int scroll = 0;
		GLFWwindow* pWindow = nullptr;
		friend class Window;
	};
}