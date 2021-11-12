#include "../include/Game.h"

Game::Game() :
	m_gameIsRunning{ false },
	m_groundConvexShape{ &m_world, Vector2f{0, SCREEN_HEIGHT}, SCREEN_WIDTH, 100, b2_staticBody },
	m_circle{ Vector2f{100, 100}, 50 }
{
	SDL_Init(SDL_INIT_VIDEO);

	m_window = SDL_CreateWindow("SDL game loop",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		SCREEN_WIDTH, SCREEN_HEIGHT,
		SDL_WINDOW_SHOWN);
	m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	m_shapeSpawner.reserve(10000); // reserve some space for ALOT of squares
}

Game::~Game()
{
	printf("~Game()\n");
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

		if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_SPACE)
		{
			m_playSim = !m_playSim;
		}

		processMouseEvents(e);
	}

}

void Game::processMouseEvents(SDL_Event e)
{
	Uint32 buttons = SDL_GetMouseState(&x, &y);

	// if the left mouse button is pressed down
	if (e.type == SDL_MOUSEBUTTONDOWN && ((buttons & SDL_BUTTON_LMASK) != 0))
	{
		m_shapeSpawner.emplace_back(&m_world, Vector2f{ static_cast<float>(x), static_cast<float>(y) }, 20, 20, b2_dynamicBody);
		m_sprayTime = true;
		printf("x:%d y:%d\n", x, y);
	}
	else if(e.type == SDL_MOUSEBUTTONUP)
	{
		m_sprayTime = false;
	}
}

void Game::update()
{
	if (m_playSim)
	{
		m_world.Step(m_timeStep, m_velocityIterations, m_positionIterations);

		if (m_sprayTime && m_timer.getTicksAsSeconds() > m_sprayCooldown)
		{
			/*m_shapeSpawner.emplace_back(&m_world, Vector2f{ static_cast<float>(x), static_cast<float>(y) }, 20, 20, b2_dynamicBody);*/
			m_timer.restart();
		}

		for (ConvexShape& shape : m_shapeSpawner)
		{
			shape.update();
		}
	}
}

void Game::render()
{
	SDL_SetRenderDrawColor(m_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(m_renderer);

	//m_dynamicConvexShape.render(m_renderer);
	for (ConvexShape& shape : m_shapeSpawner)
	{
		shape.render(m_renderer);
	}
	m_groundConvexShape.render(m_renderer);
	m_circle.render(m_renderer);

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