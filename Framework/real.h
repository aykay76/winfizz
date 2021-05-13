#pragma once

#include "base.h"

#include <sstream>

class integer;

class real : public base
{
public:
	real();
	real(float f);
	real(float* f, bool temporary);
	virtual ~real();

public:
	static void describe(SymbolTable* table);

public:
	real& operator=(const float& rhs) { *m_value = rhs; return *this; }
	operator int() { return (int)*m_value; }
	operator float() { return *m_value; }
	operator std::wstring()
	{
		std::wstringstream str; str << *m_value;
		return str.str();
	}

	// clone this specific type returning a polymorphic base pointer
	virtual base* clone();

	virtual base& operator=(base& rhs) { *m_value = *((const real&)rhs).m_value; return *this; }
	virtual base& operator+=(base& rhs) { *m_value += (float)rhs; return *this; }
	virtual base& operator-=(base& rhs) { *m_value -= (float)rhs; return *this; }
	virtual base& operator*=(base& rhs) { *m_value *= (float)rhs; return *this; }
	virtual base& operator/=(base& rhs) { *m_value /= (float)rhs; return *this; }

	virtual bool operator==(base& rhs) { return (*m_value == static_cast<float>(rhs)); }
	virtual bool operator!=(base& rhs) { return (*m_value != static_cast<float>(rhs)); }
	virtual bool operator<(base& rhs) { return (*m_value < static_cast<float>(rhs)); }
	virtual bool operator>(base& rhs) { return (*m_value > static_cast<float>(rhs)); }
	virtual bool operator<=(base& rhs) { return (*m_value <= static_cast<float>(rhs)); }
	virtual bool operator>=(base& rhs) { return (*m_value >= static_cast<float>(rhs)); }

	// unary operators
	virtual base& operator-() { *m_value = -*m_value; return *this; }
	virtual base& operator+() { *m_value = +*m_value; return *this; }
	virtual base& operator!() { *m_value = !*m_value; return *this; }

	virtual base& operator++(int) { *m_value++; return *this; }
	virtual base& operator--(int) { *m_value--; return *this; }

public:
	float *m_value;
};