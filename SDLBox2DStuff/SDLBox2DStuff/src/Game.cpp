#include "../include/Game.h"

Game::Game() :
	m_gameIsRunning{ false },
	m_groundConvexShape{ &m_world, Vector2f{0, SCREEN_HEIGHT - 70.0f}, SCREEN_WIDTH, 100, b2_staticBody },
	m_leftWallConvexShape{ &m_world, Vector2f{SCREEN_WIDTH, 0}, 10, SCREEN_HEIGHT, b2_staticBody },
	m_rightWallConvexShape{ &m_world, Vector2f{-10, 0}, 10, SCREEN_HEIGHT, b2_staticBody },
	m_roofConvexShape{ &m_world, Vector2f{0, -10}, SCREEN_WIDTH, 10, b2_staticBody },
	m_rectanglePrefab{ &m_world, Vector2f{-100, 0}, 20, 50, b2_dynamicBody },
	m_squarePrefab{ &m_world, Vector2f{-100, 0}, 20, 20, b2_dynamicBody },
	m_targetPrefab{ &m_world, Vector2f{-100, 0}, 20, 20, b2_dynamicBody, Type::TARGET, SDL_Color{ 240, 207, 46, 255 } },
	m_playerPrefab{ &m_world, Vector2f{-100, 0}, 20, 20, b2_staticBody, Type::PLAYER, SDL_Color{ 0x24, 0x3C, 0xAE, 0xFF } },
	m_circle{ Vector2f{100, 100}, 50 }
{
	SDL_Init(SDL_INIT_VIDEO);

	int imgFlags = IMG_INIT_PNG;

	IMG_Init((imgFlags)&imgFlags);

	TTF_Init();

	m_window = SDL_CreateWindow("SDL game loop",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		SCREEN_WIDTH, SCREEN_HEIGHT,
		SDL_WINDOW_SHOWN);

	m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);

	m_fontNormal = TTF_OpenFont("assets/LemonMilk.otf", 18);
	m_fontSmall = TTF_OpenFont("assets/LemonMilk.otf", 14);
	m_fontTiny = TTF_OpenFont("assets/LemonMilk.otf", 8);

	m_shapeSpawner.reserve(10000); // reserve some space for ALOT of squares
	m_currentShape = &m_rectanglePrefab;
	m_selectedButton = &m_rectButton;
	m_world.SetContactListener(&m_contactListener);

	m_debugDraw.setRenderer(m_renderer);
	m_world.SetDebugDraw(&m_debugDraw);
	m_debugDraw.SetFlags(b2Draw::e_shapeBit);

	printf("Edit Phase\n");

	std::string bulletStr = "Bullets Left: " + std::to_string(m_bulletsCount);

	m_phaseText = loadFromRenderedText("Edit Phase", SDL_Color{ 0, 0, 0, 255 }, m_fontNormal, m_renderer);
	m_bulletCountText = loadFromRenderedText(bulletStr.c_str(), SDL_Color{ 0, 0, 0, 255 }, m_fontNormal, m_renderer);

	std::string powerStr = "Power: " + std::to_string((int)(m_power / 8.0f)) + '%';

	m_powerText = loadFromRenderedText(powerStr.c_str(), SDL_Color{ 0, 0, 0, 255 }, m_fontNormal, m_renderer);
	m_warningText = loadFromRenderedText("Level needs a player and a target!", SDL_Color{ 0, 0, 0, 255 }, m_fontNormal, m_renderer);


	m_squareText = loadFromRenderedText("Square Block", SDL_Color{ 0, 0, 0, 255 }, m_fontTiny, m_renderer);
	m_rectangleText = loadFromRenderedText("Rectangle Block", SDL_Color{ 0, 0, 0, 255 }, m_fontTiny, m_renderer);
	m_targetText = loadFromRenderedText("Target Block", SDL_Color{ 0, 0, 0, 255 }, m_fontTiny, m_renderer);
	m_playerText = loadFromRenderedText("Player Block", SDL_Color{ 0, 0, 0, 255 }, m_fontTiny, m_renderer);

	m_helpText.texture = SDL_CreateTextureFromSurface(m_renderer,
		TTF_RenderText_Blended_Wrapped(m_fontNormal, "H:    Toggle Help\nG:    Run Difficulty Estimation\nS:    Save Level\nL:    Load Level\nSpacebar:    Toggle Edit/Play Mode",
			SDL_Color{ 0, 0, 0, 255 }, 400));
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
		if (e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_ESCAPE)
		{
			m_gameIsRunning = false;
		}

		if (e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_SPACE && !m_estimationMode)
		{
			m_playSim = !m_playSim;

			if (!m_playSim)
			{
				m_gameState = GameState::EDIT;
				reset();
				m_shootMode = false;
				printf("Edit Phase\n");
				m_phaseText = loadFromRenderedText("Edit Phase", SDL_Color{ 0, 0, 0, 255 }, m_fontNormal, m_renderer);
			}
			else
			{
				if (m_targetPresent && m_playerPresent)
				{
					m_gameState = GameState::GAMEPLAY;
					printf("Shoot Phase\n");
					m_phaseText = loadFromRenderedText("Simulate Phase", SDL_Color{ 0, 0, 0, 255 }, m_fontNormal, m_renderer);
				}
				else
				{
					m_playSim = !m_playSim;
					m_showWarning = true;
					m_warningTimer.restart();
				}
			}
		}

		if (m_gameState == GameState::EDIT)
		{
			if (!m_estimationMode)
			{
				if (e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_h)
				{
					m_showHelp = !m_showHelp;
				}

				if (e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_s)
				{
					saveLevelData("level");

					m_levelSaveText = loadFromRenderedText("Level Saved!", SDL_Color{ 0, 0, 0, 255 }, m_fontNormal, m_renderer);
					m_showLevelSave = true;
					m_levelSaveTimer.restart();
				}

				if (e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_l)
				{
					loadLevelData("level");

					m_levelSaveText = loadFromRenderedText("Level Loaded!", SDL_Color{ 0, 0, 0, 255 }, m_fontNormal, m_renderer);
					m_showLevelSave = true;
					m_levelSaveTimer.restart();
				}
			}

			if (e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_g)
			{
				if (m_targetPresent && m_player)
				{
					printf("Difficulty Simulation Phase\n");
					m_phaseText = loadFromRenderedText("Difficulty Simulation Phase", SDL_Color{ 0, 0, 0, 255 }, m_fontNormal, m_renderer);

					m_estimationMode = true;
					SDL_EventState(SDL_KEYUP, SDL_IGNORE);
					estimateDifficulty();
					SDL_EventState(SDL_KEYUP, SDL_ENABLE);
					m_estimationMode = false;
				}
				else
				{
					m_playSim = !m_playSim;
					m_showWarning = true;
					m_warningTimer.restart();
				}
			}
		}

		processMouseEvents(e);
	}

}

