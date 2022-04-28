#include "../include/Game.h"

int runEstimation(void* data);

/// <summary>
/// Setup the Box2D simulation and SDL2 window, renderer, fonts, etc.
/// </summary>
Game::Game() :
	m_gameIsRunning{ false },
	m_groundShape{ &m_world, Vector2f{-1, SCREEN_HEIGHT - 70.0f}, SCREEN_WIDTH + 2, 100, b2_staticBody, Type::WALL, SDL_Color{220, 220, 220, 0xFF} },
	m_leftWallShape{ &m_world, Vector2f{SCREEN_WIDTH, 0}, 10, SCREEN_HEIGHT, b2_staticBody, Type::WALL, SDL_Color{220, 220, 220, 0xFF} },
	m_rightWallShape{ &m_world, Vector2f{-10, 0}, 10, SCREEN_HEIGHT, b2_staticBody, Type::WALL, SDL_Color{220, 220, 220, 0xFF} },
	m_roofShape{ &m_world, Vector2f{0, -10}, SCREEN_WIDTH, 10, b2_staticBody, Type::WALL, SDL_Color{220, 220, 220, 0xFF} },
	m_rectanglePrefab{ &m_world, Vector2f{-100, 0}, 20, 50, b2_dynamicBody },
	m_rectangleRotatedPrefab{ &m_world, Vector2f{-100, 0}, 50, 20, b2_dynamicBody },
	m_squarePrefab{ &m_world, Vector2f{-100, 0}, 20, 20, b2_dynamicBody },
	m_targetPrefab{ &m_world, Vector2f{-100, 0}, 20, 20, b2_dynamicBody, Type::TARGET, SDL_Color{ 240, 207, 46, 255 } },
	m_playerPrefab{ &m_world, Vector2f{-100, 0}, 20, 20, b2_staticBody, Type::PLAYER, SDL_Color{ 0x24, 0x3C, 0xAE, 0xFF } }
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

	m_sdlDraw.setRenderer(m_renderer);
	m_world.SetDebugDraw(&m_sdlDraw);
	m_sdlDraw.SetFlags(b2Draw::e_shapeBit);

	printf("Edit Phase\n");

	std::string bulletStr = "Bullets Left: " + std::to_string(m_bulletsCount);

	m_phaseText = loadFromRenderedText("Edit Phase", SDL_Color{ 0, 0, 0, 255 }, m_fontNormal, m_renderer);
	m_bulletCountText = loadFromRenderedText(bulletStr.c_str(), SDL_Color{ 0, 0, 0, 255 }, m_fontNormal, m_renderer);

	m_levelNameString = "Level Name";
	m_levelNameText = loadFromRenderedText(m_levelNameString.c_str(), SDL_Color{ 0, 0, 0, 255 }, m_fontTiny, m_renderer);

	std::string powerStr = "Power: " + std::to_string((int)(m_power / 8.0f)) + '%';

	m_powerText = loadFromRenderedText(powerStr.c_str(), SDL_Color{ 0, 0, 0, 255 }, m_fontNormal, m_renderer);
	m_warningText = loadFromRenderedText("Level needs a player and a target!", SDL_Color{ 0, 0, 0, 255 }, m_fontNormal, m_renderer);


	m_squareText = loadFromRenderedText("Square Block", SDL_Color{ 0, 0, 0, 255 }, m_fontTiny, m_renderer);
	m_rectangleText = loadFromRenderedText("Rectangle Block", SDL_Color{ 0, 0, 0, 255 }, m_fontTiny, m_renderer);
	m_rectangleRotatedText = loadFromRenderedText("Rectangle Block", SDL_Color{ 0, 0, 0, 255 }, m_fontTiny, m_renderer);
	m_targetText = loadFromRenderedText("Target Block", SDL_Color{ 0, 0, 0, 255 }, m_fontTiny, m_renderer);
	m_playerText = loadFromRenderedText("Player Block", SDL_Color{ 0, 0, 0, 255 }, m_fontTiny, m_renderer);

	m_helpText.texture = SDL_CreateTextureFromSurface(m_renderer,
		TTF_RenderText_Blended_Wrapped(m_fontNormal, "H:    Toggle Help\nG:    Run Difficulty Estimation\nS:    Save Level\nL:    Load Level\nSpacebar:    Toggle Edit/Play Mode",
			SDL_Color{ 0, 0, 0, 255 }, 400));

	LevelList::updateList();
}

