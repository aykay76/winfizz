#include "../stdafx.h"
#include ".\Templates.h"
#include <stack>
#include "engine.h"
#include <stdio.h>
#include "../3ds/3ds.h"
#include "../framework/string.h"
#include "../framework/real.h"
#include "../framework/integer.h"
#include "template.h"
#include "../script/symboltable.h"
#include "../script/symboltablenode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

Templates::Templates(void)
{
}

base* Templates::clone()
{
	return new Templates(*this);
}

Templates::~Templates(void)
{
}

base* Templates::invoke(std::wstring& name, std::vector<base *>& params)
{
	if (name.compare(L"Add") == 0)
	{
		return add(params);
	}
	else if (name.compare(L"Sphere") == 0)
	{
		return sphere(params);
	}
	else if (name.compare(L"Spline") == 0)
	{
		return spline(params);
	}
	else if (name.compare(L"Torus") == 0)
	{
		return torus(params);
	}
	else if (name.compare(L"Cylinder") == 0)
	{
		return cylinder(params);
	}
	else if (name.compare(L"Cone") == 0)
	{
		return cone(params);
	}

	return NULL;
}

base* Templates::add(std::vector<base *>& params)
{
	framework::string* type = NULL;
	
	// finally, the name
	framework::string* name = (framework::string *)params[0];

	// last value on (first value off) is the filename of the texture
	type = static_cast<framework::string *>(params[1]);
	std::wstring filename = (std::wstring)(*type);

	Template* tplate = new Template;
	a3DS loader;
	loader.LoadFile(filename.c_str(), tplate->meshes());

	std::vector<Mesh>::iterator it = tplate->meshes().begin();
	std::vector<Mesh>::iterator end = tplate->meshes().end();
	while (it != end)
	{
		it->Optimise(Simple);

//		if (it->radius() > tplate->m_radius) tplate->m_radius = it->radius();

		++it;
	}

	m_members[*name] = tplate;
	
	return NULL;
}

base* Templates::sphere(std::vector<base *>& params)
{
	framework::string* name = NULL;

	Template* tplate = new Template;
	Mesh m;

	if (params.size() == 4)
	{
		name = static_cast<framework::string *>(params[0]);
		integer* slices = static_cast<integer *>(params[1]);
		integer* stacks = static_cast<integer *>(params[2]);
		real* radius = static_cast<real *>(params[3]);

		m.fromSphere((float)(*radius), (int)(*stacks), (int)(*slices));
	}
	else
	{
		name = static_cast<framework::string *>(params[0]);
		real* radius = static_cast<real *>(params[1]);
		integer* lod = static_cast<integer *>(params[2]);

		m.fromSphere((float)(*radius), (int)(*lod));
	}

	tplate->addMesh(m);

	// last is the name
	m_members[*name] = tplate;

	return NULL;
}

base* Templates::torus(std::vector<base *>& params)
{
	framework::string* name = static_cast<framework::string *>(params[0]);
	real* innerRadius = static_cast<real *>(params[1]);
	real* outerRadius = static_cast<real *>(params[2]);
	integer* stacks = static_cast<integer *>(params[3]);
	integer* slices = static_cast<integer *>(params[4]);

	Template* tplate = new Template;
	Mesh m;
	m.fromTorus((float)(*innerRadius), (float)(*outerRadius), (int)(*stacks), (int)(*slices));

	tplate->addMesh(m);

	m_members[*name] = tplate;

	return NULL;
}

base* Templates::spline(std::vector<base *>& params)
{
	framework::string* name = static_cast<framework::string *>(params[0]);
	Vector* v = static_cast<Vector *>(params[1]);
	Vector* v2 = static_cast<Vector *>(params[2]);
	Vector* v3 = static_cast<Vector *>(params[3]);
	Vector* v4 = static_cast<Vector *>(params[4]);
	integer* segs = static_cast<integer *>(params[5]);
	real* width = static_cast<real *>(params[6]);

	Template* t = new Template;
	Mesh m;
	m.fromSpline(*v, *v2, *v3, *v4, (int)(*segs), (float)(*width));
	t->addMesh(m);

	m_members[*name] = t;

	return NULL;
}

base* Templates::cylinder(std::vector<base *>& params)
{
	framework::string* name = static_cast<framework::string *>(params[0]);
	real* radius = static_cast<real *>(params[1]);
	real* height = static_cast<real *>(params[2]);
	integer* stacks = static_cast<integer *>(params[3]);
	integer* slices = static_cast<integer *>(params[4]);

	Template* tplate = new Template;
	Mesh m;
	m.fromCylinder((float)(*radius), (float)(*height), (int)(*stacks), (int)(*slices));

	tplate->addMesh(m);

	m_members[*name] = tplate;

	return NULL;
}

base* Templates::cone(std::vector<base *>& params)
{
	framework::string* name = static_cast<framework::string *>(params[0]);
	real* bottomRadius = static_cast<real *>(params[1]);
	real* topRadius = static_cast<real *>(params[2]);
	real* length = static_cast<real *>(params[3]);
	integer* stacks = static_cast<integer *>(params[4]);
	integer* slices = static_cast<integer *>(params[5]);

	Template* tplate = new Template;
	Mesh m;
	m.fromCone((float)(*bottomRadius), (float)(*topRadius), (float)(*length), (int)(*stacks), (int)(*slices));

	tplate->addMesh(m);

	m_members[*name] = tplate;

	return NULL;
}

void Templates::describe(SymbolTable* table)
{
	SymbolTableNode* newNode = table->enter(L"Templates", dtCollection);
	SymbolTable* newTable = new SymbolTable(table);
	newNode->symbols(newTable);
	newNode->object(this);

	// add any other parameters or methods specific to this object
	newNode = newTable->enter(L"Add", dtMethod);
	newNode->object(this);
	newNode = newTable->enter(L"Sphere", dtMethod);
	newNode->object(this);
	newNode = newTable->enter(L"Spline", dtMethod);
	newNode->object(this);
	newNode = newTable->enter(L"Torus", dtMethod);
	newNode->object(this);
	newNode = newTable->enter(L"Cylinder", dtMethod);
	newNode->object(this);
	newNode = newTable->enter(L"Cone", dtMethod);
	newNode->object(this);
}