void Game::processMouseEvents(SDL_Event e)
{
	Uint32 buttons = SDL_GetMouseState(&x, &y);


	if (m_gameState == GameState::EDIT && y < m_toolbarBg.y)
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
					m_currentShape->b2BodyDefType(),
					m_currentShape->type(),
					m_currentShape->color());

				storeShapeData(m_shapeSpawner.back().data());

				m_player = &m_shapeSpawner.back();
				m_playerPresent = true;

			}
			else if (m_currentShape->type() != Type::PLAYER)
			{
				m_shapeSpawner.emplace_back(&m_world,
					Vector2f{ static_cast<float>(x), static_cast<float>(y) },
					m_currentShape->width(),
					m_currentShape->height(),
					m_currentShape->b2BodyDefType(),
					m_currentShape->type(),
					m_currentShape->color());


				storeShapeData(m_shapeSpawner.back().data());


				if (m_currentShape->type() == Type::TARGET)
				{
					m_targetPresent = true;
				}
			}
		}
	}
	else if (m_gameState == GameState::EDIT && y > m_toolbarBg.y)
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

	if (m_gameState == GameState::GAMEPLAY && m_player && m_shootMode && m_bulletsCount > 0)
	{
		if (e.type == SDL_MOUSEBUTTONDOWN && ((buttons & SDL_BUTTON_LMASK) != 0))
		{
			shoot(Vector2f{ static_cast<float>(x), static_cast<float>(y) });
			m_shootMode = false;
			--m_bulletsCount;

			std::string bulletStr = "Bullets Left: " + std::to_string(m_bulletsCount);

			m_phaseText = loadFromRenderedText("Simulate Phase", SDL_Color{ 0, 0, 0, 255 }, m_fontNormal, m_renderer);
			m_bulletCountText = loadFromRenderedText(bulletStr.c_str(), SDL_Color{ 0, 0, 0, 255 }, m_fontNormal, m_renderer);
			printf("Simulate Phase\n");
		}
	}
}

