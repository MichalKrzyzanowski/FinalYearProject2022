#include "ConvexShape.h"


ConvexShape::ConvexShape(b2World* world, Vector2f topLeftPosition, float width, float height, b2BodyType b2Type, SDL_Color color, Type type) :
	m_world{ world },
	m_width{ width },
	m_height{ height },
	m_center{ topLeftPosition.x + m_width / 2.0f, topLeftPosition.y + m_height / 2.0f }, // temporary center
	m_color{ color },
	m_type{ type }
{
	// setup rect points
	m_points.push_back(SDL_FPoint{ topLeftPosition.x, topLeftPosition.y });
	m_points.push_back(SDL_FPoint{ topLeftPosition.x + m_width, topLeftPosition.y });
	m_points.push_back(SDL_FPoint{ topLeftPosition.x + m_width, topLeftPosition.y + m_height });
	m_points.push_back(SDL_FPoint{ topLeftPosition.x, topLeftPosition.y + m_height });

	m_staticPosition = topLeftPosition;

	// setup initial box2d shape
	m_b2BodyDef.type = b2Type;
	m_b2BodyDef.position.Set((topLeftPosition.x + m_width / 2.0f) / SCALING_FACTOR, (topLeftPosition.y + m_height / 2.0f) / SCALING_FACTOR);
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

	m_center.x = m_b2Body->GetWorldCenter().x;
	m_center.y = m_b2Body->GetWorldCenter().y;
}

ConvexShape::~ConvexShape()
{
	printf("~ConvexShape()\n");
	m_world->DestroyBody(m_b2Body);
}

void ConvexShape::update()
{
	if (m_b2BodyDef.type != b2_staticBody)
	{
		setPosition(m_b2Body->GetPosition());
		rotate(Rad2Deg(m_b2Body->GetAngle() / SCALING_FACTOR));
	}
}

void ConvexShape::setPosition(b2Vec2 position)
{
	for (int i{}; i < m_points.size(); ++i)
	{
		m_points[i].x = (position.x + m_b2Shape.m_vertices[i % m_b2Shape.m_count].x) * SCALING_FACTOR;
		m_points[i].y = (position.y + m_b2Shape.m_vertices[i % m_b2Shape.m_count].y) * SCALING_FACTOR;
	}
}

void ConvexShape::rotate(float radians)
{
	for (SDL_FPoint& point : m_points)
	{
		rotatePoint(m_b2Body->GetWorldCenter().x * SCALING_FACTOR,
			m_b2Body->GetWorldCenter().y * SCALING_FACTOR,
			radians, point);
	}
}

void ConvexShape::render(SDL_Renderer* renderer)
{
	int i = color().r;
	int j = color().r;
	int k = color().r;

	SDL_SetRenderDrawColor(renderer, color().r, color().g, color().b, color().a);

	renderLines(renderer);
}

void ConvexShape::renderLines(SDL_Renderer* renderer)
{
	SDL_RenderDrawLinesF(renderer, m_points.data(), m_points.size());

	// draw a line from the last point to the first point
	SDL_RenderDrawLineF(renderer,
		m_points.data()[m_points.size() - 1].x,
		m_points.data()[m_points.size() - 1].y,
		m_points.data()[0].x,
		m_points.data()[0].y);

	// draw the center of mass
	SDL_RenderDrawPointF(renderer,
		m_b2Body->GetWorldCenter().x * SCALING_FACTOR,
		m_b2Body->GetWorldCenter().y * SCALING_FACTOR);
}

void ConvexShape::launch(b2Vec2 direction, float power)
{
	m_b2Body->ApplyForceToCenter(b2Vec2{ direction.x * m_b2Body->GetMass() * power, direction.y * m_b2Body->GetMass() * power }, true);
}
