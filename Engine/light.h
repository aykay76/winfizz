#pragma once

#include "entity.h"
#include "colour.h"
#include "../framework/real.h"

class Light : public Entity
{
public:
	Light(void);
	~Light(void);

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
	void init(unsigned int id);
	void draw();

public:
	Colour m_ambient;
	Colour m_diffuse;
	Colour m_specular;
	Vector m_direction;
	int m_exponent;
	int m_cutoff;
	bool m_spot;
	GLuint m_shadowMap;

	static int m_lightCount;
	static wchar_t* m_className;
};
