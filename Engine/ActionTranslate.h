#pragma once

#include "action.h"
#include "vector.h"
#include "spline.h"

#include <vector>
#include <memory>

class Entity;

class ActionTranslate : public Action
{
public:
	ActionTranslate(Entity* object, Vector& v, double duration);
	virtual ~ActionTranslate(void);

public:
	virtual bool init();

	// returns false when life is over
	virtual bool step(double fractionSecond);

protected:
	Vector m_v;
	float m_origX, m_origY, m_origZ;
	Spline m_spline;
};
