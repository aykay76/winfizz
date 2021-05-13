#include "../stdafx.h"
#include ".\action.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

Action::Action(Entity* object, double duration)
	: m_duration(duration)
	, m_lifespan(duration)
	, m_object(object)
{
}

Action::~Action(void)
{
}

bool Action::step(double fractionSecond)
{
	m_lifespan -= fractionSecond;
	if (m_lifespan <= 0.0)
	{
		m_lifespan = 0.0;
		return false;
	}

	return true;
}

bool Action::init()
{
	return true;
}