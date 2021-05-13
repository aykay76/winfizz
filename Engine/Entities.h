#pragma once

#include "../framework/collection.h"
#include "entity.h"
#include <map>

// 
class Entities : public collection
{
public:
	Entities();
	virtual ~Entities();
	
	void describe(SymbolTable* table);

// Dispatcher interface
public:
	virtual base* clone();
	virtual base* invoke(std::wstring& name, std::vector<base *>& params);

// Collection interface
public:
	base* add(std::vector<base *>& params);

public:
	Entity* findForglName(GLuint name);
};