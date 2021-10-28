#ifndef CONVEX_SHAPE_H
#define CONVEX_SHAPE_H

#include <vector>
#include <SDL_video.h>
#include <box2d.h>

// temp for now
struct Vector2f
{
	float x, y;
};

class ConvexShape
{
private:

	std::vector<SDL_FPoint> m_points;
	SDL_Color m_color;

	Vector2f m_position;
	float m_width;
	float m_height;

	b2World* m_world;

	b2BodyDef m_b2BodyDef;
	b2Body* m_b2Body;
	b2PolygonShape m_b2Shape;
	b2FixtureDef m_b2FixtureDef;

public:
	ConvexShape(b2World* world);
	~ConvexShape();
};
#endif // !CONVEX_SHAPE_H

