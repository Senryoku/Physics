#ifndef _PHYSICSCONSTRAINT_H_
#define _PHYSICSCONSTRAINT_H_

#include "Vertex.hpp"


namespace Physics
{

/** @class Constraint
 * @brief Lien entre deux Vertexs
 *
 * Classe à usage interne, doit être héritée.
**/
class Constraint
{
	protected:
		Vertex*	myV1;
		Vertex*	myV2;
		float	myLength; ///< Longueur à l'équilibre

	public:
		Constraint(Vertex* V1, Vertex* V2, float Length = -1);
		Constraint(Vertex &V1, Vertex &V2, float Length = -1);
		virtual ~Constraint();

		/// @brief Applique la contrainte
		virtual void resolve() =0;

		/// @brief Accesseur de P1
		Vertex* getP1() { return myV1; }
		/// @brief Accesseur de P2
		Vertex* getP2() { return myV2; }

		Vertex* operator[](const unsigned int);

		/// @brief Retourne le vecteur associé à la contrainte (dans le sens P1 vers P2)
		Vec2 getVector() { return (myV2->getPosition() - myV1->getPosition()); }

		virtual void glDraw() =0;
};

}

#endif
