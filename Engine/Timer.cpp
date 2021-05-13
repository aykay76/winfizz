#include "../stdafx.h"
#include ".\timer.h"

#include "engine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

Timer::Timer(long interval, int count)
	: m_interval(interval)
	, m_fractions(0.0)
	, m_count(count)
	, m_ticks(0)
{
}

base* Timer::clone()
{
	return new Timer(*this);
}

Timer::~Timer(void)
{
}

bool Timer::step(const double fractionSecond)
{
	if (m_count && m_ticks >= m_count)
	{
		return false;
	}

	m_fractions += fractionSecond;
	// if we overstepped the interval, trigger the timer event and save the remainder
	if (m_fractions * 1000.0 > m_interval)
	{
		m_fractions -= ((double)m_interval / 1000.0);
		Engine::instance()->addEvent(&m_event);
		m_ticks++;
	}

	return true;
}