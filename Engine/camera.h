#pragma once

#include "entity.h"
#include "../framework/object.h"
#include "../framework/real.h"
#include "../framework/string.h"
#include "vector.h"

class Camera : public Entity
{
public:
	Camera(void);
	~Camera(void);

	static void describe(SymbolTable* table);

public:
	// clone this specific type returning a polymorphic base pointer
	virtual base* clone();

// object interface
public:
	// get a parameter by name or ordinal
	virtual base* get(base* param);
	// set parameter to value, param is name or ordinal of parameter
	virtual void set(base* param, base* value);
	// invoke a method on this object by name 
	virtual base* invoke(std::wstring& name, std::vector<base *>& params);
	virtual const wchar_t* className() { return m_className; }

// entity functions
public:
	virtual void drawSetup(int mode, bool fromParent = false);

	void init();

	Matrix matrix();

public:
	real m_fov;
	framework::string m_name;
	static wchar_t* m_className;
};
