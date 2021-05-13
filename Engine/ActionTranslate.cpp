#include "../stdafx.h"
#include ".\ActionTranslate.h"
#include "entity.h"
#include "constants.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

ActionTranslate::ActionTranslate(Entity* object, Vector& v, double duration)
	: Action(object, duration)
	, m_v(v)
	, m_spline(object->position(), object->position(), object->position() + v, object->position() + v, true)
{
	m_origX = object->x();
	m_origY = object->y();
	m_origZ = object->z();
}

ActionTranslate::~ActionTranslate(void)
{
	Action::~Action();
}

bool ActionTranslate::step(double fractionSecond)
{
	bool result = Action::step(fractionSecond);

	// we need to interpolate the angle for this time so that we might include different
	// interpolation methods.
	// for now, standard linear interpolation applies
	// y = ya + (x - xa)*(yb - ya)/(xb - xa)
	// where y is the angle and x is the time
	m_origX = m_object->x();
	m_origY = m_object->y();
	m_origZ = m_object->z();

	Vector& v = m_spline.get(m_lifespan / m_duration);

//	float x = static_cast<float>(0.0 + (m_duration - m_lifespan - 0.0) * (m_v.m_values[0] - 0.0) / (m_duration - 0.0));
//	float y = static_cast<float>(0.0 + (m_duration - m_lifespan - 0.0) * (m_v.m_values[1] - 0.0) / (m_duration - 0.0));
//	float z = static_cast<float>(0.0 + (m_duration - m_lifespan - 0.0) * (m_v.m_values[2] - 0.0) / (m_duration - 0.0));
//	m_object->translate(x - m_origX, y - m_origY, z - m_origZ, 0);

	m_object->x(v.m_values[0]);
	m_object->y(v.m_values[1]);
	m_object->z(v.m_values[2]);

	return result;
}

bool ActionTranslate::init()
{
	return true;
}