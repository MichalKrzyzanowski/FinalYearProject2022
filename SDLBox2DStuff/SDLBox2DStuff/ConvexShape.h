#ifndef CONVEX_SHAPE_H
#define CONVEX_SHAPE_H

#include <vector>
#include <SDL.h>
#include <box2d.h>
#include "Utility.h"

enum class Type
{
	BLOCK,
	PLAYER,
	TARGET
};

class ConvexShape
{
private:
	std::vector<SDL_FPoint> m_points; // for rendering
	SDL_Color m_color;

	Vector2f m_center;
	Vector2f m_staticPosition;
	float m_width;
	float m_height;

	Type m_type;

	b2World* m_world;

	b2BodyDef m_b2BodyDef;
	b2Body* m_b2Body;
	b2PolygonShape m_b2Shape;
	b2FixtureDef m_b2FixtureDef;

private:
	void setPosition(b2Vec2 position);
	void rotate(float radians);

public:
	// rect constructor
	ConvexShape(b2World* world,
		Vector2f topLeftPosition,
		float width,
		float height,
		b2BodyType b2Type,
		SDL_Color color = SDL_Color{ 0, 0, 0, 255 },
		Type type = Type::BLOCK);
	~ConvexShape();

	void update();
	void render(SDL_Renderer* renderer);
	void renderShadow(SDL_Renderer* renderer, Vector2f position);
	void renderLines(SDL_Renderer* renderer);
	void renderLines(SDL_Renderer* renderer, std::vector<SDL_FPoint>* points);
	void launch(b2Vec2 direction, float power);

	int width() { return m_width; }
	int height() { return m_height; }
	b2BodyType b2Body() { return m_b2BodyDef.type; }

	b2Vec2 position() { return m_b2Body->GetPosition(); }
	Vector2f staticPosition() { return m_staticPosition; }

	Type& type() { return m_type; }
	SDL_Color& color() { return m_color; }
};
#endif // !CONVEX_SHAPE_H

