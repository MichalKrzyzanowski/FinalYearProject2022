#include <SDL.h>
#include <iostream>
#include <string>
#include <box2d.h>
#include "Rect.h"
#include "ConvexShape.h"
#include "Utility.h"
#include "ConvexShape.h"

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

class Game
{
public:
    Game();
    ~Game();
    void run();

    void rotatePoint(float cx, float cy, float angle, SDL_FPoint& point);

private:

    void processEvents(SDL_Event e);
    void update();
    void render();

    void cleanUp();

    bool m_gameIsRunning;

    SDL_Window* m_window;
    SDL_Renderer* m_renderer;

    // box2d setup
    b2Vec2 m_gravity{ 4.0f, 9.8f };
    b2World m_world{ m_gravity };

    // ground
    //b2BodyDef m_groundBodDef;
    //b2Body* m_groundBody;
    //b2PolygonShape m_groundBox;

    //// dynamic shape
    //b2BodyDef m_DynamicBodDef;
    //b2Body* m_dynamicBoxBody;
    //b2PolygonShape m_dynamicBox;
    //b2FixtureDef m_boxFix;

    //SDL_FRect m_groundRect;
    //SDL_FRect m_boxRect;

    float m_timeStep{ 1.0f / 60.0f };
    int32_t m_velocityIterations{ 6 };
    int32_t m_positionIterations{ 2 };

    /*SDL_FPoint pointsG[4];
    Rect m_rect;
    Rect m_groundRect;*/

    ConvexShape m_groundConvexShape;
    ConvexShape m_dynamicConvexShape;
};