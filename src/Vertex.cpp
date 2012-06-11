#include "Vertex.hpp"

namespace Physics
{

Vertex::Vertex(float x, float y, float Mass) :
	myPosition(x, y),
	myOldPosition(myPosition), // Vitesse nulle à la création
	myAcceleration(0, 0), // Accélération nulle à la création
	myRadius(0),
	myMass(Mass),
	myBounce(0),
	myFixed(0)
{
}

Vertex::Vertex(Vec2 Pos, float Mass) :
	myPosition(Pos),
	myOldPosition(myPosition), // Vitesse nulle à la création
	myAcceleration(0, 0), // Accélération nulle à la création
	myRadius(0),
	myMass(Mass),
	myBounce(0),
	myFixed(0)
{
}

Vertex::~Vertex()
{
}

bool Vertex::setPosition(Vec2 newPos, bool oldToo)
{
	myPosition = newPos;
	if(oldToo) myOldPosition = newPos;
	return true;
}

bool Vertex::correctPosition(Vec2 add)
{
	if(myFixed) return false;
	myPosition += add;
	return true;
}

bool Vertex::setAcceleration(Vec2 newAcc)
{
	if(myFixed) return false;
	myAcceleration = newAcc;
	return true;
}

bool Vertex::setRadius(float newRadius)
{
	if(newRadius > 0) {
		myRadius = newRadius;
		return true;
	} else return false;
}

bool Vertex::setMass(float newMass)
{
	// Tests...
	myMass = newMass;
	return true;
}

void Vertex::applyForce(Vec2 Force)
{
	if(!myFixed) myAcceleration += Force/myMass;
}

void Vertex::addAcceleration(Vec2 Acc)
{
	if(!myFixed) myAcceleration += Acc;
}

void Vertex::resolve(float prevdt, float dt)
{
	if(myFixed) return;
	Vec2 tmp = myPosition;
	//            Dissipation d'énergie, dépend du milieu
	myPosition += 0.99f*((myPosition - myOldPosition)*(dt/prevdt)) // Inertie
	 + myAcceleration*dt*dt; // Accélération
	myOldPosition = tmp;
	myAcceleration = Vec2(0,0);
}

void Vertex::glDraw()
{
	glPointSize(2);
	glBegin(GL_POINTS);
	glVertex2f(myPosition.x, myPosition.y);
	glEnd();
	glPointSize(1);
}

void Vertex::glDraws()
{
	glVertex2f(myPosition.x, myPosition.y);
}

}
