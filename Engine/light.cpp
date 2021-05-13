#include "../stdafx.h"
#include <windows.h>
#include ".\light.h"
#include "../framework/string.h"
#include "../framework/real.h"
#include "../framework/integer.h"
#include "../script/symboltable.h"
#include "../engine/engine.h"
#include "constants.h"
#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glaux.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

wchar_t* Light::m_className = L"Light";

Light::Light(void)
{
	m_direction[0] = m_direction[1] = m_direction[2] = 0.0;
	m_direction[1] = -1.0;
	m_exponent = 0;
	m_cutoff = 45;
	m_spot = true;
}

base* Light::clone()
{
	return new Light(*this);
}

Light::~Light(void)
{
}

void Light::describe(SymbolTable* table)
{
	SymbolTableNode* node = table->enter(std::wstring(L"Light"), dtTypeDeclaration);

	// we need to add these parameters to the tree to aid syntax checking
	// these are just holding places for the names - the actual values are not
	// stored in the sript object model. if type checking is not needed this
	// doesn't need to happen
	SymbolTable* newTable = new SymbolTable(table);
	node->symbols(newTable);
	SymbolTableNode* newNode = newTable->enter(std::wstring(L"ambient"), dtParameter);
	newNode->type(Engine::instance()->globals().find(std::wstring(L"Colour")));

	newNode = newTable->enter(std::wstring(L"diffuse"), dtParameter);
	newNode->type(Engine::instance()->globals().find(std::wstring(L"Colour")));
	newNode = newTable->enter(std::wstring(L"specular"), dtParameter);
	newNode->type(Engine::instance()->globals().find(std::wstring(L"Colour")));
	newTable->enter(std::wstring(L"x"), dtParameter);
	newTable->enter(std::wstring(L"y"), dtParameter);
	newTable->enter(std::wstring(L"z"), dtParameter);
	newTable->enter(std::wstring(L"w"), dtParameter);
	newTable->enter(std::wstring(L"dx"), dtParameter);
	newTable->enter(std::wstring(L"dy"), dtParameter);
	newTable->enter(std::wstring(L"dz"), dtParameter);
	newTable->enter(std::wstring(L"intensity"), dtParameter);
	newTable->enter(std::wstring(L"cutoff"), dtParameter);
}

// get a parameter by name or ordinal
base* Light::get(base* param)
{
	framework::string* name = reinterpret_cast<framework::string *>(param);

	if (wcscmp(L"ambient", (const wchar_t *)(*name)) == 0) return &m_ambient;
	if (wcscmp(L"diffuse", (const wchar_t *)(*name)) == 0) return &m_diffuse;
	if (wcscmp(L"specular", (const wchar_t *)(*name)) == 0) return &m_specular;
	if (wcscmp(L"x", (const wchar_t *)(*name)) == 0) return new real(&m_matrix.m_values[12], true);
	if (wcscmp(L"y", (const wchar_t *)(*name)) == 0) return new real(&m_matrix.m_values[13], true);
	if (wcscmp(L"z", (const wchar_t *)(*name)) == 0) return new real(&m_matrix.m_values[14], true);
	if (wcscmp(L"w", (const wchar_t *)(*name)) == 0) return new real(&m_matrix.m_values[15], true);
	if (wcscmp(L"dx", (const wchar_t *)(*name)) == 0) return new real(&m_direction[0], true);
	if (wcscmp(L"dy", (const wchar_t *)(*name)) == 0) return new real(&m_direction[1], true);
	if (wcscmp(L"dz", (const wchar_t *)(*name)) == 0) return new real(&m_direction[2], true);
	if (wcscmp(L"intensity", (const wchar_t *)(*name)) == 0) return new integer(&m_exponent, true);
	if (wcscmp(L"cutoff", (const wchar_t *)(*name)) == 0) return new integer(&m_cutoff, true);

	return NULL;
}

// set parameter to value, param is name or ordinal of parameter
void Light::set(base* param, base* value)
{
}

// invoke a method on this object by name 
base* Light::invoke(std::wstring& name, std::vector<base *>& params)
{
	return NULL;
}

void Light::init(unsigned int id)
{
	glEnable(id);
	glLightfv(id, GL_AMBIENT, m_ambient.m_values);
	glLightfv(id, GL_DIFFUSE, m_diffuse.m_values);
	glLightfv(id, GL_SPECULAR, m_specular.m_values);

	if (m_spot)
	{
		Vector v = position();
		v.m_values[3] = 1.0f; // just make sure  ;)
		glLightfv(id, GL_POSITION, v.m_values);
	}
	else
	{
		Vector v = m_direction;
		v.m_values[3] = 0.0f; // ensure a directional light
		glLightfv(id, GL_POSITION, v.m_values);
	}

	// additional spotlight parameters
	if (m_spot)
	{
		glLightfv(id, GL_SPOT_DIRECTION, m_direction.m_values);
		glLighti(id, GL_SPOT_EXPONENT, m_exponent);
		glLighti(id, GL_SPOT_CUTOFF, m_cutoff);
	}
}

void Light::draw()
{
	glLoadName(m_glName);
	glPushMatrix();
	glDisable(GL_LIGHTING);
	glColor3f(1.0, 1.0, 0.0);
	Vector& pos = position();
	glTranslatef(pos.m_values[0], pos.m_values[1], pos.m_values[2]);

	Vector v(0.0, 0.0, -1.0);
	float angle = (float)v.angleTo(m_direction);
	Vector r = v.crossProduct(m_direction);
	glRotatef(angle * 180.0f / constants::pi, r.m_values[0], r.m_values[1], r.m_values[2]);

	if (m_spot)
	{
//		auxSolidCone(1.0, 2.0);
		GLUquadric* q = gluNewQuadric();
		gluCylinder(q, 1.0, 2.0, 5.0, 5, 5);
		gluDeleteQuadric(q);
	}
	else
	{
//		auxSolidSphere(2.0);
		GLUquadric* q = gluNewQuadric();
		gluSphere(q, 2.0, 5, 5);
		gluDeleteQuadric(q);
	}

	glEnable(GL_LIGHTING);
	glPopMatrix();
}