/// <summary>
/// destructor, cleans up SDL2 variables
/// </summary>
Game::~Game()
{
	printf("~Game()\n");
	cleanUp();
}

/// <summary>
/// main game loop for SDL2
/// </summary>
void Game::run()
{
	m_gameIsRunning = true;
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

/// <summary>
/// main event handling loop, handles most of the key inputs
/// </summary>
/// <param name="e">sdl event</param>
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
			if (m_editorState != EditorState::ENTERTEXT
				&& m_editorState != EditorState::LOADLEVEL)
			{
				m_gameIsRunning = false;
			}
			else
			{
				m_editorState = EditorState::PLACE;
				m_loadLevelListY = 100.0f;
			}
		}

		if (e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_SPACE && m_editorState != EditorState::ENTERTEXT
			&& m_editorState != EditorState::LOADLEVEL)
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
				if (m_targetCount > 0 && m_playerPresent)
				{
					m_gameState = GameState::GAMEPLAY;
					m_skipStepTimer.restart();
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
			if (e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_s)
			{
				m_editorState = EditorState::ENTERTEXT;
				SDL_StartTextInput();
			}

			if (m_editorState != EditorState::ENTERTEXT)
			{
				if (e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_h)
				{
					m_showHelp = !m_showHelp;
				}


				if (e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_l)
				{
					m_editorState = EditorState::LOADLEVEL;
				}
			}

			if (e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_c)
			{
				clear();
			}

			if (e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_g && m_editorState != EditorState::ENTERTEXT
				&& m_editorState != EditorState::LOADLEVEL)
			{
				if (m_targetCount > 0 && m_player)
				{
					saveLevelData("sim-level");

					// run thread here
					m_threads.push_back(SDL_CreateThread(runEstimation, "Estimation Thread", (void*)NULL));
				}
				else
				{
					m_playSim = !m_playSim;
					m_showWarning = true;
					m_warningTimer.restart();
				}
			}
			if (m_editorState == EditorState::LOADLEVEL)
			{
				if (e.type == SDL_MOUSEWHEEL)
				{
					// scrolled up
					if (e.wheel.y > 0)
					{
						m_loadLevelListY -= m_scrollIncrement;
					}
					// scrolled down
					else if (e.wheel.y < 0)
					{
						m_loadLevelListY += m_scrollIncrement;
					}
				}
			}

			if (m_editorState == EditorState::ENTERTEXT)
			{
				bool updateText{ false };

				if (e.type == SDL_KEYDOWN)
				{
					if (e.key.keysym.sym == SDLK_BACKSPACE && m_levelNameString.length() > 0)
					{
						m_levelNameString.pop_back();
						updateText = true;
					}
					// cut & paste functionality
					else if (e.key.keysym.sym == SDLK_c && SDL_GetModState() & KMOD_CTRL)
					{
						SDL_SetClipboardText(m_levelNameString.c_str());
					}
					else if (e.key.keysym.sym == SDLK_v && SDL_GetModState() & KMOD_CTRL)
					{
						m_levelNameString = SDL_GetClipboardText();
						updateText = true;
					}
				}
				else if (e.type == SDL_TEXTINPUT)
				{
					// text input implementation. get any key as long as it is not a special character used for cut & paste, etc.
					if (!(SDL_GetModState() & KMOD_CTRL &&
						(e.text.text[0] == 'c' || e.text.text[0] == 'C' || e.text.text[0] == 'v' || e.text.text[0] == 'V'))
						&& m_levelNameString.length() <= 10)
					{
						m_levelNameString += e.text.text;
						updateText = true;
					}
				}

				if (updateText)
				{
					if (m_levelNameString != "")
					{
						m_levelNameText = loadFromRenderedText(m_levelNameString.c_str(), SDL_Color{ 0, 0, 0, 255 }, m_fontTiny, m_renderer);
					}
					else
					{
						m_levelNameText = loadFromRenderedText(" ", SDL_Color{ 0, 0, 0, 255 }, m_fontTiny, m_renderer);
					}
				}

				if (e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_RETURN)
				{
					if (m_targetCount > 0 && m_player && m_levelNameString.length() > 0)
					{
						m_editorState = EditorState::PLACE;
						saveLevelData(m_levelNameString);
						SDL_StopTextInput();
						m_levelSaveText = loadFromRenderedText("Level Saved!", SDL_Color{ 0, 0, 0, 255 }, m_fontNormal, m_renderer);
						LevelList::updateList();
						m_showLevelSave = true;
						m_levelSaveTimer.restart();
					}
					else
					{
						m_showWarning = true;
						m_warningTimer.restart();
					}
				}
			}
		}

		processMouseEvents(e);
	}
}

