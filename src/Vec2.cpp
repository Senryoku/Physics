#include "Vec2.hpp"

Vec2::Vec2() : x(0), y(0)
{
}

Vec2::Vec2(float newX, float newY) : x(newX), y(newY)
{
}

Vec2::~Vec2()
{
}

Vec2& Vec2::operator+=(const Vec2& V)
{
	x += V.x;
	y += V.y;
	return *this;
}

Vec2& Vec2::operator-=(const Vec2& V)
{
	x -= V.x;
	y -= V.y;
	return *this;
}

Vec2& Vec2::operator*=(const float& f)
{
	x *= f;
	y *= f;
	return *this;
}

Vec2& Vec2::operator/=(const float& f)
{
	x /= f;
	y /= f;
	return *this;
}
