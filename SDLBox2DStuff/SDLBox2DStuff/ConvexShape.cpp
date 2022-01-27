#include "ConvexShape.h"


ConvexShape::ConvexShape(b2World* world, Vector2f topLeftPosition, float width, float height, b2BodyType b2Type, Type type, SDL_Color color) :
	m_world{ world }
{
	m_data.width = width;
	m_data.height = height;
	m_data.position = topLeftPosition;
	m_data.height = height;
	m_data.color = color;
	m_data.type = type;

	m_marked = false;

	m_center.x = topLeftPosition.x + m_data.width / 2.0f;
	m_center.y = topLeftPosition.y + m_data.height / 2.0f;

	// setup rect points
	m_points.push_back(b2Vec2{ topLeftPosition.x / SCALING_FACTOR, topLeftPosition.y / SCALING_FACTOR });
	m_points.push_back(b2Vec2{ (topLeftPosition.x + m_data.width) / SCALING_FACTOR, topLeftPosition.y / SCALING_FACTOR });
	m_points.push_back(b2Vec2{ (topLeftPosition.x + m_data.width) / SCALING_FACTOR, (topLeftPosition.y + m_data.height) / SCALING_FACTOR });
	m_points.push_back(b2Vec2{ topLeftPosition.x / SCALING_FACTOR, (topLeftPosition.y + m_data.height) / SCALING_FACTOR });

	// setup initial box2d shape
	m_data.b2BodyType = b2Type;
	m_b2BodyDef.type = b2Type;
	m_b2BodyDef.position.Set((topLeftPosition.x + m_data.width / 2.0f) / SCALING_FACTOR, (topLeftPosition.y + m_data.height / 2.0f) / SCALING_FACTOR);
	m_b2BodyDef.userData.pointer = reinterpret_cast<uintptr_t>(this);
	m_b2Body = m_world->CreateBody(&m_b2BodyDef);
	setShape();
	//m_b2Shape.SetAsBox(m_data.width / 2.0f / SCALING_FACTOR, m_data.height / 2.0f / SCALING_FACTOR);


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
	/*for (SDL_FPoint& point : m_points)
	{
		rotatePoint(m_b2Body->GetWorldCenter().x * SCALING_FACTOR,
			m_b2Body->GetWorldCenter().y * SCALING_FACTOR,
			radians, point);
	}*/
}

void ConvexShape::render(SDL_Renderer* renderer)
{
	SDL_SetRenderDrawColor(renderer, color().r, color().g, color().b, color().a);

	renderLines(renderer);
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

void ConvexShape::renderLines(SDL_Renderer* renderer)
{
	//SDL_RenderDrawLinesF(renderer, m_points.data(), m_points.size());

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

	SDL_RenderDrawPointF(renderer,
		m_b2Body->GetWorldCenter().x * SCALING_FACTOR,
		m_b2Body->GetWorldCenter().y * SCALING_FACTOR);
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

void ConvexShape::setShape()
{
	/*std::vector<b2Vec2> points;

	for (SDL_FPoint point : m_points)
	{
		points.push_back(b2Vec2{ point.x / SCALING_FACTOR, point.y / SCALING_FACTOR });
	}*/

	m_b2Shape.Set(m_points.data(), m_points.size());
}
