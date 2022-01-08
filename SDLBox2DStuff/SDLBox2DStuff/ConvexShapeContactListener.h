#ifndef CONVEX_SHAPE_CONTACT_LISTENER
#define CONVEX_SHAPE_CONTACT_LISTENER

#include <box2d.h>
#include "ConvexShape.h"

class ConvexShapeContactListener : public b2ContactListener
{
public:
	void BeginContact(b2Contact* contact)
	{
		ConvexShape* first = reinterpret_cast<ConvexShape*>(contact->GetFixtureA()->GetBody()->GetUserData().pointer);
		ConvexShape* other = reinterpret_cast<ConvexShape*>(contact->GetFixtureB()->GetBody()->GetUserData().pointer);


		if (first->type() == Type::BULLET && other->type() == Type::TARGET ||
			other->type() == Type::BULLET && first->type() == Type::TARGET)
		{
			printf("Coll\n");

			if (first->type() == Type::TARGET)
			{
				first->marked() = true;
			}

			else if (other->type() == Type::TARGET)
			{
				other->marked() = true;
			}
		}

	}
};

#endif // !CONVEX_SHAPE_CONTACT_LISTENER