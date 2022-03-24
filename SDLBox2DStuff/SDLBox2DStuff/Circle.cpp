#include "Circle.h"

Circle::Circle(Vector2f position, int32_t radius) :
	m_position{ position },
	m_radius{ radius }
{
	m_points.reserve(360);

	setup();
}

void Circle::setup()
{
	m_points.clear();

	int px;
	int py;

	for (int i = 0; i < 360; i++)
	{
		px = m_position.x + m_radius * cos(i);
		py = m_position.y + m_radius * sin(i);
		m_points.push_back(SDL_Point{ px, py });
	}
}

void Circle::renderMidPointAlgo(SDL_Renderer* renderer)
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

void Circle::render(SDL_Renderer* renderer)
{
	SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
	for (auto point : m_points)
	{
		SDL_RenderDrawPoint(renderer, point.x, point.y);
	}
}

SDL_Point& Circle::operator[](int index)
{
	return m_points.at(index);
}

