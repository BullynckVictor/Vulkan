#include "Engine/Include/GLFWManager.h"
#include "Engine/Include/CommonIncludes.h"
#include "Engine/Include/Window.h"

rave::GLFWManager::GLFWManager()
{
	glfwInit();
	for (int i = 0; i <= GLFW_KEY_LAST; i++)
	{
		const char* name = glfwGetKeyName(i, 0);
		if (name)
		{
			const unsigned char j = static_cast<unsigned char>(name[0]);
			Keyboard::keymap[j] = i;
		}
	}
	rave::errors.Clear();
}

rave::GLFWManager::~GLFWManager()
{
	glfwTerminate();
	windowManager.Cleanup();
}
