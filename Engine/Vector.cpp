#include "../framework/real.h"
#include "../framework/integer.h"
#include ".\vector.h"
#include "engine.h"
#include <math.h>

#include "constants.h"

Vector Vector::m_prototype;

Vector::Vector()
{
	m_values[0] = m_values[1] = m_values[2] = 0.0f;
	m_values[3] = 1.0f;
}

Vector::Vector(float x, float y, float z, float w)
{
	m_values[0] = x;
	m_values[1] = y;
	m_values[2] = z;
	m_values[3] = w;
}

base* Vector::clone()
{
	Vector* v = new Vector(*this);
	v->m_temporary = true;
	return v;
}

Vector::~Vector(void)
{
}

Vector& Vector::operator=(const Vector& rhs)
{
	m_values[0] = rhs.m_values[0];
	m_values[1] = rhs.m_values[1];
	m_values[2] = rhs.m_values[2];
	m_values[3] = rhs.m_values[3];

	return *this;
}

base& Vector::operator=(base& rhs)
{
	Vector& rhs2 = static_cast<Vector&>(rhs);

	m_values[0] = rhs2.m_values[0];
	m_values[1] = rhs2.m_values[1];
	m_values[2] = rhs2.m_values[2];
	m_values[3] = rhs2.m_values[3];

	return *this;
}

void Vector::construct(std::vector<base *>& params)
{
	real* type = NULL;
	int o = 0;
	type = (real *)params[o++];
	m_values[0] = (float)(*type);
	type = (real *)params[o++];
	m_values[1] = (float)(*type);
	type = (real *)(params[o++]);
	m_values[2] = (float)(*type);
	if (params.size() > 3)
	{
		type = (real *)params[o++];
		m_values[3] = (float)(*type);
	}
}

void Vector::describe(SymbolTable* table)
{
	SymbolTableNode* node = table->enter(std::wstring(L"Vector"), dtTypeDeclaration);
	SymbolTable* newTable = new SymbolTable(table);
	node->symbols(newTable);
	node->object(&m_prototype);
	newTable->enter(std::wstring(L"x"), dtParameter);
	newTable->enter(std::wstring(L"y"), dtParameter);
	newTable->enter(std::wstring(L"z"), dtParameter);
	newTable->enter(std::wstring(L"w"), dtParameter);

	newTable->enter(std::wstring(L"rotate"), dtMethod);
}

// get a parameter by name or ordinal
base* Vector::get(base* param)
{
	framework::string* name = reinterpret_cast<framework::string *>(param);

	// TODO: finish this - still not sure it's the best way
	if (wcscmp(L"x", (const wchar_t *)(*name)) == 0) return new real(&m_values[0], true);
	if (wcscmp(L"y", (const wchar_t *)(*name)) == 0) return new real(&m_values[1], true);
	if (wcscmp(L"z", (const wchar_t *)(*name)) == 0) return new real(&m_values[2], true);
	if (wcscmp(L"w", (const wchar_t *)(*name)) == 0) return new real(&m_values[3], true);

	return NULL;
}

void Vector::set(base* param, base* value)
{
}

// invoke a method on this object by name 
base* Vector::invoke(std::wstring& name, std::vector<base *>& params)
{
	if (name.compare(L"rotate") == 0)
	{
		return rotate(params);
	}

	return NULL;
}

base* Vector::rotate(std::vector<base *>& params)
{
	real* angle = (real *)params[0];
	Vector* axis = (Vector *)params[1];
	Matrix m;
	m.fromRotation((float)(*angle) * constants::pi / 180.0f, axis->m_values[0], axis->m_values[1], axis->m_values[2]);

	Vector* v = new Vector;
	*v = (*this) * m;
	v->m_temporary = true;

	return v;
}

float Vector::length() const
{
    return (float)sqrt(m_values[0] * m_values[0] + m_values[1] * m_values[1] + m_values[2] * m_values[2]);
}

void Vector::normalise()
{
    double a = length();
	double b = 1.0f / a;

    m_values[0] *= static_cast<float>(b);
    m_values[1] *= static_cast<float>(b);
    m_values[2] *= static_cast<float>(b);
}

Vector Vector::crossProduct(const Vector& v) const
{
    Vector res(m_values[1] * v.m_values[2] - m_values[2] * v.m_values[1], m_values[2] * v.m_values[0] - m_values[0] * v.m_values[2], m_values[0] * v.m_values[1] - m_values[1] * v.m_values[0]);
    return res;
}

double Vector::angleTo(const Vector& other)
{
	double dot = m_values[0] * other.m_values[0] + m_values[1] * other.m_values[1] + m_values[2] * other.m_values[2];
	double c = dot / (length() * other.length());
	return acos(c);
}

double Vector::dot(const Vector& other)
{
	return m_values[0] * other.m_values[0] + m_values[1] * other.m_values[1] + m_values[2] * other.m_values[2];
}

double Vector::distanceFromPlane(const Vector& u, const Vector& v, const Vector& planePt) const
{
	Vector n = u.crossProduct(v);
	Vector t = (*this) - planePt;

	n.normalise();

	return t.dot(n);
}