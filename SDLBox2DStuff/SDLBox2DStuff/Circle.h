#ifndef CIRCLE_H
#define CIRCLE_H

#include <SDL.h>
#include <vector>
#include "Utility.h"

class Circle
{
private:
	int32_t m_radius;
	Vector2f m_position;
	std::vector<SDL_Point> m_points{};

public:
	Circle(Vector2f position, int32_t radius);
	void setup();

	void renderMidPointAlgo(SDL_Renderer* renderer);
	void render(SDL_Renderer* renderer);

	SDL_Point& operator [](int index);

	Vector2f& position() { return m_position; }
	int32_t radius() { return m_radius; }

	int pointCount() { return m_points.size(); }
};

#endif // !CIRCLE_H

