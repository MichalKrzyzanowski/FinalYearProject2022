#ifndef TIMER_H
#define TIMER_H

#include <iostream>
#include <SDL_timer.h>
#include <cstdint>

class Timer
{
private:
	Uint32 m_startTicks;

public:
	Timer();

	void start();
	void restart();

	uint32_t getTicks();
	float getTicksAsSeconds();
};

#endif // !TIMER_H

