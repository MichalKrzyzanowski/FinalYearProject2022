#include "Rect.h"

Rect::Rect(const Vector2f& pos, float width, float height, SDL_Color color, SDL_Color debugColor) :
	m_position{ pos },
	m_width{ width },
	m_height{ height },
	m_color{ color },
	m_debugColor{ debugColor }
{
	m_debugRect.x = m_position.x;
	m_debugRect.y = m_position.y;
	m_debugRect.w = m_width;
	m_debugRect.h = m_height;

	m_points[0].x = m_position.x;
	m_points[0].y = m_position.y;

	m_points[1].x = m_position.x + m_width;
	m_points[1].y = m_position.y;

	m_points[2].x = m_position.x + m_width;
	m_points[2].y = m_position.y + m_height;

	m_points[3].x = m_position.x;
	m_points[3].y = m_position.y + m_height;

	m_center.x = m_debugRect.x + m_width / 2.0f;
	m_center.y = m_debugRect.y + m_height / 2.0f;
}

void Rect::update()
{
	//move(Vector2f{ 0.4f, 0.4f });
	//rotate();

	//setPosition(Vector2f{ 200.0f, 100.0f });
}

void Rect::render(SDL_Renderer* renderer)
{
	SDL_SetRenderDrawColor(renderer, m_debugColor.r, m_debugColor.g, m_debugColor.b, m_debugColor.a);
	//SDL_RenderDrawRectF(renderer, &m_debugRect);

	SDL_SetRenderDrawColor(renderer, m_color.r, m_color.g, m_color.b, m_color.a);
	//SDL_RenderDrawPointsF(renderer, m_points, m_POINT_COUNT);
	renderLines(renderer);
}

void Rect::rotate(float radians)
{
	for (SDL_FPoint& point : m_points)
	{
		rotatePoint(m_center.x, m_center.y, radians, point);
	}
}

void Rect::move(const Vector2f& velocity)
{
	m_debugRect.x += velocity.x;
	m_debugRect.y += velocity.y;

	for (SDL_FPoint& point : m_points)
	{
		point.x += velocity.x;
		point.y += velocity.y;
	}

	m_center.x = m_debugRect.x + m_width / 2.0f;
	m_center.y = m_debugRect.y + m_height / 2.0f;

	m_position.x = m_debugRect.x;
	m_position.y = m_debugRect.y;
}

void Rect::renderLines(SDL_Renderer* renderer)
{
	SDL_RenderDrawLinesF(renderer, m_points, m_POINT_COUNT);

	// draw final line connecting to point 1
	SDL_RenderDrawLineF(renderer, m_points[3].x, m_points[3].y, m_points[0].x, m_points[0].y);
}

void Rect::setPosition(const Vector2f& pos)
{
	Vector2f diff;
	diff.x = pos.x - m_position.x;
	diff.y = pos.y - m_position.y;

	move(diff);
}

void Rect::setPosition(float x, float y)
{
	Vector2f diff;
	diff.x = x - m_position.x;
	diff.y = y - m_position.y;

	move(diff);
}

void Rect::rotatePoint(float cx, float cy, float angle, SDL_FPoint& point)
{
	point.x -= cx;
	point.y -= cy;

	float newX = point.x * cosf(angle) - point.y * sinf(angle);
	float newY = point.x * sinf(angle) + point.y * cosf(angle);

	point.x = newX + cx;
	point.y = newY + cy;
}
