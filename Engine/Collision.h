#pragma once

#include "vector.h"

class Entity;

class Collision
{
public:
	Collision(void);
public:
	~Collision(void);

	// consider the entities involved in the collision
	Entity* m_e1;
	Entity* m_e2;

	// some properties of the collision
	Vector m_collisionNormal;
	Vector m_collisionPoint;
	Vector m_collisionTangent;

	// dunno what these are
	Vector m_relativeVelocity;
	Vector m_relativeAcceleration;
};
