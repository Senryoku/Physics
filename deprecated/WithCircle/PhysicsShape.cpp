#include "PhysicsShape.h"

namespace Physics
{

Shape::Shape() : myFriction(1.f), myDetectionMask(PHYSICS_ALL), myReactionMask(PHYSICS_ALL)
{
	//ctor
}

Shape::~Shape()
{
	//dtor
}

} // Namespace Physics
