#include "World.hpp"

#include <iostream> // DEBUG
#include <algorithm>

#include <omp.h>

namespace Physics
{

Grid::Grid(unsigned int Width, unsigned int Height, float CellWidth, float CellHeight) :
	myWidth(Width), myHeight(Height), myCellWidth(CellWidth), myCellHeight(CellHeight)
{
	myGrid.resize(Height);
	for(std::vector<std::vector<Cell> >::iterator ite = myGrid.begin();
		ite != myGrid.end(); ite++)
		ite->resize(Width);
}

Grid::~Grid()
{
}

Coord Grid::getCellCoord(Vec2 Pos)
{
	Coord C;

	C.X = std::max(0u, std::min(myWidth - 1, static_cast<unsigned int>(Pos.x/myCellWidth)));
	C.Y = std::max(0u, std::min(myHeight - 1, static_cast<unsigned int>(Pos.y/myCellHeight)));
	// std::cout << C.X << " " << C.Y << std::endl; // DEBUG
	return C;
}

std::list<Polygon*> Grid::getList(BBox BB)
{
	return getList(getCellCoord(BB.TopLeft), getCellCoord(BB.BottomRight));
}

std::list<Polygon*> Grid::getList(Coord C1, Coord C2)
{
	std::list<Polygon*> Accumulator;
	std::list<Polygon*> Tmp;
	for(unsigned int j = C1.Y; j <= C2.Y; j++)
	{
		for(unsigned int i = C1.X; i <= C2.X; i++)
		{
			Tmp = myGrid[j][i]; // Copie
			Accumulator.splice(Accumulator.begin(), Tmp); // Transfert dans Accumulator
		}
	}
	return Accumulator;
}

void Grid::add(Polygon* P)
{
	add(P, P->getBBox());
}

void Grid::add(Polygon* P, BBox BB)
{
	add(P, getCellCoord(BB.TopLeft), getCellCoord(BB.BottomRight));
}

void Grid::add(Polygon* P, Coord C1, Coord C2)
{
	for(unsigned int j = C1.Y; j <= C2.Y; j++)
	{
		for(unsigned int i = C1.X; i <= C2.X; i++)
		{
			myGrid[j][i].push_back(P);
		}
	}
}

void Grid::rm(Polygon* P)
{
	for(unsigned int j = 0; j < myHeight; j++)
	{
		for(unsigned int i = 0; i < myWidth; i++)
		{
			myGrid[j][i].remove(P);
		}
	}
}

void Grid::rm(Polygon* P, BBox BB)
{
	rm(P, getCellCoord(BB.TopLeft), getCellCoord(BB.BottomRight));
}

void Grid::rm(Polygon* P, Coord C1, Coord C2)
{
	for(unsigned int j = C1.Y; j <= C2.Y; j++)
	{
		for(unsigned int i = C1.X; i <= C2.X; i++)
		{
			myGrid[j][i].remove(P);
		}
	}
}

void Grid::glDraw() const
{
	glColor4f(1.f, 1.f, 1.f, 0.2f);
	glBegin(GL_LINES);
	for(unsigned int i = 0; i < myWidth; i++)
	{
		glVertex2f((float) i*myCellWidth, 0.f);
		glVertex2f((float) i*myCellWidth, myHeight*myCellHeight);
	}
	for(unsigned int i = 0; i < myHeight; i++)
	{
		glVertex2f(0.f, (float) i*myCellHeight);
		glVertex2f(myWidth*myCellWidth, (float) i*myCellHeight);
	}
	glEnd();
}

World::World(float Width, float Height) :
	myWidth(Width), myHeight(Height), myRigidIterations(1), myOverallIterations(4),
	myGrid(Width/WORLD_BASE_GRID_WIDTH + 1, Height/WORLD_BASE_GRID_WIDTH + 1)
{
}

World::~World()
{
	deleteAll();
}

void World::updateGrid()
{
	BBox Old, Actual;
	Coord Old_Top, Old_Bottom, Actual_Top, Actual_Bottom;
	for(auto p : myPolygons)
	{
		Old = p->getOldBBox();
		// Old.glDraw(); // DEBUG
		Old_Top = myGrid.getCellCoord(Old.TopLeft);
		Old_Bottom = myGrid.getCellCoord(Old.BottomRight);
		Actual = p->getBBox();
		// Actual.glDraw(); // DEBUG
		Actual_Top = myGrid.getCellCoord(Actual.TopLeft);
		Actual_Bottom = myGrid.getCellCoord(Actual.BottomRight);

		if(Old_Top != Actual_Top || Old_Bottom != Actual_Bottom)
		{
			//std::cout<<"grid size before rm :"<< myGrid.getCell(Old_Top).size()<<std::endl;
			myGrid.rm(p, Old);
			//std::cout<<"grid size after rm :"<< myGrid.getCell(Old_Top).size()<<std::endl;
			myGrid.add(p, Actual);
		}

		p->setOldBBox(Actual);
	}
}

void World::update(float prevdt, float dt)
{
	applyGlobalAcceleration();
	resolveElastics();
	resolveVertices(prevdt, dt);
	clearCIs();
	for(unsigned int i = 0; i < myOverallIterations; i++)
	{
		for(unsigned int j = 0; j < myRigidIterations; j++)
			resolveRigids();
		resolvePolygons();
	}
}

Vertex* World::getNearestVertex(Vec2 Pos)
{
	if (myVertices.empty())
		return NULL;

	auto it = myVertices.begin();
	Vertex* P = *it;
	it++;
	float tmp, dis = (P->getPosition() - Pos)*(P->getPosition() - Pos);

	while(it != myVertices.end())
	{
		tmp = ((*it)->getPosition() - Pos)*((*it)->getPosition() - Pos);
		if (dis > tmp)
		{
			dis = tmp;
			P = (*it);
		}
		it++;
	}
	return P;
}

void World::correctBBox(BBox& BB)
{
	BB.BottomRight.x = std::max(0.f, std::min(BB.BottomRight.x, myWidth));
	BB.BottomRight.y = std::max(0.f, std::min(BB.BottomRight.y, myHeight));
	BB.TopLeft.x = std::max(0.f, std::min(BB.TopLeft.x, myWidth));
	BB.TopLeft.y = std::max(0.f, std::min(BB.TopLeft.y, myHeight));
}

void World::add(Vertex* V)
{
	myVertices.push_back(V);
}

void World::add(Elastic* E)
{
	myElastics.push_back(E);
}

void World::add(Rigid* R)
{
	myRigids.push_back(R);
}

void World::add(Polygon* P)
{
	myPolygons.push_back(P);
	myGrid.add(P);
}

Vertex* World::newVertex(float x, float y, float Mass)
{
	Vertex* V = new Vertex(x, y, Mass);
	myVertices.push_back(V);
	return V;
}

Vertex* World::newVertex(Vec2 Vec, float Mass)
{
	Vertex* V = new Vertex(Vec, Mass);
	myVertices.push_back(V);
	return V;
}

Elastic* World::newElastic(Vertex* P1, Vertex* P2, float Length, float Spring)
{
	Elastic* E = new Elastic(P1, P2, Length, Spring);
	myElastics.push_back(E);
	return E;
}

Rigid* World::newRigid(Vertex* P1, Vertex* P2, float Length)
{
	Rigid* R = new Rigid(P1, P2, Length);
	myRigids.push_back(R);
	return R;
}

Polygon* World::newRectangle(float Width, float Height)
{
	if(Height < 0.f) Height = Width;
	std::vector<Vertex*> V;
	V.reserve(4);
	V.push_back(newVertex(0.f, 0.f));
	V.push_back(newVertex(Width, 0.f));
	V.push_back(newVertex(Width, Height));
	V.push_back(newVertex(0.f, Height));
	Polygon* P = new Polygon(V);
	add(P);
	return P;
}

void World::draw() const
{
	myGrid.glDraw();
	drawVertices();
	drawRigids();
	drawElastics();
	drawPolygons();
}

void World::drawVertices() const
{
	glPointSize(2);
	glBegin(GL_POINTS);
	for(auto v : myVertices)
		v->glDraws();
	glEnd();
	glPointSize(1);
}

void World::drawElastics() const
{
	glBegin(GL_LINES);
	for(auto e : myElastics)
		e->glDraws();
	glEnd();
}

void World::drawRigids() const
{
	glBegin(GL_LINES);
	glColor4f(0.f, 0.f, 1.f, 1.f);
	for(auto r : myRigids)
		r->glDraws();
	glEnd();
}

void World::drawPolygons() const
{
	for(auto p : myPolygons)
		p->glDraw();
}

void World::resolveVertices(float prevdt, float dt)
{
	#pragma omp parallel for
	for(size_t i = 0; i < myVertices.size(); ++i)
	{
		auto v = myVertices[i];
		v->resolve(prevdt, dt);

		// Replace le point dans les limites du monde
		if(v->getPosition().x > myWidth)
			v->setPosition(Vec2(myWidth, v->getPosition().y));
		if(v->getPosition().x < 0)
			v->setPosition(Vec2(0.f, v->getPosition().y));
		if(v->getPosition().y > myHeight)
			v->setPosition(Vec2(v->getPosition().x, myHeight));
		if(v->getPosition().y < 0)
			v->setPosition(Vec2(v->getPosition().x, 0.f));

        // Teste si une coordonnée vaut NaN
        if(v->getPosition().x != v->getPosition().x ||
			v->getPosition().y != v->getPosition().y)
        {
        	std::cout << "Nan Error !" << std::endl; // DEBUG
            v->setPosition(Vec2(0, 0), true);
        }
	}
}

void World::applyForceVertices(Vec2 Force)
{
	for(auto v : myVertices)
		v->applyForce(Force);
}

void World::addGlobalAcceleration(Vec2 Acc)
{
	myGlobalAcceleration += Acc;
}

void World::applyGlobalAcceleration()
{
	for(auto v : myVertices)
		v->addAcceleration(myGlobalAcceleration);
	myGlobalAcceleration = Vec2(0.f, 0.f);
}

void World::resolveElastics()
{
	#pragma omp parallel for
	for(size_t i = 0; i < myElastics.size(); ++i)
		myElastics[i]->resolve();
}

void World::resolveRigids()
{
	#pragma omp parallel for
	for(size_t i = 0; i < myRigids.size(); ++i)
		myRigids[i]->resolve();
}

void World::resolvePolygons()
{
	CollisionInfo Info;
	updateGrid();
	for(size_t i = 0; i < myPolygons.size(); ++i)
	{
		myPolygons[i]->resolveRigids();
		BBox BB = myPolygons[i]->getBBox();
		std::list<Polygon*> List = myGrid.getList(BB);
		for(auto p : List)
		{
			if(p != myPolygons[i])
			{
				Info = myPolygons[i]->collide(p);
				// Il y a collision
				if(Info.P1 != NULL)
				{
					// Sauvegarde des informations de collision
					if(Info.P1->getSaveCIs()) Info.P1->addCI(Info);
					if(Info.P2->getSaveCIs()) Info.P2->addCI(Info);

					// Ils ont un masque de réaction en commun
					if (Info.P1->getReactionMask() & Info.P2->getReactionMask())
					{
						// Réponse
						Vec2 CollisionVector = Info.Normal*(Info.Depth*0.5f);

						Vec2 PosE1 = Info.Edge->getP1()->getPosition();
						Vec2 PosE2 = Info.Edge->getP2()->getPosition();

						float PositionOnEdge; // Position du point sur la face
						// On évite les divisions par 0 !
						if(std::abs(PosE1.x - PosE2.x) > std::abs(PosE1.y - PosE2.y))
							PositionOnEdge = (Info.V->getPosition().x - CollisionVector.x
							- PosE1.x)/(PosE2.x - PosE1.x);
						else
							PositionOnEdge = (Info.V->getPosition().y - CollisionVector.y
							- PosE1.y)/(PosE2.y - PosE1.y);

						// Moins cher que de normaliser le vecteur de Edge
						Vec2 Tangent = Vec2(Info.Normal.y, -Info.Normal.x);
						Info.V->applyForce(Info.P1->getFriction()*Info.P2->getFriction()*Info.Depth*((Tangent*Info.V->getSpeed() < 0) ? 1 : -1)*Tangent);

						float CorrectionFactor = -1.0f/(PositionOnEdge*PositionOnEdge + (1 - PositionOnEdge)*(1 - PositionOnEdge));

						// Correction des positions
						Info.V->correctPosition(CollisionVector); // Du point
						// De  la face
						CollisionVector *= CorrectionFactor;
						Info.Edge->getP1()->correctPosition(CollisionVector*(1-PositionOnEdge));
						Info.Edge->getP2()->correctPosition(CollisionVector*PositionOnEdge);
					}
				}
			}
		}
    }
}

void World::clearCIs()
{
	for(auto p : myPolygons)
		p->clearCIs();
}

void World::deleteAll()
{
	deletePolygons();
	deleteRigids();
	deleteElastics();
	deleteVertices();
}

void World::deleteVertices()
{
	for(auto v : myVertices)
        delete v;
	myVertices.clear();
}

void World::deleteVertex(Vertex* V)
{
	myVertices.erase(std::remove(myVertices.begin(), myVertices.end(), V), myVertices.end());
	delete V;
}

void World::deleteRigids()
{
	for(auto r : myRigids)
		delete r;
	myRigids.clear();
}

void World::deleteRigid(Rigid* R)
{
	myRigids.erase(std::remove(myRigids.begin(), myRigids.end(), R), myRigids.end());
	delete R;
}

void World::deleteElastics()
{
	for(auto e : myElastics)
        delete e;
	myElastics.clear();
}

void World::deleteElastic(Elastic* E)
{
	myElastics.erase(std::remove(myElastics.begin(), myElastics.end(), E), myElastics.end());
	delete E;
}

void World::deletePolygons()
{
	for(auto p : myPolygons)
        delete p;
	myPolygons.clear();
}

void World::deletePolygon(Polygon* P)
{
	myPolygons.erase(std::remove(myPolygons.begin(), myPolygons.end(), P), myPolygons.end());
	delete P;
}

}
