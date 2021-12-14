#include "Player.h"

Player::Player(b2World* world, Vector2f topLeftPosition) :
	ConvexShape(world, topLeftPosition, 20, 20, b2_staticBody)
{
	type() = Type::PLAYER;
}