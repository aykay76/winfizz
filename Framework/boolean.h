#pragma once

#include "base.h"

#ifdef boolean
#undef boolean
#endif

// this is the base class of everything - it allows the script
// and the engine to represent all objects with a common base
namespace framework
{

class boolean : public base
{
public:
	boolean();
	boolean(bool b) { m_value = b; }
	virtual ~boolean();

public:
	static void describe();

public:
	virtual base* clone();

	virtual base& operator=(const base& rhs) { throw; }
	boolean& operator=(const bool& rhs) { m_value = rhs; return *this; }
	operator bool() { return m_value; }

private:
	bool m_value;
};

};