#pragma once

#include "../framework/collection.h"
#include "light.h"
#include <map>

class Lights : public collection
{
public:
	Lights();
	virtual ~Lights();

	void describe(SymbolTable* table);

// Dispatcher interface
public:
	virtual base* clone();

	virtual base* invoke(std::wstring& name, std::vector<base *>& params);

	void init();
// Collection interface
public:
	base* add(std::vector<base *>& params);
};