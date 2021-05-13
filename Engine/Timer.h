#pragma once
#include "../framework/base.h"
#include "event.h"
#include "observer.h"

class Timer : public base
{
public:
	Timer(long interval, int count);
	~Timer(void);

	static void describe(SymbolTable* table);

public:
	virtual base* clone();
	bool step(const double fractionSecond);
	
	virtual base& operator=(const base& rhs) { throw; }

public:
	double m_fractions;
	Event m_event;
	long m_interval;
	int	m_count;
	int m_ticks;
};
