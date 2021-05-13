#include "../stdafx.h"
#include "real.h"
#include "integer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

real::real()
	: m_value(new float)
{
	m_temporary = false;
	m_local = true;
	*m_value = 0.0f;
}

real::real(float f)
{
	m_value = new float;
	*m_value = f;
	m_temporary = true;
	m_local = true;
}

real::real(float* f, bool temporary)
{
	m_value = f;
	m_temporary = temporary;
	m_local = false;
}

real::~real()
{
	if (m_local && m_value) delete m_value;
}

// export methods and parameters into the script object model so that
// this object will become scriptable
void real::describe(SymbolTable* table)
{
}

base* real::clone()
{
	return new real(*m_value);
}