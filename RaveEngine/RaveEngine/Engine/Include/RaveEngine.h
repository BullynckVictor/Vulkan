#pragma once
#include "Engine/Include/Platform.h"
#include "Engine/Include/GLFWManager.h"
#include "Engine/Include/Window.h"

#include "Engine/Graphics/Include/Graphics.h"

#include "Engine/Utilities/Include/Timer.h"

#include <fstream>

namespace rave
{
	typedef void(*rave_main_func)(void);
	template<rave_main_func F>
	void Try()
	{
		if constexpr (System::windows)
		{
			try
			{
				rave::GLFWManager glfwMan;
				F();
			}
			catch (rave::Exception& e)
			{
				MessageBox(NULL, e.whide_what(), L"rave::Exception", MB_OK | MB_ICONEXCLAMATION);

				std::ofstream f("errors.txt");
				f << e.what();
			}
			catch (std::exception& e)
			{
				MessageBox(NULL, rave::Widen(e.what()).c_str(), L"std::exception", MB_OK | MB_ICONEXCLAMATION);

				std::ofstream f("errors.txt");
				f << e.what();
			}
			catch (...)
			{
				MessageBox(NULL, L"Unknown type caught", L"Unknown exception", MB_OK | MB_ICONEXCLAMATION);
			}
		}
	}
}