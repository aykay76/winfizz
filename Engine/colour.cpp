#include "../stdafx.h"
#include "../framework/real.h"
#include ".\colour.h"
#include "engine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

Colour Colour::m_prototype;

Colour::Colour()
{
	m_values[0] = m_values[1] = m_values[2] = m_values[3] = 0.0f;
}

Colour::Colour(float r, float g, float b, float a)
{
	m_values[0] = r;
	m_values[1] = g;
	m_values[2] = b;
	m_values[3] = a;
}

Colour::~Colour(void)
{
}

base* Colour::clone()
{
	Colour* c = new Colour(*this);
	c->m_temporary = true;

	return c;
}

Colour& Colour::operator=(const Colour& rhs)
{
	m_values[0] = rhs.m_values[0];
	m_values[1] = rhs.m_values[1];
	m_values[2] = rhs.m_values[2];
	m_values[3] = rhs.m_values[3];

	return *this;
}

base& Colour::operator=(const base& rhs)
{
	const Colour& rhs2 = static_cast<const Colour&>(rhs);

	m_values[0] = rhs2.m_values[0];
	m_values[1] = rhs2.m_values[1];
	m_values[2] = rhs2.m_values[2];
	m_values[3] = rhs2.m_values[3];

	return *this;
}

void Colour::construct(std::vector<base *>& params)
{
	int o = 0;
	base* type = NULL;
	type = (real *)params[o++];
	m_values[0] = (float)(*type);
	type = (real *)params[o++];
	m_values[1] = (float)(*type);
	type = (real *)params[o++];
	m_values[2] = (float)(*type);
	if (params.size() > 3)
	{
		type = (real *)params[o++];
		m_values[3] = (float)(*type);
	}
}

void Colour::describe(SymbolTable* table)
{
	SymbolTableNode* node = table->enter(std::wstring(L"Colour"), dtTypeDeclaration);
	SymbolTable* newTable = new SymbolTable(table);
	node->symbols(newTable);
	node->object(&m_prototype);
	newTable->enter(std::wstring(L"r"), dtParameter);
	newTable->enter(std::wstring(L"g"), dtParameter);
	newTable->enter(std::wstring(L"b"), dtParameter);
	newTable->enter(std::wstring(L"a"), dtParameter);
}

// get a parameter by name or ordinal
base* Colour::get(base* param)
{
	framework::string* name = reinterpret_cast<framework::string *>(param);

	// TODO: finish this - still not sure it's the best way
	if (wcscmp(L"r", (const wchar_t *)(*name)) == 0) return new real(&m_values[0], true);
	if (wcscmp(L"g", (const wchar_t *)(*name)) == 0) return new real(&m_values[1], true);
	if (wcscmp(L"b", (const wchar_t *)(*name)) == 0) return new real(&m_values[2], true);
	if (wcscmp(L"a", (const wchar_t *)(*name)) == 0) return new real(&m_values[3], true);

	return NULL;
}
