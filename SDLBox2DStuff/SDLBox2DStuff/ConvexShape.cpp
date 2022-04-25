#include "ConvexShape.h"


ConvexShape::ConvexShape(b2World* world, Vector2f topLeftPosition,
	float width, float height, b2BodyType b2Type, Type type, SDL_Color color, float angle, int id, bool marked) :
	m_world{ world }
{
	m_data.width = width;
	m_data.height = height;
	m_data.position = topLeftPosition;
	m_data.color = color;
	m_data.type = type;
	m_data.angle = angle;
	m_data.id = id;
	m_data.marked = marked;

	m_marked = m_data.marked;
	m_active = true;

	m_center.x = topLeftPosition.x + m_data.width / 2.0f;
	m_center.y = topLeftPosition.y + m_data.height / 2.0f;

	// setup rect points
	m_points.push_back(b2Vec2{ topLeftPosition.x / SCALING_FACTOR, topLeftPosition.y / SCALING_FACTOR });
	m_points.push_back(b2Vec2{ (topLeftPosition.x + m_data.width) / SCALING_FACTOR, topLeftPosition.y / SCALING_FACTOR });
	m_points.push_back(b2Vec2{ (topLeftPosition.x + m_data.width) / SCALING_FACTOR, (topLeftPosition.y + m_data.height) / SCALING_FACTOR });
	m_points.push_back(b2Vec2{ topLeftPosition.x / SCALING_FACTOR, (topLeftPosition.y + m_data.height) / SCALING_FACTOR });

	// setup initial box2d shape
	m_data.b2BodyType = b2Type;
	m_b2BodyDef.type = m_data.b2BodyType;
	m_b2BodyDef.angle = m_data.angle;
	m_b2BodyDef.position.Set((topLeftPosition.x + m_data.width / 2.0f) / SCALING_FACTOR, (topLeftPosition.y + m_data.height / 2.0f) / SCALING_FACTOR);
	m_b2BodyDef.userData.pointer = reinterpret_cast<uintptr_t>(this);
	m_b2Body = m_world->CreateBody(&m_b2BodyDef);
	//setShape();
	m_b2Shape.SetAsBox(m_data.width / 2.0f / SCALING_FACTOR, m_data.height / 2.0f / SCALING_FACTOR);

	if (m_data.type == Type::BULLET || m_data.type == Type::TARGET || m_data.type == Type::BLOCK)
	{
		m_b2Body->SetBullet(true);
	}

	if (m_data.type == Type::PLAYER)
	{
		m_b2Body->SetEnabled(false);
	}

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
	m_world->DestroyBody(m_b2Body);
}

void ConvexShape::renderShadow(SDL_Renderer* renderer, Vector2f position)
{
	SDL_SetRenderDrawColor(renderer, color().r, color().g, color().b, 128);

	std::vector<SDL_FPoint> points{};
	points.push_back(SDL_FPoint{ position.x, position.y });
	points.push_back(SDL_FPoint{ position.x + m_data.width, position.y });
	points.push_back(SDL_FPoint{ position.x + m_data.width, position.y + m_data.height });
	points.push_back(SDL_FPoint{ position.x, position.y + m_data.height });

	renderLines(renderer, &points);
}

void ConvexShape::renderLines(SDL_Renderer* renderer, std::vector<SDL_FPoint>* points)
{
	SDL_RenderDrawLinesF(renderer, points->data(), points->size());

	// draw a line from the last point to the first point
	SDL_RenderDrawLineF(renderer,
		points->data()[points->size() - 1].x,
		points->data()[points->size() - 1].y,
		points->data()[0].x,
		points->data()[0].y);

	// draw the center of mass
	SDL_RenderDrawPointF(renderer,
		m_b2Body->GetWorldCenter().x * SCALING_FACTOR,
		m_b2Body->GetWorldCenter().y * SCALING_FACTOR);
}

void ConvexShape::launch(b2Vec2 direction, float power)
{
	m_b2Body->ApplyForceToCenter(b2Vec2{ direction.x * m_b2Body->GetMass() * power, direction.y * m_b2Body->GetMass() * power }, true);
}