#include "ConvexShape.h"

ConvexShape::ConvexShape(b2World* world, Vector2f topLeftPosition, float width, float height, b2BodyType type) :
	m_world{ world },
	m_width{ width },
	m_height{ height },
	m_position{ topLeftPosition.x + m_width / 2.0f, topLeftPosition.y + m_width / 2.0f }
{
	// setup rect points
	m_points.push_back(SDL_FPoint{ topLeftPosition.x, topLeftPosition.y });
	m_points.push_back(SDL_FPoint{ topLeftPosition.x + width, topLeftPosition.y });
	m_points.push_back(SDL_FPoint{ topLeftPosition.x + width, topLeftPosition.y + height });
	m_points.push_back(SDL_FPoint{ topLeftPosition.x, topLeftPosition.y + height });

	// setup initial box2d shape
	m_b2BodyDef.type = type;
	m_b2BodyDef.position.Set(m_position.x / SCALING_FACTOR, m_position.y / SCALING_FACTOR);
	m_b2Body = m_world->CreateBody(&m_b2BodyDef);
	m_b2Shape.SetAsBox(m_width / 2.0f / SCALING_FACTOR, m_height / 2.0f / SCALING_FACTOR);


	if (m_b2BodyDef.type != b2_staticBody)
	{	
		// temp dynamic fixture
		m_b2FixtureDef.shape = &m_b2Shape;
		m_b2FixtureDef.density = 1.0f;
		m_b2FixtureDef.friction = 0.3f;
		m_b2Body->CreateFixture(&m_b2FixtureDef);
	}
	else
	{
		// static fixture
		m_b2Body->CreateFixture(&m_b2Shape, 0.0f);
	}
}

ConvexShape::~ConvexShape()
{
}

void ConvexShape::update()
{
	
}

void ConvexShape::render(SDL_Renderer* renderer)
{
	renderLines(renderer);
}

void ConvexShape::renderLines(SDL_Renderer* renderer)
{
	SDL_RenderDrawLinesF(renderer, m_points.data(), m_points.size());

	printf("Point: %f %f\n", m_points.data()[m_points.size() - 1].x, m_points.data()[m_points.size() - 1].y);

	SDL_RenderDrawLineF(renderer,
		m_points.data()[m_points.size() - 1].x,
		m_points.data()[m_points.size() - 1].y,
		m_points.data()[0].x,
		m_points.data()[0].y);
}
