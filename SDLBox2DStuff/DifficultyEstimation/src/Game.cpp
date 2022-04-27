#include "../include/Game.h"

Game::Game(int mainArgsCount, char** mainArgs) :
	m_gameIsRunning{ true },
	m_groundShape{ &m_world, Vector2f{-1, SCREEN_HEIGHT - 70.0f}, SCREEN_WIDTH + 2, 100, b2_staticBody, Type::WALL, SDL_Color{220, 220, 220, 0xFF} },
	m_leftWallShape{ &m_world, Vector2f{SCREEN_WIDTH, 0}, 10, SCREEN_HEIGHT, b2_staticBody, Type::WALL, SDL_Color{220, 220, 220, 0xFF} },
	m_rightWallShape{ &m_world, Vector2f{-10, 0}, 10, SCREEN_HEIGHT, b2_staticBody, Type::WALL, SDL_Color{220, 220, 220, 0xFF} },
	m_roofShape{ &m_world, Vector2f{0, -10}, SCREEN_WIDTH, 10, b2_staticBody, Type::WALL, SDL_Color{220, 220, 220, 0xFF} },
	m_rectanglePrefab{ &m_world, Vector2f{-100, 0}, 20, 50, b2_dynamicBody },
	m_squarePrefab{ &m_world, Vector2f{-100, 0}, 20, 20, b2_dynamicBody },
	m_targetPrefab{ &m_world, Vector2f{-100, 0}, 20, 20, b2_dynamicBody, Type::TARGET, SDL_Color{ 240, 207, 46, 255 } },
	m_playerPrefab{ &m_world, Vector2f{-100, 0}, 20, 20, b2_staticBody, Type::PLAYER, SDL_Color{ 0x24, 0x3C, 0xAE, 0xFF } }
{
	//ShowWindow(GetConsoleWindow(), SW_HIDE);

	if (mainArgsCount > 1)
	{
		m_levelName = mainArgs[1];
	}

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
	}

	int imgFlags = IMG_INIT_PNG;

	if (!IMG_Init((imgFlags)&imgFlags))
	{
		printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
	}

	if (TTF_Init() < 0)
	{
		printf("SDL_ttf could not initialize! SDL_image Error: %s\n", TTF_GetError());
	}

	printf("level selected: %s\n", m_levelName.c_str());
	m_window = SDL_CreateWindow("SDL game loop",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		SCREEN_WIDTH, SCREEN_HEIGHT,
		SDL_WINDOW_SHOWN);

	if (m_window == nullptr)
	{
		printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
	}

	m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (m_window == nullptr)
	{
		printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
	}
	SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);

	m_fontNormal = TTF_OpenFont("assets/LemonMilk.otf", 18);
	if (m_fontNormal == nullptr)
	{
		printf("failed to load font\n");
	}

	m_shapeSpawner.reserve(10000); // reserve some space for ALOT of squares
	m_currentShape = &m_rectanglePrefab;
	m_world.SetContactListener(&m_contactListener);

	m_sdlDraw.setRenderer(m_renderer);
	m_world.SetDebugDraw(&m_sdlDraw);
	m_sdlDraw.SetFlags(b2Draw::e_shapeBit);

	std::string bulletStr = "Bullets Left: " + std::to_string(m_bulletsCount);

	m_phaseText = loadFromRenderedText("Estimation Phase", SDL_Color{ 0, 0, 0, 255 }, m_fontNormal, m_renderer);
	printf("got here\n");

	m_bulletCountText = loadFromRenderedText(bulletStr.c_str(), SDL_Color{ 0, 0, 0, 255 }, m_fontNormal, m_renderer);

	std::string powerStr = "Power: " + std::to_string((int)(m_power / 8.0f)) + '%';

	m_powerText = loadFromRenderedText(powerStr.c_str(), SDL_Color{ 0, 0, 0, 255 }, m_fontNormal, m_renderer);

	clearResultsFile();

	estimateDifficulty();
}

