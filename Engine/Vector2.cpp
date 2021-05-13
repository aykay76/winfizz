#include "../framework/real.h"
#include ".\Vector2.h"
#include "engine.h"
#include <math.h>

Vector2 Vector2::m_prototype;

Vector2::Vector2()
{
	m_values[0] = m_values[1] = 0.0f;
}

Vector2::Vector2(float x, float y, float z, float w)
{
	m_values[0] = x;
	m_values[1] = y;
}

base* Vector2::clone()
{
	return new Vector2(*this);
}

Vector2::~Vector2(void)
{
}

Vector2& Vector2::operator=(const Vector2& rhs)
{
	m_values[0] = rhs.m_values[0];
	m_values[1] = rhs.m_values[1];

	return *this;
}

base& Vector2::operator=(const base& rhs)
{
	const Vector2& rhs2 = static_cast<const Vector2&>(rhs);

	m_values[0] = rhs2.m_values[0];
	m_values[1] = rhs2.m_values[1];

	return *this;
}

void Vector2::construct(int params)
{
	std::stack<base *>& stack = Engine::instance()->runtimeStack();

	real* type = NULL;

	type = (real *)stack.top(); stack.pop();
	m_values[1] = (float)(*type); delete type;
	type = (real *)stack.top(); stack.pop();
	m_values[0] = (float)(*type); delete type;
}

void Vector2::describe(SymbolTable* table)
{
	SymbolTableNode* node = table->enter(std::wstring(L"Vector2"), dtTypeDeclaration);
	SymbolTable* newTable = new SymbolTable(table);
	node->symbols(newTable);
	node->object(&m_prototype);
	newTable->enter(std::wstring(L"x"), dtParameter);
	newTable->enter(std::wstring(L"y"), dtParameter);
}

// get a parameter by name or ordinal
base* Vector2::get(base* param)
{
	framework::string* name = reinterpret_cast<framework::string *>(param);

	// TODO: finish this - still not sure it's the best way
	if (wcscmp(L"x", (const wchar_t *)(*name)) == 0) return new real(&m_values[0], true);
	if (wcscmp(L"y", (const wchar_t *)(*name)) == 0) return new real(&m_values[1], true);

	return NULL;
}

float Vector2::length() const
{
    return (float)sqrt(m_values[0] * m_values[0] + m_values[1] * m_values[1]);
}

void Vector2::normalise()
{
    float a = length();
	float b = 1.0f / a;

    m_values[0] *= b;
    m_values[1] *= b;
}

double Vector2::angleTo(const Vector2& other)
{
	double dot = m_values[0] * other.m_values[0] + m_values[1] * other.m_values[1] + m_values[2] * other.m_values[2];
	double c = dot / (length() * other.length());
	return acos(c);
}

float Vector2::dot(const Vector2& other)
{
	return m_values[0] * other.m_values[0] + m_values[1] * other.m_values[1];
}