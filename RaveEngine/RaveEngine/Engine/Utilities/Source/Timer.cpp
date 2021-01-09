#include "Engine/Utilities/Include/Timer.h"

rave::Timer::Timer() noexcept
	:
	last(std::chrono::steady_clock::now())
{
}

float rave::Timer::Mark() noexcept
{
	const auto old = last;
	last = std::chrono::steady_clock::now();
	const std::chrono::duration<float> frameTime = last - old;
	return frameTime.count();
}

float rave::Timer::Peek() const noexcept
{
	return std::chrono::duration<float>(std::chrono::steady_clock::now() - last).count();
}