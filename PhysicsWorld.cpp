#include "PhysicsWorld.h"

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

World::World(float Width, float Height) :
	myWidth(Width), myHeight(Height), myOverallIterations(4), myRigidIterations(1)
{
}

World::~World()
{
	deleteAll();
}

void World::update(float prevdt, float dt)
{
	applyGlobalAcceleration();
	resolveVertices(prevdt, dt);
	resolveElastics();
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
}

void World::add(Rectangle* R)
{
	myPolygons.push_back(R);
	R->addVerticesToWorld(this);
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

void World::newPolygon()
{
}

void World::draw()
{
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
	glColor3f(0.f, 0.f, 1.f);
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
            (*ite)->setPosition(Vec2(0, 0), 1);
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

void World::resolvePolygons(bool saveCI)
{
	CollisionInfo Info;
	for(std::list<Polygon*>::iterator ite = myPolygons.begin();
		ite != myPolygons.end(); ite++)
	{
		(*ite)->resolveRigids();
		for(std::list<Polygon*>::iterator ite2 = myPolygons.begin(); ite2 != myPolygons.end(); ite2++)
		{
			if( ite != ite2) {
				Info = (*ite)->collide((*ite2));
				// Il y a collision
				if(Info.P1 != NULL)
				{
					if(saveCI) // Sauvegarde des informations de collision
					{
						Info.P1->addCI(Info);
						Info.P2->addCI(Info);
					}
					// Ils ont un masque de réaction en commun
					if (Info.P1->getReactionMask() & Info.P2->getReactionMask())
					{
						// Réponse
						Vec2 CollisionVector = Info.Normal*Info.Depth;

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
						Info.V->correctPosition(CollisionVector*0.5f); // Du point
						// De  la face
						Info.Edge->getP1()->correctPosition(CollisionVector*
							CorrectionFactor*(1-PositionOnEdge)*0.5f);
						Info.Edge->getP2()->correctPosition(CollisionVector*
							CorrectionFactor*(PositionOnEdge)*0.5f);
					}
				}
			}
		}
    }
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

void World::deleteRigids()
{
	for(std::list<Rigid*>::iterator ite = myRigids.begin();
		ite != myRigids.end(); ite++)
        delete *ite;
	myRigids.clear();
}

void World::deleteElastics()
{
	for(std::list<Elastic*>::iterator ite = myElastics.begin();
		ite != myElastics.end(); ite++)
        delete *ite;
	myElastics.clear();
}

void World::deletePolygons()
{
	for(std::list<Polygon*>::iterator ite = myPolygons.begin();
		ite != myPolygons.end(); ite++)
        delete *ite;
	myPolygons.clear();
}

}
