#pragma once

#include "../framework/collection.h"
#include "texture.h"
#include <map>

class Textures : public collection
{
public:
	Textures();
	virtual ~Textures();

	void describe(SymbolTable* table);

// Dispatcher interface
public:
	virtual base* clone();

	virtual base* invoke(std::wstring& name, std::vector<base *>& params);

// Collection interface
public:
	base* add(std::vector<base *>& params);
};