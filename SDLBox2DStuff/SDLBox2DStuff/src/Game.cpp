#include "../include/Game.h"

Game::Game() :
	m_gameIsRunning{ false },
	m_groundConvexShape{ &m_world, Vector2f{0, SCREEN_HEIGHT}, SCREEN_WIDTH, 100, b2_staticBody },
	m_rectanglePrefab{ &m_world, Vector2f{0, 0}, 20, 50, b2_staticBody },
	m_squarePrefab{ &m_world, Vector2f{0, 0}, 20, 20, b2_staticBody },
	m_circle{ Vector2f{100, 100}, 50 }
{
	SDL_Init(SDL_INIT_VIDEO);

	m_window = SDL_CreateWindow("SDL game loop",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		SCREEN_WIDTH, SCREEN_HEIGHT,
		SDL_WINDOW_SHOWN);
	m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);

	m_shapeSpawner.reserve(10000); // reserve some space for ALOT of squares
	m_currentShape = &m_rectanglePrefab;
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


	if (!m_playSim && y < m_toolbarBg.y)
	{
		// if the left mouse button is pressed down
		if (e.type == SDL_MOUSEBUTTONDOWN && ((buttons & SDL_BUTTON_LMASK) != 0))
		{
			m_shapeSpawner.emplace_back(&m_world, Vector2f{ static_cast<float>(x), static_cast<float>(y) }, m_currentShape->width(), m_currentShape->height(), b2_dynamicBody);
			//m_sprayTime = true;
			printf("x:%d y:%d\n", x, y);
		}
	}
	else if (!m_playSim && y > m_toolbarBg.y)
	{
		if ((x >= m_squareButton.position().x && x <= m_squareButton.position().x + m_squareButton.width()) &&
			(y >= m_squareButton.position().y && y <= m_squareButton.position().y + m_squareButton.height()))
		{
			if (e.type == SDL_MOUSEBUTTONDOWN && ((buttons & SDL_BUTTON_LMASK) != 0))
			{
				m_currentShape = &m_squarePrefab;
			}
		}
		else if ((x >= m_rectButton.position().x && x <= m_rectButton.position().x + m_rectButton.width()) &&
			(y >= m_rectButton.position().y && y <= m_rectButton.position().y + m_rectButton.height()))
		{
			if (e.type == SDL_MOUSEBUTTONDOWN && ((buttons & SDL_BUTTON_LMASK) != 0))
			{
				m_currentShape = &m_rectanglePrefab;
			}
		}
	}
	/*else if(e.type == SDL_MOUSEBUTTONUP)
	{
		m_sprayTime = false;
	}*/
}

void Game::update()
{
	if (m_playSim)
	{
		m_world.Step(m_timeStep, m_velocityIterations, m_positionIterations);

		//if (m_sprayTime && m_timer.getTicksAsSeconds() > m_sprayCooldown)
		//{
		//	/*m_shapeSpawner.emplace_back(&m_world, Vector2f{ static_cast<float>(x), static_cast<float>(y) }, 20, 20, b2_dynamicBody);*/
		//	m_timer.restart();
		//}

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
	//m_circle.render(m_renderer);

	if (!m_playSim)
	{
		SDL_SetRenderDrawColor(m_renderer, 128, 128, 128, 128);
		SDL_RenderFillRectF(m_renderer, &m_toolbarBg);

		SDL_SetRenderDrawColor(m_renderer, 0x00, 0x00, 0x00, 0xFF);
		m_rectButton.render(m_renderer);
		m_squareButton.render(m_renderer);

		SDL_RenderFillRectF(m_renderer, &m_squareShapeSelect);
		SDL_RenderFillRectF(m_renderer, &m_rectangleShapeSelect);
	}

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