Game::~Game()
{
	printf("~Game()\n");
	cleanUp();
}

void Game::run()
{
	SDL_Event e{};
	Timer fpsTimer{};
	int countedFrames{};
	fpsTimer.start();

	while (m_gameIsRunning)
	{
		float avgFPS = countedFrames / (fpsTimer.getTicks() / 1000.0f);
		if (avgFPS > 2000000.0f)
		{
			avgFPS = 0.0f;
		}

		std::string fps{ "simulation running @ " + std::to_string(avgFPS) + "fps" };

		SDL_SetWindowTitle(m_window, fps.c_str());

		processEvents(e);
		update();
		render();
		++countedFrames;
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
	}
}

void Game::update()
{
	m_world.Step(m_timeStep * m_simSpeed, m_velocityIterations, m_positionIterations);

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
			if (shape.awake())
			{
				readyToShoot = false;
				break;
			}
		}

		bool gameWin{ true };

		for (ConvexShape& target : m_shapeSpawner)
		{
			if (target.type() == Type::TARGET)
			{
				if (!target.marked())
				{
					gameWin = false;
					break;
				}
			}
		}

		if (m_skipStepTimer.getTicksAsSeconds() >= m_skipTimerGoal)
		{
			readyToShoot = true;
			m_skipStepTimer.restart();
		}

		if (readyToShoot)
		{
			m_shootMode = true;
		}
	}

	if (m_state == GameState::DONE)
	{
		if (m_quitTimer.getTicksAsSeconds() >= 5.0f)
		{
			quit();
		}
	}
}

void Game::render()
{
	SDL_SetRenderDrawColor(m_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(m_renderer);

	m_world.DebugDraw();

	if (m_player)
	{
		SDL_SetRenderDrawColor(m_renderer, 0x24, 0x3C, 0xAE, 0xFF);
		SDL_RenderDrawLine(m_renderer, m_player->position().x * SCALING_FACTOR, m_player->position().y * SCALING_FACTOR, m_aimTargetPoint.x, m_aimTargetPoint.y);
	}

	SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255);
	renderText(m_renderer, &m_powerText, Vector2f{ 10, SCREEN_HEIGHT - static_cast<float>(m_phaseText.height) - 10 });


	SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255);
	renderText(m_renderer, &m_phaseText, Vector2f{ SCREEN_WIDTH - static_cast<float>(m_phaseText.width) - 10, 10 });
	renderText(m_renderer, &m_bulletCountText, Vector2f{ SCREEN_WIDTH - static_cast<float>(m_bulletCountText.width) - 10, 40 });

	SDL_RenderPresent(m_renderer);
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
	std::ifstream levelData("levels/" + fileName + ".txt");

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

		// get angle data
		std::getline(levelData, temp);

		tempData.angle = std::stof(temp);

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
	m_shapeSpawner.clear();
	m_playerPresent = false;
	m_targetCount = 0;
	m_player = nullptr;
	int id{};

	for (ShapeData& data : m_shapeData)
	{
		m_shapeSpawner.emplace_back(&m_world,
			Vector2f{ data.position },
			data.width,
			data.height,
			data.b2BodyType,
			data.type,
			data.color,
			data.angle,
			id,
			data.marked);

		if (data.type == Type::PLAYER)
		{
			m_player = &m_shapeSpawner.back();
			m_playerPresent = true;
		}

		else if (data.type == Type::TARGET)
		{
			m_targetCount++;
		}

		if (data.type != Type::BULLET)
		{
			id++;
		}
	}
}

void Game::updateShapeData(std::vector<ShapeData>& dataVec)
{
	for (int i{}; i < m_shapeSpawner.size(); ++i)
	{
		if (m_shapeSpawner.at(i).type() != Type::BULLET)
		{
			dataVec.at(i).position.x = (m_shapeSpawner.at(i).position().x * SCALING_FACTOR) - dataVec.at(i).width / 2.0f;
			dataVec.at(i).position.y = (m_shapeSpawner.at(i).position().y * SCALING_FACTOR) - dataVec.at(i).height / 2.0f;
			dataVec.at(i).angle = m_shapeSpawner.at(i).angle();
			dataVec.at(i).marked = m_shapeSpawner.at(i).marked();
		}
	}
}

