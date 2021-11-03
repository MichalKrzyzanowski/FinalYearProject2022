#ifndef CIRCLE_H
#define CIRCLE_H

#include <SDL.h>
#include "Utility.h"

class Circle
{
private:
	int32_t m_radius;
	Vector2f m_position;

public:
	Circle(Vector2f position, int32_t radius);
	
	void render(SDL_Renderer* renderer);
};

#endif // !CIRCLE_H

