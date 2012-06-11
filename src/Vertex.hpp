#ifndef _PHYSICSVERTEX_H_
#define _PHYSICSVERTEX_H_

#include <cmath>
#include <list>
#include <SFML/OpenGL.hpp>

#include "Vec2.hpp"

namespace Physics
{

/** @class Vertex
 * @brief Décrit une particule dans une approximation de Verlet
 *
 * Dans un premier temps, l'approximation de Verlet pose que la dérivée de
 * l'accélération est nulle. Une prédiction de cette accélération peut elle
 * améliorer significativement la simulation ?
 *
 * L'initialisation simple suppose une vitesse et une accélération initiale
 * nulle. Une initilisation plus fine DOIT préciser une accélération initiale
 * valide (i.e. réaliste) pour une simulation précise.
**/
class Vertex
{
	private:
		Vec2	myPosition; ///< Position actuelle
		Vec2	myOldPosition; ///< Ancienne position
		Vec2	myAcceleration; ///< Acceleration courante
		float	myRadius; ///< Rayon, utilisé pour les collisions
		float	myMass; ///< Masse
		float	myBounce; ///< Coefficient de restitution
		bool	myFixed; ///< Détermine si le point est fixe ou non (applyForce n'aura alors aucun effet)

	public:
		/// @brief Constructeur par défaut
		Vertex(float x = 0.f, float y = 0.f, float Mass = 1.f);

		/// @brief Constructeur par Vec2
		Vertex(Vec2 Pos, float Mass = 1.f);

		/// @brief Destructeur par défaut
		~Vertex();

		/** @brief Accesseur de la position
		 *
		 * @return Vec2
		**/
		Vec2 getPosition() { return myPosition; }

		/** @brief Accesseur de l'ancienne position
		 *
		 * @return Vec2
		**/
		Vec2 getOldPosition() { return myOldPosition; }

		/** @brief Accesseur de l'accélération
		 *
		 * @return Vec2
		**/
		Vec2 getAcceleration() { return myAcceleration; }

		/** @brief Accesseur de la Masse
		 *
		 * @return float Masse
		**/
		float getRadius() { return myRadius; }

		/** @brief Accesseur de la Masse
		 *
		 * @return float Masse
		**/
		float getMass() { return myMass; }

		/** @brief Accesseur de la vitesse
		 *
		 * @return Vec2
		**/
		Vec2 getSpeed() { return myPosition - myOldPosition; }

		/// @brief Accesseur de myFixe
		bool isFixed() { return myFixed; }

		/** @brief Mutateur de la position
		 *
		 * @param newPos Nouvelle Position
		 * @param oldToo Vrai si l'ancienne position doit être également changée
		 * @return Vrai si la position a effectivement été changée, faux sinon
		**/
		bool setPosition(Vec2 newPos, bool oldToo = 1);

		/** @brief Corrige la position actuelle par un vecteur
		 *
		 * @param add Vecteur à ajouter (déplacement)
		 * @return Vrai si la position a effectivement été changée, faux sinon
		**/
		bool correctPosition(Vec2 add);

		/** @brief Mutateur de l'accélération
		 *
		 * @return Vrai si l'accélération effectivement été changée, faux sinon
		**/
		bool setAcceleration(Vec2 newAcc);

		/** @brief Mutateur du rayon
		 *
		 * @return Vrai si le rayon a effectivement été changée, faux sinon
		**/
		bool setRadius(float newRadius);

		/** @brief Mutateur de la masse
		 *
		 * @return Vrai si la masse a effectivement été changée, faux sinon
		**/
		bool setMass(float newMass);

		/// @brief Mutateur de myFixe
		void setFixed(bool newFixed = true) { myFixed = newFixed; }

		/** @brief Applique une force à la particule
		 *
		 * @param Force Force à ajouter/appliquer à la particule
		**/
		void applyForce(Vec2 Force);

		/** @brief Ajoute un vecteur d'accélération à la particule
		 *
		 * @param Acc Accélération à ajouter/appliquer à la particule
		**/
		void addAcceleration(Vec2 Acc);

		/** @brief Déplace la particule selon son momentum et son acceleration
		 *
		 * @param prevdt Intervalle de temps lors de la frame précédente
		 * @param dt Intervalle de temps
		**/
		void resolve(float prevdt, float dt);

		void glDraw();
		void glDraws();
};

} // Namespace Physics

#endif
