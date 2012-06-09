#ifndef _PHYSICS_H_
#define _PHYSICS_H_

/** @defgroup Physics
 * @todo Vérifier collisions
 * @todo Collisions d'objets élastiques ?
 * @todo Séparer l'affichage
 * @{
**/
#include "PhysicsVertex.h"
#include "PhysicsConstraint.h"
#include "PhysicsElastic.h"
#include "PhysicsRigid.h"
#include "PhysicsPolygon.h"
#include "PhysicsRectangle.h"

namespace Physics
{

/** @brief Met à jour la simulation
 *
 * @param prevdt Intervalle de temps lors de la dernière simulation
 * @param dt Intervalle de temps à considérer
**/
void Update(float prevdt, float dt)
{
	Elastic::ResolveAll();
	Vertex::UpdateAll(prevdt, dt);

	for(int i = 0; i < 4; i++)
		Rigid::ResolveAll(),
		Polygon::HandleCollisions(),
		Polygon::clearAllCIs();
}

void DeleteAll()
{
	Polygon::DeleteAll();
	Rigid::DeleteAll();
	Elastic::DeleteAll();
	Vertex::DeleteAll();
}

void applyForceAll(Vec2 V) { Vertex::applyForceAll(V); }
void addAccelerationAll(Vec2 V) { Vertex::addAccelerationAll(V); }

}
/// @}

#endif
