#ifndef RECT_H
#define RECT_H

#include <SDL.h>
#include "Utility.h"

class Rect
{
private:
	const int m_POINT_COUNT{ 4 };

	SDL_FPoint m_points[4];
	SDL_FRect m_debugRect;

	float m_width;
	float m_height;
	SDL_Color m_color;
	SDL_Color m_debugColor;

	Vector2f m_position;
	Vector2f m_center;

public:
	Rect(const Vector2f& pos,
		float width,
		float height,
		SDL_Color color = SDL_Color{ 0, 0, 0, 255 },
		SDL_Color debugColor = SDL_Color{ 255, 0, 0, 255 });

	void update();
	void render(SDL_Renderer* renderer);

	void rotate(float radians);
	void move(const Vector2f& velocity);
	void renderLines(SDL_Renderer* renderer);

	Vector2f position() { return m_position; }
	float width() { return m_width; }
	float height() { return m_height; }

	void setPosition(const Vector2f& pos);
	void setPosition(float x, float y);

	void setRotation(float radians);

private:
	void rotatePoint(float cx, float cy, float angle, SDL_FPoint& point);
};

#endif // !RECT_H

