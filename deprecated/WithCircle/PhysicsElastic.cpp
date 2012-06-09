#include "PhysicsElastic.h"

namespace Physics
{

std::list<Elastic*> Elastic::List;

Elastic::Elastic(Vertex* P1, Vertex* P2,
								float Length, float Spring) :
								Constraint(P1, P2, Length),
								mySpring(Spring)
{
	Elastic::List.push_back(this);
}

Elastic::Elastic(Vertex &P1, Vertex &P2,
								float Length, float Spring) :
								Constraint(P1, P2, Length),
								mySpring(Spring)
{
	Elastic::List.push_back(this);
}

Elastic::~Elastic()
{
	Elastic::List.remove(this);
}

void Elastic::ResolveAll()
{
	for(std::list<Elastic*>::iterator ite = Elastic::List.begin();
		ite != Elastic::List.end(); ite++)
		/*if ((*ite)->getVector().getLength()>10.f*(*ite)->myLength)
		{
			Elastic* E=(*ite);
			ite--;
			delete <#expression#>
		}
		else*/
			(*ite)->Resolve();
}

void Elastic::DeleteAll()
{
    while (Elastic::List.size()>0)
        delete (Elastic::List.front());
}

void Elastic::Resolve()
{
	// Vecteur P1P2
	Vec2 Vect = P2->getPosition() - P1->getPosition();

	// Précalcul de la distance P1P2 - Peut être optimisée par une approximation
	float acLength = Vect.getLength();

	// Loi de Hooke
	float factor = mySpring*(acLength - myLength);

	// Normalisation du vecteur (pas besoin de getNormalized(), on a déjà acLength)
	Vect = Vect/acLength;

	P2->applyForce(-Vect*factor),
	P1->applyForce(Vect*factor);
}

void Elastic::glDraw()
{
	glPushMatrix();
	glLoadIdentity();
	glBegin(GL_LINES);
	// Plus la contraite est forte, plus le lien est rouge
	glColor3f(std::abs(myLength - getVector().getLength())*mySpring*0.5f/myLength, 0.f, 0.f);
	glVertex2f(P1->getPosition().x, P1->getPosition().y);
	glVertex2f(P2->getPosition().x, P2->getPosition().y);
	glEnd();
	glPopMatrix();
}

}
