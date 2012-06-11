#include "PhysicsPolygon.h"

namespace Physics
{

Polygon::Polygon() :
	myFriction(1.f), myDetectionMask(PHYSICS_ALL), myReactionMask(PHYSICS_ALL)
{
}

Polygon::Polygon(int nb, unsigned int FLAGS, ...) :
	myFriction(1.f), myDetectionMask(PHYSICS_ALL), myReactionMask(PHYSICS_ALL)
{
	va_list ap;
	va_start(ap, FLAGS);

	Vertices.reserve(nb);
	Edges.reserve(nb);

	if(FLAGS==FLAG_NULL)
	{
		InternalContraints.reserve((nb-2)*(nb-3)/2 + 1);

		for(int i = 0; i < nb; i++)
			Vertices.push_back(va_arg(ap, Vertex*));
		for(int i = 0; i < nb; i++)
		{
			Edges.push_back(new Rigid(Vertices[i], Vertices[(i+1)%nb]));
            for(int j = i + 2; j < nb - (i==0?1:0); j++)
                InternalContraints.push_back(new Rigid(Vertices[i], Vertices[j]));
		}

	} else if(FLAGS & WITH_LENGTH) {
		std::vector<float> Lengths;
		Lengths.reserve(nb);
		Edges.reserve(nb);
		for(int i = 0; i < nb; i++)
			Vertices.push_back(va_arg(ap, Vertex*)),
			Lengths.push_back(static_cast<float>(va_arg(ap, double)));
		for(int i = 0; i < nb; i++)
			Edges.push_back(new Rigid(Vertices[i],
											Vertices[(i+1)%nb],
											Lengths[i]));
	}

	va_end(ap);
}

Polygon::~Polygon()
{
	for(std::vector<Rigid*>::iterator ite = Edges.begin();
		ite != Edges.end(); ite++)
		delete *ite;
	Edges.clear();
	for(std::vector<Rigid*>::iterator ite = InternalContraints.begin();
		ite != InternalContraints.end(); ite++)
		delete *ite;
	InternalContraints.clear();
}

void Polygon::addCI(CollisionInfo CI)
{
	myCIs.push_back(CI);
}

void Polygon::setFixed(bool B)
{
	for(unsigned int i = 1; i < Vertices.size(); i++)
		Vertices[i]->setFixed(B);
	myFixed = B;
}

Vec2 Polygon::getCenter()
{
	Vec2 Center(Vertices[0]->getPosition());
	for(unsigned int i = 1; i < Vertices.size(); i++)
	{
		Center += Vertices[i]->getPosition();
	}
	return Center/Vertices.size();
}

Vec2 Polygon::getMassCenter()
{
	float Mass = Vertices[0]->getMass();
	Vec2 Center(Vertices[0]->getPosition());
	for(unsigned int i = 1; i < Vertices.size(); i++)
	{
		Center = (Vertices[i]->getPosition()*Vertices[i]->getMass()
					+ Center*Mass)/(Mass + Vertices[i]->getMass());
		Mass += Vertices[i]->getMass();
	}
	return Center;
}

BBox Polygon::getBBox()
{
	BBox BB;
	Vec2 Pos = Vertices[0]->getPosition();
	BB.TopLeft = Pos;
	BB.BottomRight = Pos;
	for(unsigned int i = 1; i < Vertices.size(); i++)
	{
		Pos = Vertices[i]->getPosition();
		BB.TopLeft.x = std::min(BB.TopLeft.x, Pos.x);
		BB.TopLeft.y = std::min(BB.TopLeft.y, Pos.y);
		BB.BottomRight.x = std::max(BB.BottomRight.x, Pos.x);
		BB.BottomRight.y = std::max(BB.BottomRight.y, Pos.y);
	}
	return BB;
}

BBox Polygon::getOldBBox()
{
	BBox BB;
	Vec2 Pos = Vertices[0]->getOldPosition();
	BB.TopLeft = Pos;
	BB.BottomRight = Pos;
	for(unsigned int i = 1; i < Vertices.size(); i++)
	{
		Pos = Vertices[i]->getOldPosition();
		BB.TopLeft.x = std::min(BB.TopLeft.x, Pos.x);
		BB.TopLeft.y = std::min(BB.TopLeft.y, Pos.y);
		BB.BottomRight.x = std::max(BB.TopLeft.x, Pos.x);
		BB.BottomRight.y = std::max(BB.BottomRight.y, Pos.y);
	}
	return BB;
}

void Polygon::resolveRigids()
{
	for(std::vector<Rigid*>::iterator ite = Edges.begin();
		ite != Edges.end(); ite++)
		(*ite)->resolve();
	for(std::vector<Rigid*>::iterator ite = InternalContraints.begin();
		ite != InternalContraints.end(); ite++)
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

	for(unsigned int i = 0; i < Edges.size() + P->Edges.size(); i++)
	{
		if(i < Edges.size())
			Edge = Edges[i];
		else Edge = P->Edges[i - Edges.size()], Info.P1 = P, Info.P2 = this;
		// P1 est toujours le polygone dont on teste la face

		// Si la face est "nulle", on n'essaye pas de la tester !
		if(Edge->getVector() == Vec2(0.f, 0.f)) continue;

		Axis = Edge->getVector().getOrthogonal().getNormalized();

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
	for(unsigned int i = 0; i < Info.P2->Vertices.size(); i++)
	{
		tmpDist = Info.Normal*(Info.P2->Vertices[i]->getPosition()-P1Center);
		if(tmpDist < distP1Vertex)
			distP1Vertex = tmpDist,
			Info.V = Info.P2->Vertices[i];
	}

	// Gère le cas où le polygone se résume à un point (Tout ses points ont les mêmes coordonnées
	if(Info.Edge == NULL)
		return CollisionInfo();
	else
		return Info;
}

void Polygon::ProjectToAxis(float &Min, float &Max, const Vec2 Axis)
{
	Min = Max = Vertices[0]->getPosition()*Axis;

	for(unsigned int i = 1; i < Vertices.size(); i++)
	{
		float Tmp = Vertices[i]->getPosition()*Axis;
		Min = std::min(Tmp, Min);
		Max = std::max(Tmp, Max);
	}
}

Rigid& Polygon::operator[](const unsigned int i)
{
	return *Edges[i];
}

void Polygon::applyForce(Vec2 V)
{
	for(unsigned int i = 0; i < Vertices.size(); i++)
	{
		Vertices[i]->applyForce(V);
	}
}

void Polygon::glDraw()
{
	glBegin(GL_LINES);
	glColor3f(0.f, 0.f, 1.f);
	for(std::vector<Rigid*>::iterator ite = Edges.begin();
		ite != Edges.end(); ite++)
		(*ite)->glDraws();
	for(std::vector<Rigid*>::iterator ite = InternalContraints.begin();
		ite != InternalContraints.end(); ite++)
		(*ite)->glDraws();
	glEnd();

	glBegin(GL_POLYGON);
	for(std::vector<Vertex*>::iterator ite = Vertices.begin();
		ite != Vertices.end(); ite++)
		Vec2 Pos = (*ite)->getPosition(),
		glVertex2f(Pos.x, Pos.y);
	glEnd();
}

}
