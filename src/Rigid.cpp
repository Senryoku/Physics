#include "Rigid.hpp"

namespace Physics
{

Rigid::Rigid(Vertex* P1, Vertex* P2,
								float Length) :
								Constraint(P1, P2, Length)
{
}

Rigid::~Rigid()
{
}

void Rigid::resolve()
{
	// Vecteur P1P2
	Vec2 Vect = myV2->getPosition() - myV1->getPosition();

	// Précalcul de la distance P1P2 - Peut être optimisée par une approximation
	float acLength = Vect.getLength();

	float factor = (acLength - myLength);

	// Normalisation du vecteur (pas besoin de Normalize(), on a déjà acLength)
	if(acLength == 0.f) Vect = Vec2(1.f, 0.f);
	else Vect = Vect*(factor/acLength);

	if(myV2->isFixed())
		myV1->correctPosition(Vect);
	else if(myV1->isFixed())
		myV2->correctPosition(-Vect);
	else {
		Vect *= 0.5f;
		myV2->correctPosition(-Vect);
		myV1->correctPosition(Vect);
	}
}

void Rigid::glDraw()
{
	glBegin(GL_LINES);
	glColor3f(0.f, 0.f, 1.f);
	glVertex2f(myV1->getPosition().x, myV1->getPosition().y);
	glVertex2f(myV2->getPosition().x, myV2->getPosition().y);
	glEnd();
}

void Rigid::glDraws()
{
	glVertex2f(myV1->getPosition().x, myV1->getPosition().y);
	glVertex2f(myV2->getPosition().x, myV2->getPosition().y);
}

}
