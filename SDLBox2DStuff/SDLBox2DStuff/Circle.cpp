#include "Circle.h"

Circle::Circle(Vector2f position, int32_t radius) :
	m_position{ position },
	m_radius{ radius }
{

}

void Circle::render(SDL_Renderer* renderer)
{
	int32_t x = 0;
	int32_t y = m_radius;
	int32_t decision = 3 - (2 * m_radius);

	SDL_RenderDrawPoint(renderer, m_position.x + x, m_position.y + y);
	SDL_RenderDrawPoint(renderer, m_position.x - x, m_position.y + y);
	SDL_RenderDrawPoint(renderer, m_position.x + x, m_position.y - y);
	SDL_RenderDrawPoint(renderer, m_position.x - x, m_position.y - y);

	SDL_RenderDrawPoint(renderer, m_position.x + y, m_position.y + x);
	SDL_RenderDrawPoint(renderer, m_position.x - y, m_position.y + x);
	SDL_RenderDrawPoint(renderer, m_position.x + y, m_position.y - x);
	SDL_RenderDrawPoint(renderer, m_position.x - y, m_position.y - x);

	while (y >= x)
	{
		++x;
		if (decision > 0)
		{
			--y;
			decision = decision + 4 * (x - y) + 10;
		}
		else
		{
			decision = decision + 4 * x + 6;
		}

		SDL_RenderDrawPoint(renderer, m_position.x + x, m_position.y + y);
		SDL_RenderDrawPoint(renderer, m_position.x - x, m_position.y + y);
		SDL_RenderDrawPoint(renderer, m_position.x + x, m_position.y - y);
		SDL_RenderDrawPoint(renderer, m_position.x - x, m_position.y - y);

		SDL_RenderDrawPoint(renderer, m_position.x + y, m_position.y + x);
		SDL_RenderDrawPoint(renderer, m_position.x - y, m_position.y + x);
		SDL_RenderDrawPoint(renderer, m_position.x + y, m_position.y - x);
		SDL_RenderDrawPoint(renderer, m_position.x - y, m_position.y - x);
	}
}

