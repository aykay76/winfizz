#pragma once
#include "../framework/base.h"
#include "event.h"

class Colour : public base
{
public:
	Colour();
	Colour(float r, float g, float b, float a);
	~Colour(void);

	static void describe(SymbolTable* table);
	// get a parameter by name or ordinal
	virtual base* get(base* param);

	Colour& operator=(const Colour& rhs);
	virtual base& operator=(const base& rhs);

public:
	base* clone();
	virtual void construct(std::vector<base *>& params);
	
public:
	float m_values[4];	// 0=r, 1=g, 2=b, 3=a

private:
	static Colour m_prototype;
};
