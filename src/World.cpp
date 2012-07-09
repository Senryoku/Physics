#include "World.hpp"

#include <iostream> // DEBUG

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

void Grid::glDraw()
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
	for(std::list<Polygon*>::iterator ite = myPolygons.begin();
		ite != myPolygons.end(); ite++)
	{
		Old = (*ite)->getOldBBox();
		Old.glDraw(); // DEBUG
		Old_Top = myGrid.getCellCoord(Old.TopLeft);
		Old_Bottom = myGrid.getCellCoord(Old.BottomRight);
		Actual = (*ite)->getBBox();
		Actual.glDraw(); // DEBUG
		Actual_Top = myGrid.getCellCoord(Actual.TopLeft);
		Actual_Bottom = myGrid.getCellCoord(Actual.BottomRight);

		if(Old_Top != Actual_Top || Old_Bottom != Actual_Bottom)
		{
			//std::cout<<"grid size before rm :"<< myGrid.getCell(Old_Top).size()<<std::endl;
			myGrid.rm(*ite, Old);
			//std::cout<<"grid size after rm :"<< myGrid.getCell(Old_Top).size()<<std::endl;
			myGrid.add(*ite, Actual);
		}

		(*ite)->setOldBBox(Actual);
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
		for(unsigned int j = 0; j < myRigidIterations; j++) resolveRigids();
		resolvePolygons();
	}
}

Vertex* World::getNearestVertex(Vec2 Pos)
{
	if (myVertices.empty())
		return NULL;

	std::list<Vertex*>::iterator it = myVertices.begin();
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

void World::draw()
{
	myGrid.glDraw();
	drawVertices();
	drawRigids();
	drawElastics();
	drawPolygons();
}

void World::drawVertices()
{
	glPointSize(2);
	glBegin(GL_POINTS);
	for (std::list<Vertex*>::iterator ite = myVertices.begin();
		ite != myVertices.end(); ite++)
		(*ite)->glDraws();
	glEnd();
	glPointSize(1);
}

void World::drawElastics()
{
	glBegin(GL_LINES);
	for(std::list<Elastic*>::iterator ite = myElastics.begin();
		ite != myElastics.end(); ite++)
		(*ite)->glDraws();
	glEnd();
}

void World::drawRigids()
{
	glBegin(GL_LINES);
	glColor4f(0.f, 0.f, 1.f, 1.f);
	for (std::list<Rigid*>::iterator ite = myRigids.begin();
		ite != myRigids.end(); ite++)
		(*ite)->glDraws();
	glEnd();
}

void World::drawPolygons()
{
	for(std::list<Polygon*>::iterator ite = myPolygons.begin();
		ite != myPolygons.end(); ite++)
		(*ite)->glDraw();
}

void World::resolveVertices(float prevdt, float dt)
{
	for(std::list<Vertex*>::iterator ite = myVertices.begin();
		ite != myVertices.end(); ite++)
	{
		(*ite)->resolve(prevdt, dt);

		// Replace le point dans les limites du monde
		if((*ite)->getPosition().x > myWidth)
			(*ite)->setPosition(Vec2(myWidth, (*ite)->getPosition().y));
		if((*ite)->getPosition().x < 0)
			(*ite)->setPosition(Vec2(0.f, (*ite)->getPosition().y));
		if((*ite)->getPosition().y > myHeight)
			(*ite)->setPosition(Vec2((*ite)->getPosition().x, myHeight));
		if((*ite)->getPosition().y < 0)
			(*ite)->setPosition(Vec2((*ite)->getPosition().x, 0.f));

        // Teste si une coordonnée vaut NaN
        if((*ite)->getPosition().x != (*ite)->getPosition().x ||
			(*ite)->getPosition().y != (*ite)->getPosition().y)
        {
        	std::cout << "Nan Error !" << std::endl; // DEBUG
            (*ite)->setPosition(Vec2(0, 0), true);
        }
	}
}

void World::applyForceVertices(Vec2 Force)
{
	for(std::list<Vertex*>::iterator ite = myVertices.begin();
		ite != myVertices.end(); ite++)
		(*ite)->applyForce(Force);
}

void World::addGlobalAcceleration(Vec2 Acc)
{
	myGlobalAcceleration += Acc;
}

void World::applyGlobalAcceleration()
{
	for(std::list<Vertex*>::iterator ite = myVertices.begin();
		ite != myVertices.end(); ite++)
		(*ite)->addAcceleration(myGlobalAcceleration);
	myGlobalAcceleration = Vec2(0.f, 0.f);
}

void World::resolveElastics()
{
	for(std::list<Elastic*>::iterator ite = myElastics.begin();
		ite != myElastics.end(); ite++)
		(*ite)->resolve();
}

void World::resolveRigids()
{
	for(std::list<Rigid*>::iterator ite = myRigids.begin();
		ite != myRigids.end(); ite++)
		(*ite)->resolve();
}

void World::resolvePolygons()
{
	CollisionInfo Info;
	updateGrid();
	for(std::list<Polygon*>::iterator ite = myPolygons.begin();
		ite != myPolygons.end(); ite++)
	{
		(*ite)->resolveRigids();
		BBox BB = (*ite)->getBBox();
		std::list<Polygon*> List = myGrid.getList(BB);
		for(std::list<Polygon*>::iterator ite2 = List.begin(); ite2 != List.end(); ite2++)
		{
			if( *ite != *ite2) {
				Info = (*ite)->collide((*ite2));
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
	for(std::list<Polygon*>::iterator ite = myPolygons.begin();
		ite != myPolygons.end(); ite++)
		(*ite)->clearCIs();
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
	for(std::list<Vertex*>::iterator ite = myVertices.begin();
		ite != myVertices.end(); ite++)
        delete *ite;
	myVertices.clear();
}

void World::deleteVertex(Vertex* V)
{
	myVertices.remove(V);
	delete V;
}

void World::deleteRigids()
{
	for(std::list<Rigid*>::iterator ite = myRigids.begin();
		ite != myRigids.end(); ite++)
        delete *ite;
	myRigids.clear();
}

void World::deleteRigid(Rigid* R)
{
	myRigids.remove(R);
	delete R;
}

void World::deleteElastics()
{
	for(std::list<Elastic*>::iterator ite = myElastics.begin();
		ite != myElastics.end(); ite++)
        delete *ite;
	myElastics.clear();
}

void World::deleteElastic(Elastic* E)
{
	myElastics.remove(E);
	delete E;
}

void World::deletePolygons()
{
	for(std::list<Polygon*>::iterator ite = myPolygons.begin();
		ite != myPolygons.end(); ite++)
        delete *ite;
	myPolygons.clear();
}

void World::deletePolygon(Polygon* P)
{
	myPolygons.remove(P);
	delete P;
}

}
