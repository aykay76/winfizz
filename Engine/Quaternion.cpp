#include "../framework/real.h"
#include ".\Quaternion.h"
#include "engine.h"

#include <math.h>

Quaternion Quaternion::m_prototype;

Quaternion::Quaternion()
{
	//		 x			   y			 z
	m_values[0] = m_values[1] = m_values[2] = 0.0f;
	//		 w
	m_values[3] = 1.0f;
}

Quaternion::Quaternion(float x, float y, float z, float w)
{
	m_values[0] = x; m_values[1] = y; m_values[2] = z; m_values[3] = w;
}

base* Quaternion::clone()
{
	return new Quaternion(*this);
}

Quaternion::~Quaternion(void)
{
}

Quaternion& Quaternion::operator=(const Quaternion& rhs)
{
	m_values[0] = rhs.m_values[0];
	m_values[1] = rhs.m_values[1];
	m_values[2] = rhs.m_values[2];
	m_values[3] = rhs.m_values[3];

	return *this;
}

base& Quaternion::operator=(const base& rhs)
{
	const Quaternion& rhs2 = static_cast<const Quaternion&>(rhs);

	m_values[0] = rhs2.m_values[0];
	m_values[1] = rhs2.m_values[1];
	m_values[2] = rhs2.m_values[2];
	m_values[3] = rhs2.m_values[3];

	return *this;
}

void Quaternion::construct(int params)
{
	std::stack<base *>& stack = Engine::instance()->runtimeStack();

	real* type = NULL;
	if (params > 3)
	{
		type = (real *)stack.top(); stack.pop();
		m_values[3] = (float)(*type); delete type;
	}
	type = (real *)stack.top(); stack.pop();
	m_values[2] = (float)(*type); delete type;
	type = (real *)stack.top(); stack.pop();
	m_values[1] = (float)(*type); delete type;
	type = (real *)stack.top(); stack.pop();
	m_values[0] = (float)(*type); delete type;
}

void Quaternion::describe(SymbolTable* table)
{
	SymbolTableNode* node = table->enter(std::wstring(L"Quaternion"), dtTypeDeclaration);
	SymbolTable* newTable = new SymbolTable(table);
	node->symbols(newTable);
	node->object(&m_prototype);
	newTable->enter(std::wstring(L"x"), dtParameter);
	newTable->enter(std::wstring(L"y"), dtParameter);
	newTable->enter(std::wstring(L"z"), dtParameter);
	newTable->enter(std::wstring(L"w"), dtParameter);
}

// get a parameter by name or ordinal
base* Quaternion::get(base* param)
{
	framework::string* name = reinterpret_cast<framework::string *>(param);

	// TODO: finish this - still not sure it's the best way
	if (wcscmp(L"x", (const wchar_t *)(*name)) == 0) return new real(&m_values[0], true);
	if (wcscmp(L"y", (const wchar_t *)(*name)) == 0) return new real(&m_values[1], true);
	if (wcscmp(L"z", (const wchar_t *)(*name)) == 0) return new real(&m_values[2], true);
	if (wcscmp(L"w", (const wchar_t *)(*name)) == 0) return new real(&m_values[3], true);

	return NULL;
}

void Quaternion::fromAngleAxis(float angle, float x, float y, float z)
{
	double halfAngle = angle / 2.0;
	double s = sin(halfAngle);
	m_values[0] = (float)(x * s);
	m_values[1] = (float)(y * s);
	m_values[2] = (float)(z * s);
	m_values[3] = (float)cos(halfAngle);
}

void Quaternion::toAngleAxis(float& angle, float& x, float& y, float& z)
{
	double length2 = m_values[0] * m_values[0] + m_values[1] * m_values[1] + m_values[2] * m_values[2];
	if (length2 > 0.0)
	{
		float s = 1.0f / (float)sqrt(length2);
		angle = 2.0f * (float)acos(m_values[3]);
		x = m_values[0] * s;
		y = m_values[1] * s;
		z = m_values[2] * s;
	}
	else
	{
		angle = 0.0;
		x = y = 0.0;
		z = -1.0;
	}
}

void Quaternion::normalise()
{
	float magnitude = (float)sqrt(m_values[3] * m_values[3] + m_values[0] * m_values[0] + m_values[1] * m_values[1] + m_values[2] * m_values[2]);

	m_values[3] /= magnitude;
	m_values[0] /= magnitude;
	m_values[1] /= magnitude;
	m_values[2] /= magnitude;
}

Quaternion Quaternion::operator*(const Quaternion& rhs)
{
	Quaternion result;

    result.m_values[3] = m_values[3] * rhs.m_values[3] - m_values[0] * rhs.m_values[0] - m_values[1] * rhs.m_values[1] - m_values[2] * rhs.m_values[2];
    result.m_values[0] = m_values[3] * rhs.m_values[0] + m_values[0] * rhs.m_values[3] + m_values[1] * rhs.m_values[2] - m_values[2] * rhs.m_values[1];
    result.m_values[1] = m_values[3] * rhs.m_values[1] + m_values[1] * rhs.m_values[3] + m_values[2] * rhs.m_values[0] - m_values[0] * rhs.m_values[2];
    result.m_values[2] = m_values[3] * rhs.m_values[2] + m_values[2] * rhs.m_values[3] + m_values[0] * rhs.m_values[1] - m_values[1] * rhs.m_values[0];

	return result;
}

Vector Quaternion::operator* (const Vector& v) const
{
	// nVidia SDK implementation
	Vector uv, uuv;
	Vector qvec(m_values[0], m_values[1], m_values[2]);
	uv = qvec.crossProduct(v);
	uuv = qvec.crossProduct(uv);
	uv *= (2.0f * m_values[3]);
	uuv *= 2.0f;

	return v + uv + uuv;
}

double Quaternion::getRoll(void)
{
	return atan2(2*(m_values[0]*m_values[1] + m_values[3]*m_values[2]), m_values[3] * m_values[3] + m_values[0]*m_values[0] - m_values[1]*m_values[1] - m_values[2]*m_values[2]);
}
//-----------------------------------------------------------------------
double Quaternion::getPitch(void)
{
	return atan2(2*(m_values[1]*m_values[2] + m_values[3]*m_values[0]), m_values[3] * m_values[3] - m_values[0]*m_values[0] - m_values[1]*m_values[1] + m_values[2]*m_values[2]);
}
//-----------------------------------------------------------------------
double Quaternion::getYaw(void)
{
	return asin(-2*(m_values[0]*m_values[2] - m_values[3]*m_values[1]));
}
