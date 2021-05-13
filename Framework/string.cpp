#include "../stdafx.h"
#include "string.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace framework
{

string::string()
{
}

string::string(const std::wstring& str)
	: m_value(str)
{
}

string::~string()
{
}

// export methods and parameters into the script object model so that
// this object will become scriptable
void string::describe(SymbolTable* table)
{
}

base* string::clone()
{
	string* copy = new string(m_value);
	copy->m_temporary = true;
	return copy;
}

};