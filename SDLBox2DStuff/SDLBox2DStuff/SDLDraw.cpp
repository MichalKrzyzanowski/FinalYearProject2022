#include "SDLDraw.h"

void SDLDraw::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
}

/// <summary>
/// renders a solid polygon with SDL2 using shape data provided by the b2Draw Box2D API
/// </summary>
/// <param name="vertices">shape vertices</param>
/// <param name="vertexCount">shape vertex amount</param>
/// <param name="color">shape outline color</param>
/// <param name="fixture">shape fixture ref</param>
void SDLDraw::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color, b2Fixture* fixture)
{
	ConvexShape* data = reinterpret_cast<ConvexShape*>(fixture->GetBody()->GetUserData().pointer);
	SDL_SetRenderDrawColor(m_renderer,
		static_cast<Uint8>(color.r * 255),
		static_cast<Uint8>(color.g * 255),
		static_cast<Uint8>(color.b * 255),
		static_cast<Uint8>(color.a * 255));

	std::vector<SDL_FPoint> points{};

	for (size_t i{}; i < vertexCount; i++)
	{
		points.push_back(SDL_FPoint{ vertices[i].x * SCALING_FACTOR, vertices[i].y * SCALING_FACTOR });
	}


	const SDL_Vertex triangleVertex[6] = {
		{
			{ vertices[0].x * SCALING_FACTOR, vertices[0].y * SCALING_FACTOR },
			{ data->color() },
			{ 0.f, 0.f }
		},
		{
			{ vertices[1].x * SCALING_FACTOR, vertices[1].y * SCALING_FACTOR },
			{ data->color() },
			{ 0.f, 0.f }
		},
		{
			{ vertices[2].x * SCALING_FACTOR, vertices[2].y * SCALING_FACTOR },
			{ data->color() },
			{ 0.f, 0.f }
		},
		{
			{ vertices[0].x * SCALING_FACTOR, vertices[0].y * SCALING_FACTOR },
			{ data->color() },
			{ 0.f, 0.f }
		},
{
			{ vertices[3].x * SCALING_FACTOR, vertices[3].y * SCALING_FACTOR },
			{ data->color() },
			{ 0.f, 0.f }
		},
{
			{ vertices[2].x * SCALING_FACTOR, vertices[2].y * SCALING_FACTOR },
			{ data->color() },
			{ 0.f, 0.f }
		}
	};



	SDL_RenderGeometry(m_renderer, nullptr, triangleVertex, 6, nullptr, 0);

	SDL_RenderDrawLinesF(m_renderer, points.data(), points.size());

	// draw a line from the last point to the first point
	SDL_RenderDrawLineF(m_renderer,
		points.data()[points.size() - 1].x,
		points.data()[points.size() - 1].y,
		points.data()[0].x,
		points.data()[0].y);
}

void SDLDraw::DrawCircle(const b2Vec2& center, float radius, const b2Color& color)
{
}

void SDLDraw::DrawSolidCircle(const b2Vec2& center, float radius, const b2Vec2& axis, const b2Color& color)
{
}

void SDLDraw::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color)
{
}

void SDLDraw::DrawTransform(const b2Transform& xf)
{
}

void SDLDraw::DrawPoint(const b2Vec2& p, float size, const b2Color& color)
{
}