void Game::update()
{
	if (m_gameState == GameState::GAMEPLAY)
	{
		m_showWarning = false;
		m_showLevelSave = false;

		m_world.Step(m_timeStep, m_velocityIterations, m_positionIterations);

		if (!m_world.IsLocked())
		{
			for (ConvexShape& shape : m_shapeSpawner)
			{
				if (!shape.active() && shape.b2Body()->IsEnabled())
				{
					shape.b2Body()->SetEnabled(false);
					shape.b2Body()->SetAwake(false);
					shape.b2Body()->SetTransform(b2Vec2{ -1000.0f, -1000.0f }, 0.0f);
					printf("stopped");
				}
			}
		}

		bool readyToShoot{ true };

		if (!m_shootMode)
		{
			for (ConvexShape& shape : m_shapeSpawner)
			{
				//shape.update();
				if (shape.awake())
				{
					readyToShoot = false;
					break;
				}
			}

			bool gameWin{ true };

			for (ConvexShape& target : m_shapeSpawner)
			{
				//shape.update();
				if (target.type() == Type::TARGET)
				{
					if (!target.marked())
					{
						gameWin = false;
						break;
					}
				}
			}

			if (readyToShoot)
			{
				m_shootMode = true;

				if (!m_estimationMode)
				{
					if (m_bulletsCount <= 0 && !gameWin)
					{
						m_gameState = GameState::LOSE;
						m_phaseText = loadFromRenderedText("You Lose!", SDL_Color{ 0, 0, 0, 255 }, m_fontNormal, m_renderer);
						return;
					}
					else if (gameWin)
					{
						m_gameState = GameState::WIN;
						m_phaseText = loadFromRenderedText("You Win!", SDL_Color{ 0, 0, 0, 255 }, m_fontNormal, m_renderer);
						return;
					}

					printf("Shoot Phase\n");
					m_phaseText = loadFromRenderedText("Shoot Phase", SDL_Color{ 0, 0, 0, 255 }, m_fontNormal, m_renderer);
				}

			}
		}
		else
		{
			if (m_player && !m_estimationMode)
			{
				if (state[SDL_SCANCODE_LEFT])
				{
					if (state[SDL_SCANCODE_LSHIFT])
					{
						m_power -= m_powerGain;
					}

					m_power -= m_powerGain;

					if (m_power <= m_MIN_POWER)
					{
						m_power = m_MIN_POWER;
					}

					printf("Power: %f\n\n", m_power);

					std::string powerStr = "Power: " + std::to_string((int)(m_power / 8.0f)) + '%';

					m_powerText = loadFromRenderedText(powerStr.c_str(), SDL_Color{ 0, 0, 0, 255 }, m_fontNormal, m_renderer);
				}

				if (state[SDL_SCANCODE_RIGHT])
				{
					if (state[SDL_SCANCODE_LSHIFT])
					{
						m_power += m_powerGain;
					}

					m_power += m_powerGain;

					if (m_power >= m_MAX_POWER)
					{
						m_power = m_MAX_POWER;
					}

					printf("Power: %f\n\n", m_power);

					std::string powerStr = "Power: " + std::to_string((int)(m_power / 8.0f)) + '%';

					m_powerText = loadFromRenderedText(powerStr.c_str(), SDL_Color{ 0, 0, 0, 255 }, m_fontNormal, m_renderer);
				}
			}
		}
	}

	else if (m_gameState == GameState::EDIT)
	{
		if (m_warningTimer.getTicksAsSeconds() > 2.0f)
		{
			m_showWarning = false;
		}

		if (m_levelSaveTimer.getTicksAsSeconds() > 2.0f)
		{
			m_showLevelSave = false;
		}
	}
}

