#ifndef _PHYSICSPOLYGON_H_
#define _PHYSICSPOLYGON_H_

#include <vector>
#include <list>
#include <cstdarg>

#include "Vertex.hpp"
#include "Rigid.hpp"

#define FLAG_NULL 		0x00
#define WITH_LENGTH 	0x01

#define PHYSICS_NONE	0x0000
#define PHYSICS_LAYER1	0x0001
#define PHYSICS_LAYER2	0x0002
#define PHYSICS_LAYER3	0x0004
#define PHYSICS_LAYER4	0x0008
#define PHYSICS_LAYER5	0x0010
#define PHYSICS_LAYER6	0x0020
#define PHYSICS_LAYER7	0x0040
#define PHYSICS_LAYER8	0x0080
#define PHYSICS_LAYER9	0x0100
#define PHYSICS_LAYER10	0x0200
#define PHYSICS_LAYER11	0x0400
#define PHYSICS_LAYER12	0x0800
#define PHYSICS_LAYER13	0x1000
#define PHYSICS_LAYER14	0x2000
#define PHYSICS_LAYER15	0x4000
#define PHYSICS_LAYER16	0x8000
#define PHYSICS_ALL		0xFFFF

namespace Physics
{

class CollisionInfo;

class BBox
{
	public :
		Vec2 TopLeft;
		Vec2 BottomRight;

		void glDraw();
};

/** @brief Décrit un polygone CONVEXE : Ensemble de points (Vertices)
 * reliés par les liaisons rigides (Edges).
 *
 **/
class Polygon
{
	protected:
		std::vector<Vertex*> myVertices;
		std::vector<Rigid*> myEdges;
		std::vector<Rigid*> myInternalContraints;

		std::list<CollisionInfo> myCIs;

		float myFriction;
		unsigned int myDetectionMask;
		unsigned int myReactionMask;

		bool myFixed;
		bool mySaveCIs;
		std::vector<Vec2> Normals;

	public:
		/// @brief Constructeur par défaut, pour usage interne.
		Polygon();

		/** @brief Constructeur par une liste de Vertex
		 *
		 * Longueurs automatiques. Contraintes internes automatiques.
		 * @param VecVertec std::vector de Vertex*
		**/
		Polygon(std::vector<Vertex*> VecVertex);

		/** @brief Construit les contraintes
		 *
		 * Détruit les contraintes déjà présentes
		**/
		void createConstraints();

		/** @brief Détruit les contraintes
		 *
		**/
		void destroyConstraints();

		/** @brief Destructeur
		 *
		 * Détruit également les Rigid crées lors de la construction !
		**/
		virtual ~Polygon();

		// Accesseurs
		inline bool isFixed() { return myFixed; }
		inline float getFriction() { return myFriction; }
		inline Vertex* getVertex(unsigned int i) {return myVertices[i]; }
		Rigid& operator[](const unsigned int);
		inline unsigned int getDetectionMask() { return myDetectionMask; }
		inline unsigned int getReactionMask() { return myReactionMask; }

		// Mutateurs
		inline void addDetectionMask(unsigned int Mask) { myDetectionMask |= Mask; }
		inline void addReactionMask(unsigned int Mask) { myReactionMask |= Mask; }
		inline void rmDetectionMask(unsigned int Mask) { if(myDetectionMask & Mask) myDetectionMask -= Mask; }
		inline void rmReactionMask(unsigned int Mask) { if(myReactionMask & Mask) myReactionMask -= Mask; }
		inline void setDetectionMask(unsigned int Mask) { myDetectionMask = Mask; }
		inline void setReactionMask(unsigned int Mask) { myReactionMask = Mask; }
		void setFixed(bool B = true);

		/// @brief Renvoi le centre du polygone
		Vec2 getCenter();

		/// @brief Renvoi le centre de masse du polygone
		Vec2 getMassCenter();

		BBox getBBox();
		BBox getOldBBox();

		void resolveRigids();

		inline bool getSaveCIs() { return mySaveCIs; }
		void setSaveCIs(bool B = true) { mySaveCIs = B; }
		CollisionInfo collide(Polygon *P);
		void addCI(CollisionInfo CI);
		void clearCIs() { myCIs.clear(); }

		void ProjectToAxis(float &Min, float &Max, const Vec2& Axis);

		/// @brief Applique une force à tout les Vertices du polygon
		void applyForce(Vec2 V);

		void glDraw();
};

class CollisionInfo {
	public:
		Polygon*	P1;
		Polygon*	P2;
		float		Depth;
		Vec2		Normal;
		Rigid*		Edge;
		Vertex*		V;

		CollisionInfo() : P1(0), P2(0), Depth(0.f), Normal(0,0), Edge(0), V(0) { }
};

}

#endif
