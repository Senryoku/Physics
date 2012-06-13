#include "Elastic.hpp"

namespace Physics
{

Elastic::Elastic(Vertex* P1, Vertex* P2,
								float Length, float Spring) :
								Constraint(P1, P2, Length),
								mySpring(Spring)
{
}

Elastic::Elastic(Vertex &P1, Vertex &P2,
								float Length, float Spring) :
								Constraint(P1, P2, Length),
								mySpring(Spring)
{
}

Elastic::~Elastic()
{
}

void Elastic::resolve()
{
	// Vecteur V1V2
	Vec2 Vect = myV2->getPosition() - myV1->getPosition();

	// Précalcul de la distance V1V2 - Peut être optimisée par une approximation
	float acLength = Vect.getLength();

	// Loi de Hooke
	float factor = mySpring*(acLength - myLength);

	// Normalisation du vecteur (pas besoin de getNormalized(), on a déjà acLength)
	if(acLength == 0.f) Vect = Vec2(1.f, 0.f);
	else Vect = Vect/acLength;

	myV2->applyForce(-Vect*factor),
	myV1->applyForce(Vect*factor);
}

void Elastic::glDraw()
{
	glBegin(GL_LINES);
	// Plus la contraite est forte, plus le lien est rouge
	glColor4f(std::abs(myLength - getVector().getLength())*mySpring*0.5f/myLength, 0.f, 0.f, 1.f);
	glVertex2f(myV1->getPosition().x, myV1->getPosition().y);
	glVertex2f(myV2->getPosition().x, myV2->getPosition().y);
	glEnd();
}

void Elastic::glDraws()
{
	// Plus la contraite est forte, plus le lien est rouge
	glColor3f(std::abs(myLength - getVector().getLength())*mySpring*0.5f/myLength, 0.f, 0.f);
	glVertex2f(myV1->getPosition().x, myV1->getPosition().y);
	glVertex2f(myV2->getPosition().x, myV2->getPosition().y);
}

}
