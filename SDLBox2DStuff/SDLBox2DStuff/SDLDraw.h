#ifndef SDL_DRAW_H
#define SDL_DRAW_H

#include <vector>
#include <box2d.h>
#include <SDL.h>
#include "Utility.h"
#include "ConvexShape.h"

class SDLDraw : public b2Draw
{
private:
	SDL_Renderer* m_renderer{};

public:
	void setRenderer(SDL_Renderer* renderer) { m_renderer = renderer; }

	void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);
	void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color, b2Fixture* fixture);
	void DrawCircle(const b2Vec2& center, float radius, const b2Color& color);
	void DrawSolidCircle(const b2Vec2& center, float radius, const b2Vec2& axis, const b2Color& color);
	void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color);
	void DrawTransform(const b2Transform& xf);
	void DrawPoint(const b2Vec2& p, float size, const b2Color& color);
};

#endif // !SDL_DRAW_H

