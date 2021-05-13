#pragma once

#include <string>
#include <vector>

class SymbolTable;

// this is the base class of everything - it allows the script
// and the engine to represent all objects with a common base
class base
{
public:
	base();
	virtual ~base();

public:
	void describe(SymbolTable* table);

public:
	// clone this specific type returning a polymorphic base pointer
	virtual base* clone() { throw; }
	virtual void construct(std::vector<base *>& params) { throw; }

	virtual base& operator=(base& rhs) { throw; }
	virtual base& operator+=(base& rhs) { throw; }
	virtual base& operator-=(base& rhs) { throw; }
	virtual base& operator*=(base& rhs) { throw; }
	virtual base& operator/=(base& rhs) { throw; }

	// comparison operators
	virtual bool operator==(base& rhs) { throw; }
	virtual bool operator!=(base& rhs) { throw; }
	virtual bool operator<(base& rhs) { throw; }
	virtual bool operator<=(base& rhs) { throw; }
	virtual bool operator>(base& rhs) { throw; }
	virtual bool operator>=(base& rhs) { throw; }

	// unary operators
	virtual base& operator-() { throw; }
	virtual base& operator+() { throw; }
	virtual base& operator!() { throw; }

	virtual base& operator++(int) { throw; }
	virtual base& operator--(int) { throw; };

	virtual operator int() { throw; }
	virtual operator float() { throw; }
	virtual operator std::wstring() { throw; }

	bool m_temporary;		// should this object be deleted?
	bool m_local;			// should the actual value be deleted?
};

class real;
class integer;