void Game::estimateDifficulty()
{
	SDL_Event e{};
	loadLevelData(m_levelName);

	m_power = m_MIN_POWER;

	int powerStep{ 1 };
	int angleIncrement{ 10 };
	int scoredShotCount{};

	std::vector<int> m_scores{};
	m_scores.reserve(5000);

	int scoredShots{}; // this will be used to estimate the level difficulty

	bool m_allTargetsHit{ false };

	// used to see which targets have been hit with previous bullets
	std::vector<std::pair<int, bool>> targetHitTracker{};

	for (ConvexShape& shape : m_shapeSpawner)
	{
		if (shape.type() == Type::TARGET)
		{
			std::pair<int, bool> temp{};
			temp.first = shape.data().id;
			temp.second = false;

			targetHitTracker.push_back(temp);
		}
	}

	// loop through the bullets
	for (int bulletCount{ m_TOTAL_BULLETS }; bulletCount > 0; --bulletCount)
	{
		if (bulletCount < m_TOTAL_BULLETS) m_skipTimerGoal = 0.5f;

		printf("Starting new run. bullets: %d\n", bulletCount);

		if (m_allTargetsHit)
		{
			break;
		}
		
		bool targetHit{ false };

		reset();

		for (int i{}; i < m_shapeSpawner.size(); ++i)
		{
			if (m_shapeSpawner.at(i).type() == Type::TARGET && m_shapeSpawner.at(i).marked())
			{
				for (int j{}; j < targetHitTracker.size(); ++j)
				{
					if (targetHitTracker.at(j).first == m_shapeSpawner.at(i).data().id)
					{
						targetHitTracker.at(j).second = true;
						break;
					}
				}
			}
		}

		std::string bulletStr = "Bullets Left: " + std::to_string(bulletCount);

		m_bulletCountText = loadFromRenderedText(bulletStr.c_str(), SDL_Color{ 0, 0, 0, 255 }, m_fontNormal, m_renderer);

		m_power = m_MIN_POWER;

		m_scores.clear();

		std::vector<std::vector<ShapeData>> levelData{};
		levelData.reserve(300);

		// loop through each power level
		for (; m_power < m_MAX_POWER; m_power += ((m_MAX_POWER - m_MIN_POWER) / 2))
		{
			if (m_allTargetsHit || targetHit)
			{
				break;
			}

			printf("Setting Power\n");

			if (m_power >= m_MAX_POWER)
			{
				m_power = m_MAX_POWER;
			}

			std::string powerStr = "Power: " + std::to_string((int)(m_power / 8.0f)) + '%';

			m_powerText = loadFromRenderedText(powerStr.c_str(), SDL_Color{ 0, 0, 0, 255 }, m_fontNormal, m_renderer);

			SDL_Vertex shotTarget{};

			shotTarget.position = SDL_FPoint{ (m_player->position().x * SCALING_FACTOR) + 30.0f, (m_player->position().y * SCALING_FACTOR) };

			// loop through a 360 degree circle
			for (int j{}; j < 360; j += angleIncrement)
			{
				if (m_allTargetsHit || targetHit)
				{
					break;
				}

				rotatePoint((m_player->position().x * SCALING_FACTOR), (m_player->position().y * SCALING_FACTOR), Deg2Rad(angleIncrement), shotTarget.position);

				int currentScore{ 0 };

				printf("Starting circle loop\n");
				reset();

				// prepares the level for a shot 
				while (true)
				{
					bool shotReady{ true };

					for (ConvexShape& shape : m_shapeSpawner)
					{
						if (shape.awake())
						{
							shotReady = false;
							break;
						}
					}

					while (SDL_PollEvent(&e) != 0)
					{
						if (e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_ESCAPE)
						{
							quit();
							return;
						}
					}

					update();
					render();

					if (m_skipStepTimer.getTicksAsSeconds() >= m_skipTimerGoal)
					{
						shotReady = true;
					}

					if (shotReady)
					{
						m_skipStepTimer.restart();
						m_simSpeed = 10.0f;
						shoot(Vector2f{ shotTarget.position.x, shotTarget.position.y });
						m_aimTargetPoint.x = shotTarget.position.x;
						m_aimTargetPoint.y = shotTarget.position.y;
						break;
					}
				}


				// simulate the shot
				while (true)
				{
					bool endShotSim{ true };

					for (ConvexShape& shape : m_shapeSpawner)
					{
						if (shape.awake())
						{
							endShotSim = false;
							break;
						}
					}

					while (SDL_PollEvent(&e) != 0)
					{
						if (e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_ESCAPE)
						{
							quit();
							return;
						}
					}

					update();
					render();

					if (m_skipStepTimer.getTicksAsSeconds() >= m_skipTimerGoal)
					{
						endShotSim = true;
					}

					// calculate the score of the shot
					if (endShotSim)
					{
						std::vector<ShapeData> tempLevelData{};

						// checking if any target has been hit & storing level data
						int targetsHit{};

						// go through the targets from previous iterations that where shot first
						for (int l{}; l < targetHitTracker.size(); ++l)
						{
							if (targetHitTracker.at(l).second)
							{
								targetsHit++;
							}
						}

						if (targetsHit >= m_targetCount)
						{
							printf("all targets hit!\n");
							m_allTargetsHit = true;
						}

						if (!m_allTargetsHit)
						{
							for (ConvexShape& shape : m_shapeSpawner)
							{
								if (shape.type() != Type::BULLET)
								{
									tempLevelData.push_back(shape.data());
								}

								if (shape.type() == Type::TARGET)
								{
									if (shape.marked())
									{
										for (int k{}; k < targetHitTracker.size(); ++k)
										{
											if (targetHitTracker.at(k).first == shape.data().id)
											{
												if (!targetHitTracker.at(k).second)
												{
													currentScore += 100;
													targetHit = true;
													targetsHit++;
													break;
												}
											}
										}

										if (targetsHit >= m_targetCount)
										{
											printf("all targets hit!\n");
											m_allTargetsHit = true;
										}
									}
								}
							}
						}

						currentScore += calculateDistanceScore();

						// track results data
						if (currentScore >= 5)
						{
							scoredShotCount++;
							std::string result = std::to_string(currentScore) + "pts, shot: "
								+ std::to_string(scoredShotCount) + '\n';
							storeResult(result.c_str());
						}

						if (currentScore > 15) scoredShots++; // will be used to determine the final difficulty rating
						printf("Iteration done. Score: %dpts\n\n", currentScore);

						m_scores.push_back(currentScore);
						m_simSpeed = 1.0f;
						m_skipStepTimer.restart();

						// store the run end level data
						updateShapeData(tempLevelData);
						levelData.push_back(tempLevelData);

						break;
					}
				}
			}
		}

		int bestScore{ m_scores.at(0) };
		int bestScoreIndex{};

		for (int i{ 1 }; i < m_scores.size(); ++i)
		{
			if (m_scores.at(i) > bestScore)
			{
				bestScore = m_scores.at(i);
				bestScoreIndex = i;
			}
		}

		m_shapeData = levelData.at(bestScoreIndex);

		printf("Best Score: %dpts\n\n", bestScore);

	}
	int finalDifficulty{ evaluateDifficulty(scoredShots) };

	printf("Estimation Complete!\n Difficulty Rating: %d/10\n\n", finalDifficulty);

	std::string rating = "Difficulty Rating:" + std::to_string(finalDifficulty) + "/10";

	m_phaseText = loadFromRenderedText(rating.c_str(), SDL_Color{ 0, 0, 0, 255 }, m_fontNormal, m_renderer);

	m_state = GameState::DONE;
	m_quitTimer.restart();
}

