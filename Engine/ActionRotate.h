#pragma once

#include "action.h"
#include "vector.h"
#include "spline.h"

#include <vector>
#include <memory>

class Entity;

class ActionRotate : public Action
{
public:
	ActionRotate(Entity* object, Vector& centre, Vector& axis, double angle, double duration);
	virtual ~ActionRotate(void);

public:
	virtual bool init();

	// returns false when life is over
	virtual bool step(double fractionSecond);

protected:
	Vector m_centre;
	Vector m_axis;
	double m_angle;
	double m_prevangle;
	Spline m_spline;
};
