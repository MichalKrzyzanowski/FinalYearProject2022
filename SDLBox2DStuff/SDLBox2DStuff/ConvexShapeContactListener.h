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
				first->active() = false;
			}

			else if (other->type() == Type::TARGET)
			{
				other->marked() = true;
				other->active() = false;
			}
		}
	}

	void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
	{
		ConvexShape* first = reinterpret_cast<ConvexShape*>(contact->GetFixtureA()->GetBody()->GetUserData().pointer);
		ConvexShape* other = reinterpret_cast<ConvexShape*>(contact->GetFixtureB()->GetBody()->GetUserData().pointer);


		if (first->type() == Type::BLOCK && other->type() == Type::TARGET ||
			other->type() == Type::BLOCK && first->type() == Type::TARGET)
		{
			float force = impulse->normalImpulses[0] + impulse->normalImpulses[1];

			if (force < 0.0f)
			{
				printf("Coll with block\n");
				if (first->type() == Type::TARGET)
				{
					first->marked() = true;
					first->active() = false;
				}

				else if (other->type() == Type::TARGET)
				{
					other->marked() = true;
					other->active() = false;
				}
			}
		}
	}
};

#endif // !CONVEX_SHAPE_CONTACT_LISTENER