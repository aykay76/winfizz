#include "../stdafx.h"
#include <windows.h>
#include "Entities.h"
#include "engine.h"
#include <stack>
#include "entity.h"
#include "../framework/real.h"
#include "../framework/string.h"

#include "../script/symboltable.h"
#include "../script/symboltablenode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

Entities::Entities()
{
}

Entities::~Entities()
{
}

base* Entities::clone()
{
	return new Entities(*this);
}

base* Entities::invoke(std::wstring& name, std::vector<base *>& params)
{
	if (name.compare(L"Add") == 0)
	{
		return add(params);
	}

	return NULL;
}

base* Entities::add(std::vector<base *>& params)
{
	Entity *e = new Entity();
	real* type = NULL;

	// finally, the name
	framework::string* name = (framework::string *)params[0];
	framework::string* meshname = (framework::string *)params[1];
	Vector* v = (Vector *)params[2];

	e->x(v->m_values[0]);
	e->y(v->m_values[1]);
	e->z(v->m_values[2]);

	// link the template so that we have access to the meshes
	std::map<framework::string, base*>::iterator it = Engine::instance()->templates().members().find(*meshname);
	if (it != Engine::instance()->templates().members().end())
	{
		e->prototype(reinterpret_cast<Template *>(it->second));
	}
	e->name((std::wstring)(*name));

	// add to the Objects list ready for future use
	m_members[*name] = e;

	// find the bounding sphere radius
	e->m_radius = e->prototype()->radius();

	return NULL;
}


void Entities::describe(SymbolTable* table)
{
	SymbolTableNode* newNode = table->enter(L"Entities", dtCollection);
	SymbolTable* newTable = new SymbolTable(table);
	newNode->symbols(newTable);
	newNode->object(this);

	// set the type of object this is a collection of
	newNode->type(Engine::instance()->globals().find(std::wstring(L"Entity")));

	// add any other parameters or methods specific to this object
	newNode = newTable->enter(L"Add", dtMethod);
	newNode->object(this);
}

Entity* Entities::findForglName(GLuint glName)
{
	std::map<framework::string, base*>::iterator it = m_members.begin();
	while (it != m_members.end())
	{
		Entity* e = static_cast<Entity *>(it->second);
		if (e && e->glName() == glName) return e;
		++it;
	}

	return NULL;
}