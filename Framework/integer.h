#pragma once

#include "base.h"

#include <sstream>

class real;

class integer : public base
{
public:
	integer();
	integer(int i);
	integer(int* i);
	integer(int* i, bool temporary);
	virtual ~integer();

public:
	static void describe();

public:
	integer& operator=(const int& rhs) { *m_value = rhs; return *this; }
	operator int() { return *m_value; }
	operator float() { 
		return (float)*m_value;
	}
	operator std::wstring()
	{
		std::wstringstream str; str << *m_value;
		return str.str();
	}

	// clone this specific type returning a polymorphic base pointer
	virtual base* clone();

	virtual base& operator=(base& rhs) { *m_value = *((const integer&)rhs).m_value; return *this; }
	virtual base& operator+=(base& rhs) { *m_value += (int)rhs; return *this; }
	virtual base& operator-=(base& rhs) { *m_value -= (int)rhs; return *this; }
	virtual base& operator*=(base& rhs) { *m_value *= (int)rhs; return *this; }
	virtual base& operator/=(base& rhs) { *m_value /= (int)rhs; return *this; }

	virtual bool operator==(base& rhs) { return (*m_value == (int)rhs); }
	virtual bool operator!=(base& rhs) { return (*m_value != (int)rhs); }
	virtual bool operator<(base& rhs) { return (*m_value < (int)rhs); }
	virtual bool operator<=(base& rhs) { return (*m_value <= (int)rhs); }
	virtual bool operator>(base& rhs) { return (*m_value > (int)rhs); }
	virtual bool operator>=(base& rhs) { return (*m_value >= (int)rhs); }

	// unary operators
	virtual base& operator-() { *m_value = -*m_value; return *this; }
	virtual base& operator+() { *m_value = +*m_value; return *this; }
	virtual base& operator!() { *m_value = !*m_value; return *this; }

	virtual base& operator++(int) { *m_value = *m_value + 1; return *this; }
	virtual base& operator--(int) { *m_value = *m_value - 1; return *this; }

public:
	int *m_value;
};