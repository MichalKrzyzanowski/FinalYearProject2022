#ifndef GAME_H
#define GAME_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <iostream>
#include <string>
#include <algorithm>
#include <box2d.h>
#include <fstream>
#include <filesystem>
#include <Windows.h>
#include "../SDLBox2DStuff/ConvexShape.h"
#include "../SDLBox2DStuff/Utility.h"
#include "Timer.h"
#include "../SDLBox2DStuff/ConvexShapeContactListener.h"
#include "../SDLBox2DStuff/SDLDraw.h"

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

enum class GameState
{
	ESTIMATION,
	DONE
};

class Game
{
public:
	Game(int mainArgsCount, char** mainArgs);
	~Game();
	void run();

private:

	void processEvents(SDL_Event e);
	void update();
	void render();

	void loadLevelData(const std::string& fileName);

	void storeShapeData(ShapeData shapeData);
	void reset();
	void updateShapeData(std::vector<ShapeData>& dataVec);

	void estimateDifficulty();
	void quit();
	int calculateDistanceScore();
	int distanceScoreEvaluation(int shortestDistance);
	int evaluateDifficulty(int bestScore);

	void cleanUp();
	void shoot(Vector2f targetPosition);

	bool m_gameIsRunning;
	bool m_shootMode{ false };

	std::string m_levelName{ "sim-level" };

	SDL_Window* m_window;
	SDL_Renderer* m_renderer;
	TTF_Font* m_fontNormal;
	TTF_Font* m_fontSmall;
	TTF_Font* m_fontTiny;

	// text objects
	LTexture m_phaseText;
	LTexture m_powerText;
	LTexture m_bulletCountText;

	// box2d setup
	float fps{ 60.0f };

	float m_simSpeed{ 1.0f };

	Timer m_skipStepTimer{};
	Timer m_quitTimer{};

	float m_skipTimerGoal{ 1.0f };

	b2Vec2 m_gravity{ 0.0f, 9.8f }; // fps 60: grav = 9.8f
	b2World m_world{ m_gravity };

	float m_timeStep{ 1.0f / fps };
	int32_t m_velocityIterations{ 2000 };
	int32_t m_positionIterations{ 2000 };

	ConvexShape m_groundShape;
	ConvexShape m_leftWallShape;
	ConvexShape m_rightWallShape;
	ConvexShape m_roofShape;

	GameState m_state{ GameState::ESTIMATION };

	// shot variables
	float m_power{ 400.0f };
	float m_powerGain{ 2.0f };

	const float m_MAX_POWER{ 800.0f };
	const float m_MIN_POWER{ 200.0f };

	const int m_TOTAL_BULLETS{ 3 };
	int m_bulletsCount{ m_TOTAL_BULLETS };

	SDL_FPoint m_aimTargetPoint{};

	int m_targetCount{ 0 };

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

	ConvexShapeContactListener m_contactListener;

	SDLDraw m_sdlDraw;
};

#endif // !GAME_H