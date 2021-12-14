#ifndef PLAYER_H
#define PLAYER_H

#include "ConvexShape.h"

class Player : public ConvexShape
{
private:

public:
	Player(b2World* world, Vector2f topLeftPosition);
};

#endif // !PLAYER_H

