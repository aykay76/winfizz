#pragma once

#include "../framework/collection.h"
#include "camera.h"
#include <map>

class Cameras : public collection
{
public:
	Cameras();
	virtual ~Cameras();

	void describe(SymbolTable* table);

// Dispatcher interface
public:
	virtual base* clone();
	virtual base* invoke(std::wstring& name, std::vector<base *>& params);
	virtual void clear();

	void init();
// Collection interface
public:
	base* add(std::vector<base *>&params);
};