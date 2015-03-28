#ifndef _PHYSICSWORLD_H_
#define _PHYSICSWORLD_H_

#include <iostream> // DEBUG

#include "Vertex.hpp"
#include "Constraint.hpp"
#include "Rigid.hpp"
#include "Elastic.hpp"
#include "Polygon.hpp"

#define WORLD_BASE_GRID_WIDTH 16.f

namespace Physics
{

typedef std::list<Polygon*> Cell;

class Coord {
	public:
		unsigned int X;
		unsigned int Y;

		inline bool operator!=(Coord C) { return X != C.X || Y != C.Y; }
};

class Grid
{
	public:
		Grid(unsigned int Width, unsigned int Height, float CellWidth = WORLD_BASE_GRID_WIDTH, float CellHeight = WORLD_BASE_GRID_WIDTH);
		~Grid();

		Coord getCellCoord(Vec2 Pos);

		inline unsigned int getWidth() { return myWidth; }
		inline unsigned int getHeight() { return myHeight; }
		inline unsigned int getCellWidth() { return myCellWidth; }
		inline unsigned int getCellHeight() { return myCellHeight; }
		inline unsigned int getCellSize(unsigned int i, unsigned int j) { return myGrid[i][j].size(); }

		std::vector<Cell>& operator[](unsigned int X) { return myGrid[X]; }
		std::vector<Cell>& getLine(unsigned int X) { return myGrid[X]; }
		Cell& operator[](Coord C) { return myGrid[C.X][C.Y]; }
		Cell& getCell(unsigned int X, unsigned int Y) { return myGrid[X][Y]; }
		Cell& getCell(Coord C) { return myGrid[C.X][C.Y]; }

		inline std::list<Polygon*> getList(BBox BB);
		std::list<Polygon*> getList(Coord C1, Coord C2);

		inline void add(Polygon* P);
		inline void add(Polygon* P, BBox BB);
		void add(Polygon* P, Coord C1, Coord C2);

		void rm(Polygon* P);
		inline void rm(Polygon* P, BBox BB);
		void rm(Polygon* P, Coord C1, Coord C2);

		void glDraw() const;

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

		inline Grid& getGrid() { return myGrid; }
		void updateGrid();
		void update(float prevdt, float dt);

		float getWidth() const { return myWidth; }
		float getHeight() const { return myHeight; }
		
		size_t getVertexCount() const { return myVertices.size(); }
		size_t getElasticCount() const { return myElastics.size(); }
		size_t getRigidCount() const { return myRigids.size(); }
		size_t getPolygonCount() const { return myPolygons.size(); }

		Vertex* getNearestVertex(Vec2 Pos);

		void correctBBox(BBox& BB);

		void add(Vertex* V);
		void add(Elastic* E);
		void add(Rigid* R);
		void add(Polygon* P);

		Vertex* newVertex(float x = 0.f, float y = 0.f, float Mass = 1.f);
		Vertex* newVertex(Vec2 Vec, float Mass = 1.f);
		Elastic* newElastic(Vertex* P1, Vertex* P2, float Length = -1.f, float Spring = 1.f);
		Rigid* newRigid(Vertex* P1, Vertex* P2, float Length = -1);
		Polygon* newRectangle(float Width = 100.f, float Height = -1.f);

		void draw() const;
		void drawVertices() const;
		void drawElastics() const;
		void drawRigids() const;
		void drawPolygons() const;

		void applyForceVertices(Vec2 Force);
		void addGlobalAcceleration(Vec2 Acc);
		void applyGlobalAcceleration();

		void resolveVertices(float prevdt = 0.1f, float dt = 0.1f);
		void resolveElastics();
		void resolveRigids();
		void resolvePolygons();

		void clearCIs();

		void deleteAll();
		void deleteVertices();
		void deleteVertex(Vertex* V);
		void deleteRigids();
		void deleteRigid(Rigid* R);
		void deleteElastics();
		void deleteElastic(Elastic* E);
		void deletePolygons();
		void deletePolygon(Polygon* P);

	protected:
		float myWidth;
		float myHeight;

		unsigned int myRigidIterations;
		unsigned int myOverallIterations;

		Vec2 myGlobalAcceleration;

		std::vector<Vertex*> myVertices;
		std::vector<Elastic*> myElastics;
		std::vector<Rigid*> myRigids;
		std::vector<Polygon*> myPolygons;

		Grid myGrid;

	private:
};

}

#endif // _PHYSICSWORLD_H_
