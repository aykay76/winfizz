#pragma once

#include "../framework/collection.h"
#include "timer.h"

class Timers : public collection
{
public:
	Timers(void);
	virtual ~Timers(void);

	void describe(SymbolTable* table);

public:
	virtual base* clone();
	virtual base* invoke(std::wstring& name, std::vector<base *>& params);

	// Collection interface
public:
	base* add(std::vector<base *>& params);

public:
	void step(const double fractionSecond);
};
