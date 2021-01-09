#include "Engine/Include/GLFWManager.h"
#include "Engine/Include/CommonIncludes.h"

rave::GLFWManager::GLFWManager()
{
	glfwInit();
}

rave::GLFWManager::~GLFWManager()
{
	glfwTerminate();
}
