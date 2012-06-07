#ifndef _COLLIDINGOBJECT_H_
#define _COLLIDINGOBJECT_H_


class CollidingObject
{
	public:
		CollidingObject();
		~CollidingObject();

		inline float getFriction() { return Friction; }
	protected:
		float Friction;
	private:
};

#endif // _COLLIDINGOBJECT_H_
