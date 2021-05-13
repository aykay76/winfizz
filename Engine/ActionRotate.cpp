#include "../stdafx.h"
#include ".\actionrotate.h"
#include "entity.h"
#include "constants.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

ActionRotate::ActionRotate(Entity* object, Vector& centre, Vector& axis, double angle, double duration)
	: Action(object, duration)
	, m_centre(centre)
	, m_axis(axis)
	, m_prevangle(0)
	, m_spline(Vector(0, 0, 0), Vector(0, 0, 0), Vector(0, 0, (float)angle * constants::pi / 180.0f), Vector(0, 0, (float)angle * constants::pi / 180.0f), true)
{
	m_angle = static_cast<double>(angle * constants::pi / 180.0);
}

ActionRotate::~ActionRotate(void)
{
}

bool ActionRotate::step(double fractionSecond)
{
	bool result = Action::step(fractionSecond);

	Vector& v = m_spline.get(m_lifespan / m_duration);

	// we need to interpolate the angle for this time so that we might include different
	// interpolation methods.
	// for now, standard linear interpolation applies
	// y = ya + (x - xa)*(yb - ya)/(xb - xa)
	// where y is the angle and x is the time
//	double angle = static_cast<double>(0 + (m_duration - m_lifespan - 0) * (m_angle - 0) / (m_duration - 0));
	double angle = v.m_values[2];
	if (angle > m_angle) angle = m_angle;

	wchar_t sz[128];
	swprintf(sz, 128, L"%.4f, %.4f\n", angle, angle - m_prevangle);
	OutputDebugString(sz);

	// call the generic rotation method on the object
	m_object->rotate((float)(angle - m_prevangle), m_centre, m_axis);
	m_prevangle = angle;

	return result;
}

bool ActionRotate::init()
{
	return true;
}