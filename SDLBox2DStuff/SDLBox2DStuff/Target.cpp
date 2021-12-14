#include "Target.h"

Target::Target(b2World* world, Vector2f topLeftPosition) :
	ConvexShape(world, topLeftPosition, 20, 20, b2_dynamicBody, SDL_Color{ 240, 207, 46, 255 })
{
	type() = Type::TARGET;
}