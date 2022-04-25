#ifndef CONVEX_SHAPE_H
#define CONVEX_SHAPE_H

#include <vector>
#include <SDL.h>
#include <box2d.h>
#include "Utility.h"

enum class Type
{
	BLOCK,
	WALL,
	PLAYER,
	TARGET,
	BULLET
};

// temp struct for saving/loading
struct ShapeData
{
	SDL_Color color;
	Vector2f position;
	Type type;
	b2BodyType b2BodyType;
	int width;
	int height;
	float angle;
	int id;
	bool marked;
};

class ConvexShape
{
private:
	std::vector<b2Vec2> m_points; // for rendering

	ShapeData m_data;

	Vector2f m_center;

	bool m_marked;
	bool m_active;

	b2World* m_world;

	b2BodyDef m_b2BodyDef;
	b2Body* m_b2Body;
	b2PolygonShape m_b2Shape;
	b2FixtureDef m_b2FixtureDef;

public:
	// rect constructor
	ConvexShape(b2World* world,
		Vector2f topLeftPosition,
		float width,
		float height,
		b2BodyType b2Type,
		Type type = Type::BLOCK,
		SDL_Color color = SDL_Color{ 0, 0, 0, 255 },
		float angle = 0.0f,
		int id = 0,
		bool marked = false);
	~ConvexShape();

	void renderShadow(SDL_Renderer* renderer, Vector2f position);
	void renderLines(SDL_Renderer* renderer, std::vector<SDL_FPoint>* points);
	void launch(b2Vec2 direction, float power);

	int width() { return m_data.width; }
	int height() { return m_data.height; }
	b2BodyType b2BodyDefType() { return m_b2BodyDef.type; }
	b2Body* b2Body() { return m_b2Body; }

	b2Vec2 position() { return m_b2Body->GetPosition(); }
	Vector2f staticPosition() { return m_data.position; }
	float angle() { return m_b2Body->GetAngle(); }

	Type& type() { return m_data.type; }
	SDL_Color& color() { return m_data.color; }
	ShapeData data() { return m_data; }
	bool& marked() { return m_marked; }
	bool awake() { return m_b2Body->IsAwake(); }
	bool& active() { return m_active; }
};
#endif // !CONVEX_SHAPE_H

