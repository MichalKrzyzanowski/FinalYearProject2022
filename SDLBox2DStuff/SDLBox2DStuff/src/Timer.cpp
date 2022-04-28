#include "./include/Timer.h"

/// <summary>
/// custom timer setup
/// </summary>
Timer::Timer()
{
	restart();
}

/// <summary>
/// timer start
/// </summary>
void Timer::start()
{
	m_startTicks = SDL_GetTicks();
}

/// <summary>
/// restart timer
/// </summary>
void Timer::restart()
{
	m_startTicks = 0;
	start();
}

/// <summary>
/// get timer ticks
/// </summary>
/// <returns>ticks</returns>
uint32_t Timer::getTicks()
{
	return SDL_GetTicks() - m_startTicks;
}

/// <summary>
/// get timer ticks in seconds
/// </summary>
/// <returns>ticks in seconds</returns>
float Timer::getTicksAsSeconds()
{
	return getTicks() / 1000.0f;
}
