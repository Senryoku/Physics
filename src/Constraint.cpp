#include "Constraint.hpp"

namespace Physics
{

Constraint::Constraint(Vertex* V1, Vertex* V2, float Length) :
								myV1(V1), myV2(V2)
{
	if(Length > 0)
		myLength = Length;
	else
		myLength = (myV1->getPosition() - myV2->getPosition()).getLength();
}

Constraint::Constraint(Vertex &V1, Vertex &V2, float Length) :
								myV1(&V1), myV2(&V2)
{
	if(Length > 0)
		myLength = Length;
	else
		myLength = (V1.getPosition() - V2.getPosition()).getLength();
}

Constraint::~Constraint()
{
}

Vertex* Constraint::operator[](const unsigned int i)
{
	if(i < 1) return myV1; else return myV2;
}

}
