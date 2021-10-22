#include "../include/Game.h"

Game::Game() :
	m_gameIsRunning{ false },
	m_rect{ Vector2f{10.0f, 50.0f} , 50.0f, 50.0f }
{
	m_groundRect.w = 300.0f;
	m_groundRect.h = 100.0f;
	m_groundRect.x = 10.0f;
	m_groundRect.y = 200.0f;

	m_boxRect.w = 10.0f;
	m_boxRect.h = 10.0f;
	m_boxRect.x = 10.0f;
	m_boxRect.y = 10.0f;

	// box2d setup
	m_groundBodDef.position.Set(m_groundRect.x / m_SCALING_FACTOR, m_groundRect.y / m_SCALING_FACTOR);
	m_groundBody = m_world.CreateBody(&m_groundBodDef);
	m_groundBox.SetAsBox(480.0f / 2.0f / m_SCALING_FACTOR, m_groundRect.h / 2.0f / m_SCALING_FACTOR);
	m_groundBody->CreateFixture(&m_groundBox, 0.0f);

	m_DynamicBodDef.type = b2_dynamicBody;
	m_DynamicBodDef.position.Set(m_rect.position().x / m_SCALING_FACTOR, m_rect.position().y / m_SCALING_FACTOR);
	m_dynamicBoxBody = m_world.CreateBody(&m_DynamicBodDef);
	m_dynamicBox.SetAsBox(m_rect.width() / 2.0f / m_SCALING_FACTOR, m_rect.height() / 2.0f / m_SCALING_FACTOR);

	m_boxFix.shape = &m_dynamicBox;
	m_boxFix.density = 1.0f;
	m_boxFix.friction = 0.3f;
	m_dynamicBoxBody->CreateFixture(&m_boxFix);

	SDL_Init(SDL_INIT_VIDEO);

	m_window = SDL_CreateWindow("SDL game loop", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	SDL_SetRenderDrawColor(m_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
}

Game::~Game()
{
	cleanUp();
}

void Game::run()
{
	m_gameIsRunning = true;
	SDL_Event e{};

	while (m_gameIsRunning)
	{
		processEvents(e);
		update();
		render();
	}

	cleanUp();
}

void Game::rotatePoint(float cx, float cy, float angle, SDL_FPoint& point)
{
	point.x -= cx;
	point.y -= cy;

	float newX = point.x * cosf(angle) - point.y * sinf(angle);
	float newY = point.x * sinf(angle) + point.y * cosf(angle);

	point.x = newX + cx;
	point.y = newY + cy;
}

void Game::processEvents(SDL_Event e)
{
	// simple event loop
	while (SDL_PollEvent(&e) != 0)
	{
		if (e.type == SDL_QUIT)
		{
			m_gameIsRunning = false;
		}

		// checks if the escape key is pressed down
		if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)
		{
			m_gameIsRunning = false;
		}
	}

}

void Game::update()
{
	m_world.Step(m_timeStep, m_velocityIterations, m_positionIterations);
	b2Vec2 position = m_dynamicBoxBody->GetPosition();
	float angle = m_dynamicBoxBody->GetAngularVelocity();

	m_rect.setPosition(position.x * m_SCALING_FACTOR + m_rect.width() / 2.0, position.y * m_SCALING_FACTOR + m_rect.height() / 2.0);
	m_rect.rotate(Deg2Rad(angle));
	//m_boxRect.x = position.x * m_SCALING_FACTOR + m_boxRect.w / 2.0;
	//m_boxRect.y = position.y * m_SCALING_FACTOR + m_boxRect.h / 2.0;

	//m_rect.update();

	printf("%4.2f %4.2f %4.2f\n", position.x, position.y, angle);
}

void Game::render()
{
	SDL_SetRenderDrawColor(m_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(m_renderer);

	SDL_SetRenderDrawColor(m_renderer, 0x00, 0x00, 0x00, 0xFF);
	SDL_RenderDrawRectF(m_renderer, &m_groundRect);
	//SDL_RenderDrawRectF(m_renderer, &m_boxRect);

	m_rect.render(m_renderer);


	SDL_RenderPresent(m_renderer);
}

void Game::cleanUp()
{
	SDL_DestroyRenderer(m_renderer);
	SDL_DestroyWindow(m_window);
	m_window = nullptr;
	m_renderer = nullptr;

	SDL_Quit();
}