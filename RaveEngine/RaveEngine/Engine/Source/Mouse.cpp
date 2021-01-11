#include "Engine/Include/Mouse.h"

rave::Point rave::Mouse::GetPosition() const noexcept
{
    double x, y;
    glfwGetCursorPos(pWindow, &x, &y);
    return Point((int)floor(x), (int)floor(y));
}

void rave::Mouse::Hide() const noexcept
{
    glfwSetInputMode(pWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
}

void rave::Mouse::Disable() const noexcept
{
    glfwSetInputMode(pWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void rave::Mouse::Restore() const noexcept
{
    glfwSetInputMode(pWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

rave::Mouse::State rave::Mouse::GetState() const noexcept
{
    switch(glfwGetInputMode(pWindow, GLFW_CURSOR))
    {
    case GLFW_CURSOR_HIDDEN:
        return State::Hidden;

    case GLFW_CURSOR_DISABLED:
        return State::Disabled;

    default:
        return State::Normal;
    }
}
bool rave::Mouse::ButtonIsPressed(const Button button) const noexcept
{
    int ibutton = 0;
    switch (button)
    {
        case Button::Left:      ibutton = GLFW_MOUSE_BUTTON_LEFT;   break;
        case Button::Right:     ibutton = GLFW_MOUSE_BUTTON_RIGHT;  break;
        case Button::Middle:    ibutton = GLFW_MOUSE_BUTTON_MIDDLE; break;
    }
    return glfwGetMouseButton(pWindow, ibutton) == GLFW_PRESS;
}

int rave::Mouse::GetScroll() noexcept
{
    const int temp = scroll;
    scroll = 0;
    return temp;
}

int rave::Mouse::PeekScroll() const noexcept
{
    return scroll;
}
