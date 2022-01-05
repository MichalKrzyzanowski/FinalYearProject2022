#include "../include/Game.h"

Game::Game() :
	m_gameIsRunning{ false },
	m_groundConvexShape{ &m_world, Vector2f{0, SCREEN_HEIGHT - 70.0f}, SCREEN_WIDTH, 100, b2_staticBody },
	m_rectanglePrefab{ &m_world, Vector2f{0, 0}, 20, 50, b2_dynamicBody },
	m_squarePrefab{ &m_world, Vector2f{0, 0}, 20, 20, b2_dynamicBody },
	m_targetPrefab{ &m_world, Vector2f{0, 0}, },
	m_playerPrefab{ &m_world, Vector2f{0, 0}, },
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
	m_selectedButton = &m_rectButton;
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

			if (!m_playSim)
			{
				reset();
			}
		}

		if (!m_playSim)
		{
			if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_g)
			{
				if (m_targetPresent && m_player)
				{
					estimateDifficulty();
				}
			}
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
			if (m_currentShape->type() == Type::PLAYER && !m_playerPresent)
			{
				m_shapeSpawner.emplace_back(&m_world,
					Vector2f{ static_cast<float>(x), static_cast<float>(y) },
					m_currentShape->width(),
					m_currentShape->height(),
					m_currentShape->b2Body(),
					m_currentShape->color(),
					m_currentShape->type());

				storeShapeData(&m_shapeSpawner.back());

				m_player = &m_shapeSpawner.back();
				m_playerPresent = true;
			}
			else if (m_currentShape->type() != Type::PLAYER)
			{
				m_shapeSpawner.emplace_back(&m_world,
					Vector2f{ static_cast<float>(x), static_cast<float>(y) },
					m_currentShape->width(),
					m_currentShape->height(),
					m_currentShape->b2Body(),
					m_currentShape->color(),
					m_currentShape->type());

				storeShapeData(&m_shapeSpawner.back());

				if (m_currentShape->type() == Type::TARGET)
				{
					m_targetPresent = true;
				}
			}
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
				m_selectedButton = &m_squareButton;
			}
		}
		else if ((x >= m_rectButton.position().x && x <= m_rectButton.position().x + m_rectButton.width()) &&
			(y >= m_rectButton.position().y && y <= m_rectButton.position().y + m_rectButton.height()))
		{
			if (e.type == SDL_MOUSEBUTTONDOWN && ((buttons & SDL_BUTTON_LMASK) != 0))
			{
				m_currentShape = &m_rectanglePrefab;
				m_selectedButton = &m_rectButton;
			}
		}
		else if ((x >= m_targetButton.position().x && x <= m_targetButton.position().x + m_targetButton.width()) &&
			(y >= m_targetButton.position().y && y <= m_targetButton.position().y + m_targetButton.height()))
		{
			if (e.type == SDL_MOUSEBUTTONDOWN && ((buttons & SDL_BUTTON_LMASK) != 0))
			{
				m_currentShape = &m_targetPrefab;
				m_selectedButton = &m_targetButton;
			}
		}
		else if ((x >= m_playerButton.position().x && x <= m_playerButton.position().x + m_playerButton.width()) &&
			(y >= m_playerButton.position().y && y <= m_playerButton.position().y + m_playerButton.height()))
		{
			if (e.type == SDL_MOUSEBUTTONDOWN && ((buttons & SDL_BUTTON_LMASK) != 0))
			{
				m_currentShape = &m_playerPrefab;
				m_selectedButton = &m_playerButton;
			}
		}
	}

	if (m_playSim && m_player)
	{
		if (e.type == SDL_MOUSEBUTTONDOWN && ((buttons & SDL_BUTTON_LMASK) != 0))
		{
			shoot(Vector2f{ static_cast<float>(x), static_cast<float>(y) });
		}
	}
}

