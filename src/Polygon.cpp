#include "Polygon.hpp"

namespace Physics
{

void BBox::glDraw()
{
	glBegin(GL_LINE_LOOP);
	glColor4f(1.f, 1.f, 0.f, 0.5f);
	glVertex2f(TopLeft.x, TopLeft.y);
	glVertex2f(BottomRight.x, TopLeft.y);
	glVertex2f(BottomRight.x, BottomRight.y);
	glVertex2f(TopLeft.x, BottomRight.y);
	glEnd();
}

Polygon::Polygon() :
	myFriction(1.f), myDetectionMask(PHYSICS_ALL), myReactionMask(PHYSICS_ALL), mySaveCIs(false), myBBValid(false)
{
	setOldBBox();
}

Polygon::Polygon(std::vector<Vertex*> VecVertex) :
	myVertices(VecVertex), myFriction(1.f), myDetectionMask(PHYSICS_ALL), myReactionMask(PHYSICS_ALL), mySaveCIs(false), myBBValid(false)
{
	createConstraints();
	setOldBBox();
}

void Polygon::createConstraints()
{
	destroyConstraints();

	unsigned int nb = static_cast<unsigned int>(myVertices.size());
	myEdges.reserve(nb);
	myInternalContraints.reserve((nb-2)*(nb-3)/2 + 1);

	for(unsigned int i = 0; i < nb; i++)
	{
		myEdges.push_back(new Rigid(myVertices[i], myVertices[(i+1)%nb]));
		for(unsigned int j = i + 2; j < nb - (i==0?1:0); j++)
			myInternalContraints.push_back(new Rigid(myVertices[i], myVertices[j]));
	}
}

void Polygon::destroyConstraints()
{
	for(std::vector<Rigid*>::iterator ite = myEdges.begin();
		ite != myEdges.end(); ite++)
		delete *ite;
	myEdges.clear();
	for(std::vector<Rigid*>::iterator ite = myInternalContraints.begin();
		ite != myInternalContraints.end(); ite++)
		delete *ite;
	myInternalContraints.clear();
}

Polygon::~Polygon()
{
	destroyConstraints();
}

void Polygon::addCI(CollisionInfo CI)
{
	myCIs.push_back(CI);
}

void Polygon::setFixed(bool B)
{
	for(unsigned int i = 1; i < myVertices.size(); i++)
		myVertices[i]->setFixed(B);
	myFixed = B;
}

Vec2 Polygon::getCenter()
{
	Vec2 Center(myVertices[0]->getPosition());
	for(unsigned int i = 1; i < myVertices.size(); i++)
	{
		Center += myVertices[i]->getPosition();
	}
	return Center/myVertices.size();
}

Vec2 Polygon::getMassCenter()
{
	float Mass = myVertices[0]->getMass();
	Vec2 Center(myVertices[0]->getPosition());
	for(unsigned int i = 1; i < myVertices.size(); i++)
	{
		Center = (myVertices[i]->getPosition()*myVertices[i]->getMass()
					+ Center*Mass)/(Mass + myVertices[i]->getMass());
		Mass += myVertices[i]->getMass();
	}
	return Center;
}

void Polygon::setOldBBox()
{
	myOldBoundingBox = getBBox();
	myBBValid = false;
}

void Polygon::setOldBBox(BBox OBB)
{
	myOldBoundingBox = OBB;
	myBBValid = false;
}

BBox Polygon::getBBox()
{
	if(!myBBValid)
	{
		Vec2 Pos = myVertices[0]->getPosition();
		myBoundingBox.TopLeft = Pos;
		myBoundingBox.BottomRight = Pos;
		for(unsigned int i = 1; i < myVertices.size(); i++)
		{
			Pos = myVertices[i]->getPosition();
			myBoundingBox.TopLeft.x = std::min(myBoundingBox.TopLeft.x, Pos.x);
			myBoundingBox.TopLeft.y = std::min(myBoundingBox.TopLeft.y, Pos.y);
			myBoundingBox.BottomRight.x = std::max(myBoundingBox.BottomRight.x, Pos.x);
			myBoundingBox.BottomRight.y = std::max(myBoundingBox.BottomRight.y, Pos.y);
		}
	}
	return myBoundingBox;
}

BBox Polygon::getOldBBox()
{
	return myOldBoundingBox;
}

void Polygon::resolveRigids()
{
	for(std::vector<Rigid*>::iterator ite = myEdges.begin();
		ite != myEdges.end(); ite++)
		(*ite)->resolve();
	for(std::vector<Rigid*>::iterator ite = myInternalContraints.begin();
		ite != myInternalContraints.end(); ite++)
		(*ite)->resolve();
}

CollisionInfo Polygon::collide(Polygon *P)
{
	Vec2 Axis;
	Rigid* Edge;
	CollisionInfo Info;
	// Filtre de détection
	if(!(myDetectionMask & P->getDetectionMask())) return Info;
	Info.P1 = this;
	Info.P2 = P;
	Info.Depth = INFINITY; // Pour le minimum
	float Min, Max, MinP, MaxP, Gap; // Valeur des projections, distance

	for(unsigned int i = 0; i < myEdges.size() + P->myEdges.size(); i++)
	{
		if(i < myEdges.size())
			Edge = myEdges[i];
		else Edge = P->myEdges[i - myEdges.size()], Info.P1 = P, Info.P2 = this;
		// P1 est toujours le polygone dont on teste la face

		// Si la face est "nulle", on n'essaye pas de la tester !
		if(Edge->getVector() == Vec2(0.f, 0.f)) continue;

		Axis = Edge->getVector().getOrthogonal();
		Axis.normalize();

		ProjectToAxis(Min, Max, Axis);
		P->ProjectToAxis(MinP, MaxP, Axis);

		if(Min < MinP)
			Gap = MinP - Max;
		else Gap = Min - MaxP;

		if (Gap >= 0) return CollisionInfo(); // Pas de collision

		if(std::abs(Gap) < Info.Depth)
			Info.Depth = std::abs(Gap),
			Info.Normal = Axis,
			Info.Edge = Edge;
	}

	Vec2 P1Center = Info.P1->getCenter();
	// On s'assure que la normal est dans le bon sens (pointe vers P2)
	if(Info.Normal*(Info.P2->getCenter() - P1Center) < 0)
		Info.Normal *= -1;

	// Recherche du point de collision (= le plus proche de P1)
	float distP1Vertex = INFINITY; // On recherche un minimum
	float tmpDist;
	for(unsigned int i = 0; i < Info.P2->myVertices.size(); i++)
	{
		tmpDist = Info.Normal*(Info.P2->myVertices[i]->getPosition()-P1Center);
		if(tmpDist < distP1Vertex)
			distP1Vertex = tmpDist,
			Info.V = Info.P2->myVertices[i];
	}

	// Gère le cas où le polygone se résume à un point (Tout ses points ont les mêmes coordonnées
	if(Info.Edge == NULL)
		return CollisionInfo();
	else
		return Info;
}

void Polygon::ProjectToAxis(float &Min, float &Max, const Vec2& Axis)
{
	Min = Max = myVertices[0]->getPosition()*Axis;

	for(unsigned int i = 1; i < myVertices.size(); i++)
	{
		float Tmp = myVertices[i]->getPosition()*Axis;
		if(Tmp < Min) Min = Tmp;
		else if(Tmp > Max) Max = Tmp;
	}
}

Rigid& Polygon::operator[](const unsigned int i)
{
	return *myEdges[i];
}

void Polygon::applyForce(Vec2 V)
{
	for(unsigned int i = 0; i < myVertices.size(); i++)
	{
		myVertices[i]->applyForce(V);
	}
}

void Polygon::glDraw()
{
	glColor4f(0.f, 0.f, 1.f, 0.8f);
	glBegin(GL_LINES);
	for(std::vector<Rigid*>::iterator ite = myEdges.begin();
		ite != myEdges.end(); ite++)
		(*ite)->glDraws();
	for(std::vector<Rigid*>::iterator ite = myInternalContraints.begin();
		ite != myInternalContraints.end(); ite++)
		(*ite)->glDraws();
	glEnd();

	glColor4f(1.f, 1.f, 1.f, 0.2f);
	glBegin(GL_POLYGON);
	for(std::vector<Vertex*>::iterator ite = myVertices.begin();
		ite != myVertices.end(); ++ite)
	{
		Vec2 Pos = (*ite)->getPosition();
		glVertex2f(Pos.x, Pos.y);
	}
	glEnd();
}

}