void Game::render()
{
	//SDL_SetRenderDrawColor(m_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	//SDL_RenderClear(m_renderer);

	//for (ConvexShape& shape : m_shapeSpawner)
	//{
	//	shape.render(m_renderer);
	//}
	//m_groundConvexShape.render(m_renderer);
	////m_circle.render(m_renderer);


	SDL_SetRenderDrawColor(m_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(m_renderer);

	m_world.DebugDraw();

	if (m_gameState == GameState::EDIT)
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

		if (m_showHelp)
		{
			SDL_FRect helpRect{ SCREEN_WIDTH - 250, SCREEN_HEIGHT - m_toolbarBg.h - 110, 250, 100 };
			SDL_RenderCopyF(m_renderer, m_helpText.texture, NULL, &helpRect);
		}

		renderText(m_renderer, &m_squareText, Vector2f{ m_squareButton.position().x - m_squareButton.width() / 4,
										m_squareButton.position().y + 45.0f, });

		renderText(m_renderer, &m_rectangleText, Vector2f{ m_rectButton.position().x - m_rectButton.width() / 2,
											m_rectButton.position().y + 45.0f, });

		renderText(m_renderer, &m_targetText, Vector2f{ m_targetButton.position().x - m_targetButton.width() / 4,
											m_targetButton.position().y + 45.0f, });

		renderText(m_renderer, &m_playerText, Vector2f{ m_playerButton.position().x - m_playerButton.width() / 4,
											m_playerButton.position().y + 45.0f, });

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
	else
	{
		if (m_player)
		{
			SDL_RenderDrawLine(m_renderer, m_player->position().x * SCALING_FACTOR, m_player->position().y * SCALING_FACTOR, static_cast<float>(x), static_cast<float>(y));
		}

		SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255);
		renderText(m_renderer, &m_powerText, Vector2f{ 10, SCREEN_HEIGHT - static_cast<float>(m_phaseText.height) - 10 });
	}

	SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255);
	renderText(m_renderer, &m_phaseText, Vector2f{ SCREEN_WIDTH - static_cast<float>(m_phaseText.width) - 10, 10 });
	renderText(m_renderer, &m_bulletCountText, Vector2f{ SCREEN_WIDTH - static_cast<float>(m_bulletCountText.width) - 10, 40 });

	if (m_showWarning)
	{
		renderText(m_renderer, &m_warningText, Vector2f{ 10, 10 });
	}

	if (m_showLevelSave)
	{
		renderText(m_renderer, &m_levelSaveText, Vector2f{ 10, 30 });
	}

	SDL_RenderPresent(m_renderer);
}

void Game::saveLevelData(const std::string& fileName)
{
	printf("Saving level data to file\n");

	std::ofstream levelData(fileName + ".txt");

	levelData << m_shapeData.size() << "\n";

	for (ShapeData& data : m_shapeData)
	{
		levelData << static_cast<int>(data.color.r)
			<< "," << static_cast<int>(data.color.g)
			<< "," << static_cast<int>(data.color.b)
			<< "," << static_cast<int>(data.color.a) << "\n";

		levelData << data.position.x << "," << data.position.y << "\n";
		levelData << data.height << "\n";
		levelData << data.width << "\n";
		levelData << static_cast<int>(data.type) << "\n";
		levelData << static_cast<int>(data.b2BodyType) << "\n";
	}

	levelData.close();
}

void Game::loadLevelData(const std::string& fileName)
{
	printf("Loading level data from file\n");

	m_shapeData.clear();

	ShapeData tempData{};
	int dataLength{ 6 };
	int currentLine{ 1 };

	double shapeAmt{};
	std::string temp{};
	std::ifstream levelData(fileName + ".txt");

	std::getline(levelData, temp);

	shapeAmt = std::atof(temp.c_str());

	for (int i{}; i < shapeAmt; ++i)
	{
		// get colour data
		std::getline(levelData, temp);

		std::vector<std::string> stringSplit{ splitString(temp, ',') };
		tempData.color.r = std::stoi(stringSplit[0]);
		tempData.color.g = std::stoi(stringSplit[1]);
		tempData.color.b = std::stoi(stringSplit[2]);
		tempData.color.a = std::stoi(stringSplit[3]);

		// get position data
		std::getline(levelData, temp);

		stringSplit.clear();
		stringSplit = splitString(temp, ',');
		tempData.position.x = std::atof(stringSplit[0].c_str());
		tempData.position.y = std::atof(stringSplit[1].c_str());

		// get height data
		std::getline(levelData, temp);

		tempData.height = std::stoi(temp);

		// get width data
		std::getline(levelData, temp);

		tempData.width = std::stoi(temp);

		// get type data
		std::getline(levelData, temp);

		tempData.type = static_cast<Type>(std::stoi(temp));

		// get b2bodytype data
		std::getline(levelData, temp);

		tempData.b2BodyType = static_cast<b2BodyType>(std::stoi(temp));

		// add the temp data to the data vector
		storeShapeData(tempData);
	}

	reset();
}

