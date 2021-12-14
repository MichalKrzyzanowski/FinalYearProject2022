#ifndef TARGET_H
#define TARGET_H

#include "ConvexShape.h"

class Target : public ConvexShape
{
private:

public:
	Target(b2World* world, Vector2f topLeftPosition);
};

#endif // !TARGET_H

