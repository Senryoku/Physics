#ifndef _VEC2_H_
#define _VEC2_H_

#include <cmath>

#define MIN( X, Y ) ( (X) < (Y) ? (X) : (Y) ) /* Deux macros qui déterminent le min et le max */
#define MAX( X, Y ) ( (X) > (Y) ? (X) : (Y) )

/// @brief Classe simple décrivant un vecteur de R²
class Vec2
{
	public:
		float x; ///< Composante x
		float y; ///< Composante y

		Vec2();
		Vec2(float, float);
		~Vec2();

		inline float getLength() { return sqrt(x*x + y*y); }
		inline float getSquaredLength() { return x*x + y*y; }
		inline Vec2 getNormalized() { return *this/getLength(); }
		inline void normalize() { *this /= getLength(); }
		inline Vec2 getOrthogonal() { return Vec2(-y, x); }

		Vec2& operator+=(const Vec2& V);
		Vec2& operator-=(const Vec2& V);
		Vec2& operator*=(const float& f);
		Vec2& operator/=(const float& f);

		inline friend Vec2 operator-(const Vec2& V) { return Vec2(-V.x, -V.y); }
		inline friend Vec2 operator+(const Vec2& V1, const Vec2& V2) { return Vec2(V1.x + V2.x, V1.y + V2.y); }
		inline friend Vec2 operator-(const Vec2& V1, const Vec2& V2) { return Vec2(V1.x - V2.x, V1.y - V2.y); }
		inline friend Vec2 operator*(const Vec2& V, const float& f) { return Vec2(V.x*f, V.y*f); }
		inline friend Vec2 operator*(const float& f, const Vec2& V) { return Vec2(V.x*f, V.y*f); }
		inline friend Vec2 operator/(const Vec2& V, const float& f) { return Vec2(V.x/f, V.y/f); }
		/// @brief Produit scalaire
		inline friend float operator*(const Vec2& V1, const Vec2& V2) { return (V1.x*V2.x + V1.y*V2.y); }

		inline friend bool operator==(const Vec2& V1, const Vec2& V2) { return (V1.x == V2.x && V1.y == V2.y); }
		inline friend bool operator!=(const Vec2& V1, const Vec2& V2) { return (V1.x != V2.x || V1.y != V2.y); }
		/// @brief Teste les normes
		inline friend bool operator>(Vec2& V1, Vec2& V2) { return (V1.getSquaredLength() > V2.getSquaredLength()); }
		inline friend bool operator<(Vec2& V1, Vec2& V2) { return (V1.getSquaredLength() < V2.getSquaredLength()); }
		inline friend bool operator>=(Vec2& V1, Vec2& V2) { return (V1.getSquaredLength() >= V2.getSquaredLength()); }
		inline friend bool operator<=(Vec2& V1, Vec2& V2) { return (V1.getSquaredLength() <= V2.getSquaredLength()); }
};

#endif
