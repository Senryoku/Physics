#ifndef _PHYSICSWORLD_H_
#define _PHYSICSWORLD_H_

#include "PhysicsVertex.h"
#include "PhysicsConstraint.h"
#include "PhysicsRigid.h"
#include "PhysicsElastic.h"
#include "PhysicsPolygon.h"
#include "PhysicsRectangle.h"

namespace Physics
{

typedef std::list<Polygon*> Cell;

class Grid
{
	public:
		Grid(unsigned int Width, unsigned int Height, float CellWidth = 192.f, float CellHeight = 192.f);
		~Grid();

	private:
		std::vector<std::vector<Cell> > myGrid;
		unsigned int myWidth;
		unsigned int myHeight;
		float myCellWidth;
		float myCellHeight;
};

class World
{
	public:
		World(float Width = 800.f, float Height = 600.f);
		~World();

		void update(float prevdt, float dt);

		unsigned int getVertexCount() { return myVertices.size(); }
		unsigned int getElasticCount() { return myElastics.size(); }
		unsigned int getRigidCount() { return myRigids.size(); }
		unsigned int getPolygonCount() { return myPolygons.size(); }

		Vertex* getNearestVertex(Vec2 Pos);

		void add(Vertex* V);
		void add(Elastic* E);
		void add(Rigid* R);
		void add(Polygon* P);
		void add(Rectangle* R);

		Vertex* newVertex(float x = 0.f, float y = 0.f, float Mass = 1.f);
		Vertex* newVertex(Vec2 Vec, float Mass = 1.f);
		Elastic* newElastic(Vertex* P1, Vertex* P2, float Length = -1.f, float Spring = 1.f);
		Rigid* newRigid(Vertex* P1, Vertex* P2, float Length = -1);
		void newPolygon();

		void draw();
		void drawVertices();
		void drawElastics();
		void drawRigids();
		void drawPolygons();

		void applyForceVertices(Vec2 Force);
		void addGlobalAcceleration(Vec2 Acc);
		void applyGlobalAcceleration();

		void resolveVertices(float prevdt = 0.1f, float dt = 0.1f);
		void resolveElastics();
		void resolveRigids();
		void resolvePolygons(bool saveCI = false);

		void deleteAll();
		void deleteVertices();
		void deleteRigids();
		void deleteElastics();
		void deletePolygons();

	protected:
		float myWidth;
		float myHeight;

		unsigned int myRigidIterations;
		unsigned int myOverallIterations;

		Vec2 myGlobalAcceleration;

		std::list<Vertex*> myVertices;
		std::list<Elastic*> myElastics;
		std::list<Rigid*> myRigids;
		std::list<Polygon*> myPolygons;

	private:
};

}

#endif // _PHYSICSWORLD_H_
