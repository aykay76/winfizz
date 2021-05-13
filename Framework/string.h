#pragma once

#include "base.h"
#include <string>

namespace framework
{

// this is the base class of everything - it allows the script
// and the engine to represent all objects with a common base
class string : public base
{
public:
	string();
	string(const std::wstring& str);
	virtual ~string();

	static void describe(SymbolTable* table);

public:
	// clone this specific type returning a polymorphic base pointer
	virtual base* clone();

	base& operator=(const string& rhs) { m_value = rhs.m_value; return *this; }
	virtual base& operator=(const base& rhs) { throw; }
	string& operator=(const std::wstring& rhs) { m_value = rhs; return *this; }
	virtual bool operator<(const string& rhs) const { return m_value < rhs.m_value; }
	operator std::wstring() { return m_value; }
	operator const wchar_t*()
	{ 
		return m_value.c_str();
	}

	virtual base& operator+=(base& rhs) { m_value += (std::wstring)rhs; return *this; }
	virtual base& operator-=(base& rhs) { throw; }
	virtual base& operator*=(base& rhs) { throw; }
	virtual base& operator/=(base& rhs) { throw; }

	// unary operators
	virtual base& operator-() { throw; }
	virtual base& operator+() { throw; }
	virtual base& operator!() { throw; }

	virtual base& operator++() { throw; }
	virtual base& operator--() { throw; }

private:
	std::wstring m_value;
};

};