void Game::storeShapeData(ShapeData shapeData)
{
	m_shapeData.push_back(shapeData);
}

void Game::reset()
{
	m_bulletsCount = m_TOTAL_BULLETS;
	std::string bulletStr = "Bullets Left: " + std::to_string(m_bulletsCount);

	m_bulletCountText = loadFromRenderedText(bulletStr.c_str(), SDL_Color{ 0, 0, 0, 255 }, m_fontNormal, m_renderer);

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
			data.type,
			data.color);

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
	/*fps = 15.0f;
	m_gravity = b2Vec2{ 0.0f, 39.2f };

	m_world.SetGravity(m_gravity);

	m_timeStep = 1.0f / fps;*/

	int rerunAmount{ 3 };
	int shotAttempts{ 4 };
	std::vector<int> difficultyEstimation{};

	// how many times to rerun the sim
	for (int i{}; i < rerunAmount; ++i)
	{
		reset();
		m_gameState = GameState::GAMEPLAY;
		difficultyEstimation.push_back(0);

		int targetNumber{};
		for (ConvexShape& target : m_shapeSpawner)
		{
			// if the current shape is a target
			if (target.type() == Type::TARGET)
			{
				++targetNumber;

				if (target.marked())
				{
					printf("Target %d has been already previously hit\n", targetNumber);
					continue;
				}

				while (target.awake())
				{
					update();
					render();
				}
				printf("Target: %d is sleeping, preparing to shoot\n", targetNumber);

				for (int i{}; i < shotAttempts; ++i)
				{
					printf("Bullet has been shot\n");
					shoot(Vector2f{ target.position().x * SCALING_FACTOR, target.position().y * SCALING_FACTOR });
					++difficultyEstimation.back();

					while (m_currentBullet->awake())
					{
						update();
						render();

					}
					printf("Bullet has come to rest\n");

					if (target.marked())
					{
						printf("Target %d has been hit\n", targetNumber);
						break;
					}
				}
			}
		}
	}

	float difficulty = average(difficultyEstimation.data(), difficultyEstimation.size());

	printf("Difficulty: %f\n", difficulty);

	reset();
	m_gameState = GameState::EDIT;

	m_phaseText = loadFromRenderedText("Edit Phase", SDL_Color{ 0, 0, 0, 255 }, m_fontNormal, m_renderer);

	/*fps = 60.0f;
	m_gravity = b2Vec2{ 0.0f, 9.8f };

	m_world.SetGravity(m_gravity);

	m_timeStep = 1.0f / fps;*/
}

void Game::cleanUp()
{
	SDL_DestroyTexture(m_phaseText.texture);
	m_phaseText.texture = nullptr;

	SDL_DestroyTexture(m_bulletCountText.texture);
	m_bulletCountText.texture = nullptr;

	SDL_DestroyTexture(m_powerText.texture);
	m_powerText.texture = nullptr;

	SDL_DestroyTexture(m_rectangleText.texture);
	m_rectangleText.texture = nullptr;

	SDL_DestroyRenderer(m_renderer);
	SDL_DestroyWindow(m_window);
	TTF_CloseFont(m_fontNormal);
	m_fontNormal = nullptr;
	m_window = nullptr;
	m_renderer = nullptr;

	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

void Game::shoot(Vector2f targetPosition)
{
	b2Vec2 unit{ targetPosition.x - m_player->position().x * SCALING_FACTOR, targetPosition.y - m_player->position().y * SCALING_FACTOR };
	unit.Normalize();
	m_shapeSpawner.emplace_back(&m_world, Vector2f{ (m_player->position().x + unit.x / 4) * SCALING_FACTOR,
		(m_player->position().y + unit.y / 4) * SCALING_FACTOR }, 15, 15, b2_dynamicBody, Type::BULLET);

	m_currentBullet = &m_shapeSpawner.back();
	m_shapeSpawner.back().launch(unit, m_power);
}