void Game::quit()
{
	ShowWindow(GetConsoleWindow(), SW_SHOW);
	m_gameIsRunning = false;
}

int Game::calculateDistanceScore()
{
	int score{};
	std::vector<float> distances{};

	for (ConvexShape& shape : m_shapeSpawner)
	{
		if (shape.type() == Type::TARGET)
		{
			if (!shape.marked())
			{
				float distance = std::hypotf((m_currentBullet->position().x - shape.position().x) * SCALING_FACTOR,
					(m_currentBullet->position().y - shape.position().y) * SCALING_FACTOR);

				distances.push_back(distance);
			}
		}
	}

	if (!distances.empty())
	{
		float shortestDistance{ distances.at(0) };

		for (int i{ 1 }; i < distances.size(); ++i)
		{
			if (distances.at(i) < shortestDistance)
			{
				shortestDistance = distances.at(i);
			}
		}

		return distanceScoreEvaluation(shortestDistance);
	}

	return 0;
}

/// <summary>
/// Function that returns a score value based on what category the shortest distance 
/// belongs to
/// </summary>
/// <param name="shortestDistance">shortest distance</param>
/// <returns>score</returns>
int Game::distanceScoreEvaluation(int shortestDistance)
{
	if (shortestDistance < 200.0f && shortestDistance > 150.0f)
	{
		return 5;
	}
	else if (shortestDistance < 150.0f && shortestDistance > 100.0f)
	{
		return 15;
	}
	else if (shortestDistance < 100.0f && shortestDistance > 50.0f)
	{
		return 25;
	}
	else if (shortestDistance < 50.0f && shortestDistance > 0.0f)
	{
		return 50;
	}

	return 0;
}

