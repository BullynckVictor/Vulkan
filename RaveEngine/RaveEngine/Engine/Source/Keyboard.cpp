#include "Engine/Include/Keyboard.h"

std::array<short, rave::Keyboard::kms> rave::Keyboard::keymap;

bool rave::Keyboard::KeyIsPressed(const int key)
{
	return glfwGetKey(pWindow, key) == GLFW_PRESS;
}

int rave::Keyboard::MapKey(const char key)
{
	rave_assert(key < kms);
	const unsigned char j = static_cast<unsigned char>(key);
	return (int)keymap[j];
}
