#include "./include/Timer.h"

Timer::Timer()
{
	restart();
}

void Timer::start()
{
	m_startTicks = SDL_GetTicks();
}

void Timer::restart()
{
	m_startTicks = 0;
	start();
}

uint32_t Timer::getTicks()
{
	return SDL_GetTicks() - m_startTicks;
}

float Timer::getTicksAsSeconds()
{
	return getTicks() / 1000.0f;
}
