#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <iostream>
#include <string>
#include <algorithm>
#include <box2d.h>
#include <fstream>
#include "Rect.h"
#include "ConvexShape.h"
#include "Utility.h"
#include "ConvexShape.h"
#include "Timer.h"
#include "Circle.h"
#include "Button.h"
#include "ConvexShapeContactListener.h"
#include "SDLDraw.h"

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

enum class GameState
{
	EDIT,
	GAMEPLAY,
	WIN,
	LOSE
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

	void saveLevelData(const std::string& fileName);
	void loadLevelData(const std::string& fileName);

	void storeShapeData(ShapeData shapeData);
	void reset();

	void estimateDifficulty();
	void quitEstimation();
	int calculateDistanceScore();
	int distanceScoreEvaluation(int shortestDistance);
	int evaluateDifficulty(int bestScore);

	void cleanUp();
	void shoot(Vector2f targetPosition);

	bool m_gameIsRunning;
	bool m_shootMode{ false };

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
	LTexture m_squareText;
	LTexture m_targetText;
	LTexture m_playerText;

	LTexture m_helpText;

	// box2d setup
	float fps{ 60.0f };

	float m_simSpeed{ 1.0f };

	Timer m_skipStepTimer{};

	b2Vec2 m_gravity{ 0.0f, 9.8f }; // fps 60: grav = 9.8f
	b2World m_world{ m_gravity };

	float m_timeStep{ 1.0f / fps };
	int32_t m_velocityIterations{ 2000 };
	int32_t m_positionIterations{ 2000 };

	ConvexShape m_groundConvexShape;
	ConvexShape m_leftWallConvexShape;
	ConvexShape m_rightWallConvexShape;
	ConvexShape m_roofConvexShape;

	GameState m_gameState = GameState::EDIT;

	// shot variables
	const Uint8* state = SDL_GetKeyboardState(nullptr);
	float m_power{ 400.0f };
	float m_powerGain{ 2.0f };

	const float m_MAX_POWER{ 800.0f };
	const float m_MIN_POWER{ 200.0f };

	const int m_TOTAL_BULLETS{ 3 };
	int m_bulletsCount{ m_TOTAL_BULLETS };

	SDL_FPoint m_aimTargetPoint{};
	/*float m_shotAngle;
	Vector2f m_shotDirection{ 0.0f, 0.0f };*/

	Circle m_circle;

	bool m_playSim{ false };
	bool m_targetPresent{ false };

	// messing around with box2d
	int x, y;
	bool m_sprayTime{ false };
	Timer m_timer{};
	float m_sprayCooldown{ 0.05f };
	std::vector<ConvexShape> m_shapeSpawner{};
	std::vector<ShapeData> m_shapeData{};
	ConvexShape* m_currentBullet{ nullptr };


	ConvexShape m_rectanglePrefab;
	ConvexShape m_squarePrefab;
	ConvexShape m_targetPrefab;
	ConvexShape m_playerPrefab;

	ConvexShape* m_currentShape{};
	ConvexShape* m_player{};
	bool m_playerPresent{ false };
	bool m_estimationMode{ false };

	ConvexShapeContactListener m_contactListener;

	SDLDraw m_debugDraw;

	// UI
	SDL_FRect m_toolbarBg{ 0.0f, SCREEN_HEIGHT - 70.0f, SCREEN_WIDTH, 70.0f };
	UI::Button m_squareButton{ Vector2f{m_toolbarBg.x + 20.0f, m_toolbarBg.y + 10.0f} };
	UI::Button m_rectButton{ Vector2f{m_toolbarBg.x + 120.0f, m_toolbarBg.y + 10.0f} };
	UI::Button m_targetButton{ Vector2f{m_toolbarBg.x + 220.0f, m_toolbarBg.y + 10.0f} };
	UI::Button m_playerButton{ Vector2f{m_toolbarBg.x + 320.0f, m_toolbarBg.y + 10.0f} };

	UI::Button* m_selectedButton{ nullptr };

	SDL_FRect m_rectangleShapeSelect{ m_rectButton.position().x + 15.0f,
										m_rectButton.position().y + 10.0f,
										10, 20 };

	SDL_FRect m_squareShapeSelect{ m_squareButton.position().x + 15.0f,
									m_squareButton.position().y + 15.0f,
										10, 10 };

	SDL_FRect m_targetSelect{ m_targetButton.position().x + 15.0f,
									m_targetButton.position().y + 15.0f,
										10, 10 };
	
	SDL_FRect m_playerSelect{ m_playerButton.position().x + 15.0f,
									m_playerButton.position().y + 15.0f,
										10, 10 };
};