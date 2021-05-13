#include "../stdafx.h"
#include "integer.h"
#include "real.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

integer::integer()
	: m_value(new int)
{
	m_temporary = false;
	m_local = true;
	*m_value = 0;
}

integer::integer(int i)
{
	m_value = new int;
	*m_value = i;
	m_temporary = true;
}

integer::integer(int* i)
{
	m_value = i;
	m_temporary = false;
}

integer::integer(int* i, bool temporary)
{
	m_value = i;
	m_temporary = temporary;
}

integer::~integer()
{
	if (m_value) delete m_value;
}

// export methods and parameters into the script object model so that
// this object will become scriptable
void integer::describe()
{
}

base* integer::clone()
{
	return new integer(*m_value);
}