#include "Player.h"

Player::Player(b2World* world, Vector2f topLeftPosition) :
	ConvexShape(world, topLeftPosition, 20, 20, b2_staticBody, SDL_Color{ 0x24, 0x3C, 0xAE, 0xFF })
{
	type() = Type::PLAYER;
}