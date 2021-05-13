#pragma once

#include "../framework/collection.h"
#include "material.h"
#include <map>

class Materials : public collection
{
public:
	Materials();
	virtual ~Materials();

	void describe(SymbolTable* table);

// Dispatcher interface
public:
	virtual base* clone();

	virtual base* invoke(std::wstring& name, std::vector<base *>& params);
// Collection interface
public:
	base* add(std::vector<base *>& params);
};