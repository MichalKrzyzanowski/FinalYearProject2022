#ifndef GAME_H
#define GAME_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <SDL_thread.h>
#include <iostream>
#include <string>
#include <algorithm>
#include <box2d.h>
#include <fstream>
#include <filesystem>
#include <Windows.h>
#include "ConvexShape.h"
#include "Utility.h"
#include "Timer.h"
#include "Button.h"
#include "ConvexShapeContactListener.h"
#include "SDLDraw.h"
#include "LevelList.h"

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

enum class GameState
{
	EDIT,
	GAMEPLAY,
	WIN,
	LOSE
};

enum class EditorState
{
	PLACE,
	ENTERTEXT,
	LOADLEVEL
};

class Game
{
public:
	Game();
	~Game();
	void run();

private:

	void processEvents(SDL_Event e);
	void processMouseEvents(SDL_Event e);
	void update();
	void render();
	void renderLevelSelect();

	void saveLevelData(const std::string& fileName);
	void loadLevelData(const std::string& fileName);

	void storeShapeData(ShapeData shapeData);
	void reset();
	void clear();

	void cleanUp();
	void shoot(Vector2f targetPosition);

	bool m_gameIsRunning;
	bool m_shootMode{ false };

	std::vector<SDL_Thread*> m_threads{};

	SDL_Window* m_window;
	SDL_Renderer* m_renderer;
	TTF_Font* m_fontNormal;
	TTF_Font* m_fontSmall;
	TTF_Font* m_fontTiny;

	bool m_showWarning{ false };
	bool m_showHelp{ true };
	Timer m_warningTimer{};

	bool m_showLevelSave{ false };
	Timer m_levelSaveTimer{};

	// text objects
	LTexture m_phaseText;
	LTexture m_powerText;
	LTexture m_bulletCountText;
	LTexture m_warningText;
	LTexture m_levelSaveText;

	LTexture m_rectangleText;
	LTexture m_rectangleRotatedText;
	LTexture m_squareText;
	LTexture m_targetText;
	LTexture m_playerText;

	LTexture m_helpText;

	// box2d setup
	float fps{ 60.0f };

	float m_simSpeed{ 1.0f };

	float m_skipTimerGoal{ 5.0f };

	Timer m_skipStepTimer{};

	b2Vec2 m_gravity{ 0.0f, 9.8f }; // fps 60: grav = 9.8f
	b2World m_world{ m_gravity };

	float m_timeStep{ 1.0f / fps };
	int32_t m_velocityIterations{ 2000 };
	int32_t m_positionIterations{ 2000 };

	// boundary shapes
	ConvexShape m_groundShape;
	ConvexShape m_leftWallShape;
	ConvexShape m_rightWallShape;
	ConvexShape m_roofShape;

	GameState m_gameState = GameState::EDIT;
	EditorState m_editorState = EditorState::PLACE;

	// shot variables
	const Uint8* keyState = SDL_GetKeyboardState(nullptr);
	float m_power{ 400.0f };
	float m_powerGain{ 2.0f };

	const float m_MAX_POWER{ 800.0f };
	const float m_MIN_POWER{ 200.0f };

	const int m_TOTAL_BULLETS{ 3 };
	int m_bulletsCount{ m_TOTAL_BULLETS };

	SDL_FPoint m_aimTargetPoint{};

	bool m_playSim{ false };
	int m_targetCount{ 0 };

	int mouseX, mouseY;
	std::vector<ConvexShape> m_shapeSpawner{};
	std::vector<ShapeData> m_shapeData{};
	ConvexShape* m_currentBullet{ nullptr };

	// prefabs
	ConvexShape m_rectanglePrefab;
	ConvexShape m_rectangleRotatedPrefab;
	ConvexShape m_squarePrefab;
	ConvexShape m_targetPrefab;
	ConvexShape m_playerPrefab;

	ConvexShape* m_currentShape{};
	ConvexShape* m_player{};
	bool m_playerPresent{ false };

	ConvexShapeContactListener m_contactListener;

	SDLDraw m_sdlDraw;

	// UI
	SDL_FRect m_toolbarBg{ 0.0f, SCREEN_HEIGHT - 70.0f, SCREEN_WIDTH, 70.0f };
	UI::Button m_squareButton{ Vector2f{m_toolbarBg.x + 20.0f, m_toolbarBg.y + 10.0f} };
	UI::Button m_rectButton{ Vector2f{m_toolbarBg.x + 120.0f, m_toolbarBg.y + 10.0f} };
	UI::Button m_rectRotatedButton{ Vector2f{m_toolbarBg.x + 220.0f, m_toolbarBg.y + 10.0f} };
	UI::Button m_targetButton{ Vector2f{m_toolbarBg.x + 320.0f, m_toolbarBg.y + 10.0f} };
	UI::Button m_playerButton{ Vector2f{m_toolbarBg.x + 420.0f, m_toolbarBg.y + 10.0f} };

	UI::Button* m_selectedButton{ nullptr };

	SDL_FRect m_rectangleShapeSelect{ m_rectButton.position().x + 15.0f,
										m_rectButton.position().y + 10.0f,
										10, 20 };

	SDL_FRect m_rectangleRotatedShapeSelect{ m_rectRotatedButton.position().x + 10.0f,
										m_rectRotatedButton.position().y + 15.0f,
										20, 10 };

	SDL_FRect m_squareShapeSelect{ m_squareButton.position().x + 15.0f,
									m_squareButton.position().y + 15.0f,
										10, 10 };

	SDL_FRect m_targetSelect{ m_targetButton.position().x + 15.0f,
									m_targetButton.position().y + 15.0f,
										10, 10 };
	
	SDL_FRect m_playerSelect{ m_playerButton.position().x + 15.0f,
									m_playerButton.position().y + 15.0f,
										10, 10 };

	// saving UI
	SDL_FRect m_saveDialogBox { 10.0f, 50.0f, 120.0f, 30.0f};
	std::string m_levelNameString;
	LTexture m_levelNameText;

	// loading UI
	SDL_FRect m_levelDialogBox{ 10.0f, 50.0f, 120.0f, 30.0f };
	LTexture m_loadLevelNameText;
	float m_loadLevelListY{ 100.0f };
	float m_scrollIncrement{ 10.0f };
};

#endif // !GAME_H