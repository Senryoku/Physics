#ifndef _PHYSICSRIGID_H_
#define _PHYSICSRIGID_H_

#include "PhysicsVertex.hpp"
#include "PhysicsConstraint.hpp"

namespace Physics
{

/** @class Rigid
 * @brief Lien rigide (fort) entre deux Vertexs
 *
 * Permet la création de polygones à partir de Vertexs
**/
class Rigid : public Constraint
{
	private:

	public:
		Rigid(Vertex* P1, Vertex* P2, float Length = -1);
		~Rigid();

		/** @brief Replace les points pour qu'ils respectent la liaison
		 *
		 * @param iterations Nombre d'itérations, un nombre important génère une simulation plus précise.
		**/
		void resolve();

		void glDraw();
		void glDraws();
};

}

#endif
