#pragma once

#include "../framework/object.h"
#include "colour.h"

class Texture;


class Material : public object
{
public:
	Material(void);
	~Material(void);

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

public:
	Texture* m_texture;
	int m_shininess;
	Colour m_ambient;
	Colour m_diffuse;
	Colour m_specular;
	Colour m_emission;
};
