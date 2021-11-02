#include <SDL.h>
#include <iostream>
#include <string>
#include <box2d.h>
#include "Rect.h"
#include "ConvexShape.h"
#include "Utility.h"
#include "ConvexShape.h"
#include "Timer.h"

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

    void cleanUp();

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
    
    // messing around with box2d
    int x, y;
    bool m_sprayTime{ false };
    Timer m_timer{};
    float m_sprayCooldown{ 0.05f };
    std::vector<ConvexShape> m_shapeSpawner{};
};