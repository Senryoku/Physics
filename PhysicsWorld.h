#ifndef _PHYSICSWORLD_H_
#define _PHYSICSWORLD_H_

#include "PhysicsVertex.h"
#include "PhysicsRigid.h"
#include "PhysicsPolygon.h"

class World
{
	public:
		World();
		~World();
	protected:
		float Width;
		float Height;
	private:
};

#endif // _PHYSICSWORLD_H_
