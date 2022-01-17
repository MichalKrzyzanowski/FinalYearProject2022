#include <SDL.h>
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

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

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

	void storeShapeData(ShapeData* shapeData);
	void reset();
	void estimateDifficulty();
	void cleanUp();
	void shoot(Vector2f targetPosition);

	bool m_gameIsRunning;

	SDL_Window* m_window;
	SDL_Renderer* m_renderer;

	// box2d setup
	b2Vec2 m_gravity{ 0.0f, 9.8f };
	b2World m_world{ m_gravity };

	float m_timeStep{ 1.0f / 60.0f };
	int32_t m_velocityIterations{ 6 };
	int32_t m_positionIterations{ 2 };

	ConvexShape m_groundConvexShape;

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

	// UI
	SDL_FRect m_toolbarBg{ 0.0f, SCREEN_HEIGHT - 70.0f, SCREEN_WIDTH, 70.0f };
	UI::Button m_squareButton{ Vector2f{m_toolbarBg.x + 20.0f, m_toolbarBg.y + 10.0f} };
	UI::Button m_rectButton{ Vector2f{m_toolbarBg.x + 100.0f, m_toolbarBg.y + 10.0f} };
	UI::Button m_targetButton{ Vector2f{m_toolbarBg.x + 180.0f, m_toolbarBg.y + 10.0f} };
	UI::Button m_playerButton{ Vector2f{m_toolbarBg.x + 260.0f, m_toolbarBg.y + 10.0f} };

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