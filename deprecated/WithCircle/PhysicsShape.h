#ifndef _PHYSICSSHAPE_H_
#define _PHYSICSSHAPE_H_

#include <list>
#include "Vec2.h"

#define PHYSICS_NONE	0x0000
#define PHYSICS_LAYER1	0x0001
#define PHYSICS_LAYER2	0x0002
#define PHYSICS_LAYER3	0x0004
#define PHYSICS_LAYER4	0x0008
#define PHYSICS_LAYER5	0x0010
#define PHYSICS_LAYER6	0x0020
#define PHYSICS_LAYER7	0x0040
#define PHYSICS_LAYER8	0x0080
#define PHYSICS_LAYER9	0x0100
#define PHYSICS_LAYER10	0x0200
#define PHYSICS_LAYER11	0x0400
#define PHYSICS_LAYER12	0x0800
#define PHYSICS_LAYER13	0x1000
#define PHYSICS_LAYER14	0x2000
#define PHYSICS_LAYER15	0x4000
#define PHYSICS_LAYER16	0x8000
#define PHYSICS_ALL		0xFFFF

namespace Physics
{

class Vertex;
class Circle;
class Rigid;
class Polygon;

class CollisionInfo {
	public:
		enum Type {
			None,
			PolygonPolygon,
			PolygonCircle,
			CirclePolygon,
			CircleCicle
		};
		Type		T;
		Polygon*	P1;
		Polygon*	P2;
		Circle*		C1;
		Circle*		C2;
		float		Depth;
		Vec2		Normal;
		Rigid*		Edge;
		Vertex*		V;

		CollisionInfo() : T(None), P1(0), P2(0), C1(0), C2(0), Depth(0.f), Normal(0,0), Edge(0), V(0) { }
};

class Shape
{
	public:
		Shape();
		virtual ~Shape();

		static void allDetectAndHandleCollision();

		inline float getFriction() { return myFriction; }
		inline unsigned int getDetectionMask() { return myDetectionMask; }
		inline unsigned int getReactionMask() { return myReactionMask; }
		inline void setDetectionMask(unsigned int Mask) { myDetectionMask = Mask; }
		inline void setReactionMask(unsigned int Mask) { myReactionMask = Mask; }

		void addCI(CollisionInfo CI) { myCIs.push_back(CI); }
		void clearCIs() { myCIs.clear(); }

	protected:
		float myFriction;
		unsigned int myDetectionMask;
		unsigned int myReactionMask;

		std::list<CollisionInfo> myCIs;

	private:
};

} // Namespace Physics

#endif // _COLLIDINGOBJECT_H_