/// <summary>
/// mouse event handling loop, handles selecting shapes & placing selected shapes into the game world
/// </summary>
/// <param name="e">sdl event</param>
void Game::processMouseEvents(SDL_Event e)
{
	Uint32 buttons = SDL_GetMouseState(&mouseX, &mouseY);

	m_aimTargetPoint.x = static_cast<float>(mouseX);
	m_aimTargetPoint.y = static_cast<float>(mouseY);

	if (m_gameState == GameState::EDIT && mouseY < m_toolbarBg.y && m_editorState != EditorState::ENTERTEXT
		&& m_editorState != EditorState::LOADLEVEL)
	{
		// if the left mouse button is pressed down
		if (e.type == SDL_MOUSEBUTTONDOWN && ((buttons & SDL_BUTTON_LMASK) != 0))
		{
			// place player shape if a player shape is not present
			if (m_currentShape->type() == Type::PLAYER && !m_playerPresent)
			{
				m_shapeSpawner.emplace_back(&m_world,
					Vector2f{ static_cast<float>(mouseX), static_cast<float>(mouseY) },
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
				// place target shape if there are less than 3 targets
				if (m_currentShape->type() == Type::TARGET && m_targetCount < 3)
				{
					m_shapeSpawner.emplace_back(&m_world,
						Vector2f{ static_cast<float>(mouseX), static_cast<float>(mouseY) },
						m_currentShape->width(),
						m_currentShape->height(),
						m_currentShape->b2BodyDefType(),
						m_currentShape->type(),
						m_currentShape->color());

					storeShapeData(m_shapeSpawner.back().data());
					++m_targetCount;
				}

				// place any other shape
				if (m_currentShape->type() == Type::BLOCK)
				{
					m_shapeSpawner.emplace_back(&m_world,
						Vector2f{ static_cast<float>(mouseX), static_cast<float>(mouseY) },
						m_currentShape->width(),
						m_currentShape->height(),
						m_currentShape->b2BodyDefType(),
						m_currentShape->type(),
						m_currentShape->color());

					storeShapeData(m_shapeSpawner.back().data());
				}
			}
		}
	}
	// allows you to select shapes only in edit mode
	else if (m_gameState == GameState::EDIT && mouseY > m_toolbarBg.y)
	{
		if ((mouseX >= m_squareButton.position().x && mouseX <= m_squareButton.position().x + m_squareButton.width()) &&
			(mouseY >= m_squareButton.position().y && mouseY <= m_squareButton.position().y + m_squareButton.height()))
		{
			if (e.type == SDL_MOUSEBUTTONDOWN && ((buttons & SDL_BUTTON_LMASK) != 0))
			{
				m_currentShape = &m_squarePrefab;
				m_selectedButton = &m_squareButton;
				m_editorState = EditorState::PLACE;
			}
		}
		else if ((mouseX >= m_rectButton.position().x && mouseX <= m_rectButton.position().x + m_rectButton.width()) &&
			(mouseY >= m_rectButton.position().y && mouseY <= m_rectButton.position().y + m_rectButton.height()))
		{
			if (e.type == SDL_MOUSEBUTTONDOWN && ((buttons & SDL_BUTTON_LMASK) != 0))
			{
				m_currentShape = &m_rectanglePrefab;
				m_selectedButton = &m_rectButton;
				m_editorState = EditorState::PLACE;
			}
		}
		else if ((mouseX >= m_rectRotatedButton.position().x && mouseX <= m_rectRotatedButton.position().x + m_rectRotatedButton.width()) &&
			(mouseY >= m_rectRotatedButton.position().y && mouseY <= m_rectRotatedButton.position().y + m_rectRotatedButton.height()))
		{
			if (e.type == SDL_MOUSEBUTTONDOWN && ((buttons & SDL_BUTTON_LMASK) != 0))
			{
				m_currentShape = &m_rectangleRotatedPrefab;
				m_selectedButton = &m_rectRotatedButton;
				m_editorState = EditorState::PLACE;
			}
		}
		else if ((mouseX >= m_targetButton.position().x && mouseX <= m_targetButton.position().x + m_targetButton.width()) &&
			(mouseY >= m_targetButton.position().y && mouseY <= m_targetButton.position().y + m_targetButton.height()))
		{
			if (e.type == SDL_MOUSEBUTTONDOWN && ((buttons & SDL_BUTTON_LMASK) != 0))
			{
				m_currentShape = &m_targetPrefab;
				m_selectedButton = &m_targetButton;
				m_editorState = EditorState::PLACE;
			}
		}
		else if ((mouseX >= m_playerButton.position().x && mouseX <= m_playerButton.position().x + m_playerButton.width()) &&
			(mouseY >= m_playerButton.position().y && mouseY <= m_playerButton.position().y + m_playerButton.height()))
		{
			if (e.type == SDL_MOUSEBUTTONDOWN && ((buttons & SDL_BUTTON_LMASK) != 0))
			{
				m_currentShape = &m_playerPrefab;
				m_selectedButton = &m_playerButton;
				m_editorState = EditorState::PLACE;
			}
		}
	}

	// level selecting in load level mode
	if (m_editorState == EditorState::LOADLEVEL)
	{
		int row{}, col{};
		float x{ 100.0f };
		float gap{ 5 };

		for (int i{}; i < LevelList::levels().size(); ++i)
		{
			m_levelDialogBox.x = (m_levelDialogBox.w + gap) * row + x;
			m_levelDialogBox.y = (m_levelDialogBox.h + gap) * col + m_loadLevelListY;
			std::string levelName = LevelList::levels().at(i).erase(LevelList::levels().at(i).length() - 4);

			if (e.type == SDL_MOUSEBUTTONDOWN && ((buttons & SDL_BUTTON_LMASK) != 0))
			{
				if (m_aimTargetPoint.x <= m_levelDialogBox.x + m_levelDialogBox.w
					&& m_aimTargetPoint.x >= m_levelDialogBox.x
					&& m_aimTargetPoint.y <= m_levelDialogBox.y + m_levelDialogBox.h
					&& m_aimTargetPoint.y >= m_levelDialogBox.y)
				{
					printf("Clicked on Level: %s\n", levelName.c_str());
					loadLevelData(levelName);

					m_editorState = EditorState::PLACE;
					m_loadLevelListY = 100.0f;
					m_levelSaveText = loadFromRenderedText("Level Loaded!", SDL_Color{ 0, 0, 0, 255 }, m_fontNormal, m_renderer);
					m_showLevelSave = true;
					m_levelSaveTimer.restart();
				}
			}

			row++;
			if (row > 2)
			{
				row = 0;
				col++;
			}
		}
	}

	// shooting bullets in gameplay mode
	if (m_gameState == GameState::GAMEPLAY && m_player && m_shootMode && m_bulletsCount > 0)
	{
		if (e.type == SDL_MOUSEBUTTONDOWN && ((buttons & SDL_BUTTON_LMASK) != 0))
		{
			shoot(Vector2f{ static_cast<float>(mouseX), static_cast<float>(mouseY) });
			m_shootMode = false;
			--m_bulletsCount;

			std::string bulletStr = "Bullets Left: " + std::to_string(m_bulletsCount);

			m_phaseText = loadFromRenderedText("Simulate Phase", SDL_Color{ 0, 0, 0, 255 }, m_fontNormal, m_renderer);
			m_bulletCountText = loadFromRenderedText(bulletStr.c_str(), SDL_Color{ 0, 0, 0, 255 }, m_fontNormal, m_renderer);
			printf("Simulate Phase\n");
			m_skipStepTimer.restart();
		}
	}
}

/// <summary>
/// main game update loop, updates the Box2D physics simulation & checks for gameplay victory/loss.
/// allows the player to increase/decrease shot power level
/// </summary>
void Game::update()
{
	if (m_gameState == GameState::GAMEPLAY)
	{
		m_showWarning = false;
		m_showLevelSave = false;

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
		else
		{
			if (keyState[SDL_SCANCODE_LEFT])
			{
				if (keyState[SDL_SCANCODE_LSHIFT])
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

			if (keyState[SDL_SCANCODE_RIGHT])
			{
				if (keyState[SDL_SCANCODE_LSHIFT])
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

/// <summary>
/// main SDL2 render method, all of the shapes are rendered using SDLDraw class but the UI is still rendered here
/// </summary>
void Game::render()
{
	SDL_SetRenderDrawColor(m_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(m_renderer);

	m_world.DebugDraw();

	if (m_gameState == GameState::EDIT)
	{
		if (mouseY < m_toolbarBg.y && m_currentShape && m_editorState == EditorState::PLACE)
		{
			m_currentShape->renderShadow(m_renderer, Vector2f{ static_cast<float>(mouseX), static_cast<float>(mouseY) });
		}

		if (m_showHelp)
		{
			SDL_FRect helpRect{ SCREEN_WIDTH - 250, SCREEN_HEIGHT - m_toolbarBg.h - 110, 250, 100 };
			SDL_RenderCopyF(m_renderer, m_helpText.texture, NULL, &helpRect);
		}

		SDL_SetRenderDrawColor(m_renderer, 128, 128, 128, 128);
		SDL_RenderFillRectF(m_renderer, &m_toolbarBg);

		if (m_editorState == EditorState::ENTERTEXT)
		{
			SDL_RenderFillRectF(m_renderer, &m_saveDialogBox);
			SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255);
			renderText(m_renderer, &m_levelNameText, Vector2f{ m_saveDialogBox.x + 10.0f, m_saveDialogBox.y + 10.0f });
		}

		else if (m_editorState == EditorState::LOADLEVEL)
		{
			renderLevelSelect();
		}

		SDL_SetRenderDrawColor(m_renderer, 0x00, 0x00, 0x00, 0xFF);
		m_rectButton.render(m_renderer);
		m_rectRotatedButton.render(m_renderer);
		m_squareButton.render(m_renderer);
		m_targetButton.render(m_renderer);
		m_playerButton.render(m_renderer);

		SDL_RenderFillRectF(m_renderer, &m_squareShapeSelect);
		SDL_RenderFillRectF(m_renderer, &m_rectangleShapeSelect);
		SDL_RenderFillRectF(m_renderer, &m_rectangleRotatedShapeSelect);

		renderText(m_renderer, &m_squareText, Vector2f{ m_squareButton.position().x - m_squareButton.width() / 4,
										m_squareButton.position().y + 45.0f, });

		renderText(m_renderer, &m_rectangleText, Vector2f{ m_rectButton.position().x - m_rectButton.width() / 2,
											m_rectButton.position().y + 45.0f, });

		renderText(m_renderer, &m_rectangleRotatedText, Vector2f{ m_rectRotatedButton.position().x - m_rectRotatedButton.width() / 2,
											m_rectRotatedButton.position().y + 45.0f, });

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
			SDL_SetRenderDrawColor(m_renderer, 0x24, 0x3C, 0xAE, 0xFF);
			SDL_RenderDrawLine(m_renderer, m_player->position().x * SCALING_FACTOR, m_player->position().y * SCALING_FACTOR, m_aimTargetPoint.x, m_aimTargetPoint.y);
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

/// <summary>
/// render the level select list
/// </summary>
void Game::renderLevelSelect()
{
	int row{}, col{};
	float x{ 100.0f };
	float gap{ 5 };

	for (int i{}; i < LevelList::levels().size(); ++i)
	{
		m_levelDialogBox.x = (m_levelDialogBox.w + gap) * row + x;
		m_levelDialogBox.y = (m_levelDialogBox.h + gap) * col + m_loadLevelListY;
		std::string levelName = LevelList::levels().at(i).erase(LevelList::levels().at(i).length() - 4);

		m_loadLevelNameText = loadFromRenderedText(levelName.c_str(), SDL_Color{ 0, 0, 0, 255 }, m_fontTiny, m_renderer);
		SDL_SetRenderDrawColor(m_renderer, 128, 128, 128, 128);
		SDL_RenderFillRectF(m_renderer, &m_levelDialogBox);
		SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255);
		renderText(m_renderer, &m_loadLevelNameText, Vector2f{ m_levelDialogBox.x + 10.0f, m_levelDialogBox.y + 10.0f });

		row++;
		if (row > 2)
		{
			row = 0;
			col++;
		}
	}
}

/// <summary>
/// saves the shape data from current level into a file
/// </summary>
/// <param name="fileName">name of level file</param>
void Game::saveLevelData(const std::string& fileName)
{
	printf("Saving level data to file\n");

	std::ofstream levelData("levels/" + fileName + ".txt");

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
		levelData << data.angle << "\n";
	}

	levelData.close();
}

/// <summary>
/// loads a level from a file
/// </summary>
/// <param name="fileName">name of level file</param>
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

/// <summary>
/// store the current shape data into a vector of shape data
/// </summary>
/// <param name="shapeData">current shape data</param>
void Game::storeShapeData(ShapeData shapeData)
{
	m_shapeData.push_back(shapeData);
}

/// <summary>
/// reset the current levels shapes by reloading previously saved shape data
/// </summary>
void Game::reset()
{
	m_bulletsCount = m_TOTAL_BULLETS;
	std::string bulletStr = "Bullets Left: " + std::to_string(m_bulletsCount);

	m_bulletCountText = loadFromRenderedText(bulletStr.c_str(), SDL_Color{ 0, 0, 0, 255 }, m_fontNormal, m_renderer);

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
			id);

		if (data.type == Type::PLAYER)
		{
			m_player = &m_shapeSpawner.back();
			m_playerPresent = true;
		}

		else if (data.type == Type::TARGET)
		{
			m_targetCount++;
		}
		id++;
	}
}

/// <summary>
/// clear the level
/// </summary>
void Game::clear()
{
	m_shapeSpawner.clear();
	m_playerPresent = false;
	m_targetCount = 0;
	m_player = nullptr;
	m_shapeData.clear();
}

/// <summary>
/// SDL2 thread function that runs a command to open the difficulty estimation program
/// </summary>
/// <param name="data"></param>
/// <returns></returns>
int runEstimation(void* data)
{
	system("DifficultyEstimation.exe sim-level");
	printf("Thread finished\n");

	return 0;
}

/// <summary>
/// clean up of all SDL2 variables for destruction of the Game class
/// </summary>
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

	SDL_DestroyTexture(m_rectangleRotatedText.texture);
	m_rectangleRotatedText.texture = nullptr;

	SDL_DestroyTexture(m_helpText.texture);
	m_helpText.texture = nullptr;

	SDL_DestroyRenderer(m_renderer);
	SDL_DestroyWindow(m_window);
	TTF_CloseFont(m_fontNormal);
	m_fontNormal = nullptr;
	m_window = nullptr;
	m_renderer = nullptr;

	for (auto& thread : m_threads)
	{
		SDL_WaitThread(thread, nullptr);
	}

	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

/// <summary>
/// shoots a bullet type shape in a given direction
/// </summary>
/// <param name="targetPosition">position to shoot bullet at</param>
void Game::shoot(Vector2f targetPosition)
{
	b2Vec2 unit{ targetPosition.x - m_player->position().x * SCALING_FACTOR, targetPosition.y - m_player->position().y * SCALING_FACTOR };
	unit.Normalize();
	m_shapeSpawner.emplace_back(&m_world, Vector2f{ (m_player->position().x + unit.x / 4) * SCALING_FACTOR,
		(m_player->position().y + unit.y / 4) * SCALING_FACTOR }, 15, 15, b2_dynamicBody, Type::BULLET, SDL_Color{ 255, 0, 0, 255 });

	m_currentBullet = &m_shapeSpawner.back();
	m_shapeSpawner.back().launch(unit, m_power);
}
