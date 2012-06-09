#include "PhysicsCircle.h"

#include "PhysicsPolygon.h"

namespace Physics
{

Circle::Circle() : myCenter(new Vertex(0.f, 0.f)), myRadius(1.f)
{
	//ctor
}

Circle::Circle(Vec2 C) : myCenter(new Vertex(C.x, C.y)), myRadius(1.f)
{
	//ctor
}

Circle::Circle(Vec2 C, float R) : myCenter(new Vertex(C.x, C.y)), myRadius(R)
{
	//ctor
}

Circle::~Circle()
{
	delete myCenter;
}

void Circle::projectToAxis(float &Min, float &Max, const Vec2 Axis)
{
	float ProjCenter = myCenter->getPosition()*Axis;
	Min = ProjCenter - myRadius;
	Max = ProjCenter + myRadius;
}

CollisionInfo Circle::collide(Circle &C)
{
	CollisionInfo Info;
	Vec2 Normal = myCenter->getPosition() - C.getCenter()->getPosition();
	if(Normal.getSquaredLength() < (myRadius + C.getRadius())*(myRadius + C.getRadius()))
	{
		Info.C1 = this;
		Info.C2 = &C;
		float NL = Normal.getLength();
		Info.Depth = myRadius + C.getRadius() - NL;
		Info.Normal = Normal/NL;
	}
	return Info;
}

} // Namespace Physics