void Game::update()
{
	if (m_playSim)
	{
		m_world.Step(m_timeStep, m_velocityIterations, m_positionIterations);

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

	for (ConvexShape& shape : m_shapeSpawner)
	{
		shape.render(m_renderer);
	}
	m_groundConvexShape.render(m_renderer);
	//m_circle.render(m_renderer);

	if (!m_playSim)
	{
		if (y < m_toolbarBg.y && m_currentShape)
		{
			m_currentShape->renderShadow(m_renderer, Vector2f{ static_cast<float>(x), static_cast<float>(y) });
		}

		SDL_SetRenderDrawColor(m_renderer, 128, 128, 128, 128);
		SDL_RenderFillRectF(m_renderer, &m_toolbarBg);

		SDL_SetRenderDrawColor(m_renderer, 0x00, 0x00, 0x00, 0xFF);
		m_rectButton.render(m_renderer);
		m_squareButton.render(m_renderer);
		m_targetButton.render(m_renderer);
		m_playerButton.render(m_renderer);

		SDL_RenderFillRectF(m_renderer, &m_squareShapeSelect);
		SDL_RenderFillRectF(m_renderer, &m_rectangleShapeSelect);

		if (m_selectedButton)
		{
			SDL_SetRenderDrawColor(m_renderer, 0xD2, 0x21, 0x21, 0xFF);
			m_selectedButton->render(m_renderer);
		}

		SDL_SetRenderDrawColor(m_renderer, 240, 207, 46, 255);
		SDL_RenderFillRectF(m_renderer, &m_targetSelect);
		SDL_SetRenderDrawColor(m_renderer, 0x24, 0x3C, 0xAE, 0xFF);
		SDL_RenderFillRectF(m_renderer, &m_playerSelect);
	}

	SDL_RenderPresent(m_renderer);
}

void Game::storeShapeData(ConvexShape* shape)
{
	m_shapeData.push_back(ShapeData
		{
			shape->color(),
			shape->staticPosition(),
			shape->type(),
			shape->b2Body(),
			shape->width(),
			shape->height()
		});
}

void Game::reset()
{
	m_shapeSpawner.clear();
	m_playerPresent = false;
	m_targetPresent = false;
	m_player = nullptr;

	for (ShapeData& data : m_shapeData)
	{
		m_shapeSpawner.emplace_back(&m_world,
			Vector2f{ data.position },
			data.width,
			data.height,
			data.b2BodyType,
			data.color,
			data.type);

		if (data.type == Type::PLAYER)
		{
			m_player = &m_shapeSpawner.back();
			m_playerPresent = true;
		}

		else if (data.type == Type::TARGET)
		{
			m_targetPresent = true;
		}
	}
}

void Game::estimateDifficulty()
{
	int rerunAmount{ 3 };
	// how many times ro rerun the sim
	for (int i{}; i < rerunAmount; ++i)
	{
		reset();
		m_playSim = true;

		int targetNumber{};
		for (ConvexShape& target : m_shapeSpawner)
		{
			// if the current shape is a target
			if (target.type() == Type::TARGET)
			{
				++targetNumber;
				// do stuff
				while (target.awake())
				{
					update();
					render();
				}
				printf("Target: %d is sleeping, preparing to shoot\n", targetNumber);
				
				shoot(Vector2f{ target.position().x * SCALING_FACTOR, target.position().y * SCALING_FACTOR });

				while (m_currentBullet->awake())
				{
					update();
					render();
				}
				printf("Bullet has come to rest\n");
			}
		}
	}

	reset();
	m_playSim = false;
}

void Game::cleanUp()
{
	SDL_DestroyRenderer(m_renderer);
	SDL_DestroyWindow(m_window);
	m_window = nullptr;
	m_renderer = nullptr;

	SDL_Quit();
}

void Game::shoot(Vector2f targetPosition)
{
	b2Vec2 unit{ targetPosition.x - m_player->position().x * SCALING_FACTOR, targetPosition.y - m_player->position().y * SCALING_FACTOR };
	unit.Normalize();
	m_shapeSpawner.emplace_back(&m_world, Vector2f{ (m_player->position().x + unit.x / 4) * SCALING_FACTOR,
		(m_player->position().y + unit.y / 4) * SCALING_FACTOR }, 15, 15, b2_dynamicBody);

	m_currentBullet = &m_shapeSpawner.back();
	m_shapeSpawner.back().launch(unit, 500.0f);
}
