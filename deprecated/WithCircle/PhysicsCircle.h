#ifndef _PHYSICSCIRCLE_H_
#define _PHYSICSCIRCLE_H_

#include "Vec2.h"
#include "PhysicsVertex.h"
#include "PhysicsShape.h"

namespace Physics
{
class Polygon;

class Circle : public Shape
{
	public:
		Circle();
		Circle(Vec2 C);
		Circle(Vec2 C, float R);
		~Circle();

		inline Vertex* getCenter() { return myCenter; }
		inline float getRadius() { return myRadius; }

		CollisionInfo collide(Circle &C);
		CollisionInfo collide(Polygon &P);

		void projectToAxis(float &Min, float &Max, const Vec2 Axis);
	protected:
		Vertex* myCenter;
		float myRadius;
	private:
};

} // Namespace Physics

#endif // _PHYSICSCIRCLE_H_