int Game::evaluateDifficulty(int bestScore)
{
	if (bestScore > 0.0f && bestScore <= 3.0f)
	{
		return 1;
	}
	else if (bestScore > 3.0f && bestScore <= 10.0f)
	{
		return 2;
	}
	else if (bestScore > 10.0f && bestScore <= 20.0f)
	{
		return 3;
	}
	else if (bestScore > 20.0f && bestScore <= 30.0f)
	{
		return 4;
	}
	else if (bestScore > 30.0f && bestScore <= 40.0f)
	{
		return 5;
	}
	else if (bestScore > 40.0f && bestScore <= 50.0f)
	{
		return 6;
	}
	else if (bestScore > 50.0f && bestScore <= 60.0f)
	{
		return 7;
	}
	else if (bestScore > 60.0f && bestScore <= 70.0f)
	{
		return 8;
	}
	else if (bestScore > 70.0f && bestScore <= 80.0f)
	{
		return 9;
	}
	else if (bestScore > 80.0f)
	{
		return 10;
	}

	return 0;
}

void Game::storeResult(const char* data)
{
	std::ofstream results("results/results-draft.txt", std::ios::app);

	results << data << '\n';

	results.close();
}

void Game::clearResultsFile()
{
	std::ofstream results("results/results-draft.txt", std::ios::out | std::ios::trunc);

	results.close();
}

void Game::cleanUp()
{
	SDL_DestroyTexture(m_phaseText.texture);
	m_phaseText.texture = nullptr;

	SDL_DestroyTexture(m_bulletCountText.texture);
	m_bulletCountText.texture = nullptr;

	SDL_DestroyTexture(m_powerText.texture);
	m_powerText.texture = nullptr;

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
		(m_player->position().y + unit.y / 4) * SCALING_FACTOR }, 15, 15, b2_dynamicBody, Type::BULLET, SDL_Color{ 255, 0, 0, 255 });

	m_currentBullet = &m_shapeSpawner.back();
	m_shapeSpawner.back().launch(unit, m_power);
}
