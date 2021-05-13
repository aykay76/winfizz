#include "../stdafx.h"
#include <windows.h>
#include "../framework/real.h"
#include "../framework/string.h"
#include "lights.h"
#include "engine.h"
#include <stack>

#include "../script/symboltable.h"
#include "../script/symboltablenode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

Lights::Lights()
{
}

Lights::~Lights()
{
}

base* Lights::clone()
{
	return new Lights(*this);
}

base* Lights::invoke(std::wstring& name, std::vector<base *>& params)
{
	if (name.compare(L"Add") == 0)
	{
		return add(params);
	}

	return NULL;
}

base* Lights::add(std::vector<base *>&params)
{
	Light* light = new Light;
	real* type = NULL;
	Colour* col = NULL;

	framework::string* name = (framework::string *)params[0];
	col = (Colour *)params[1];
	light->m_ambient = (*col);
	col = (Colour *)params[2];
	light->m_diffuse = (*col);
	col = (Colour *)params[3];
	light->m_specular = (*col);

	Vector* pos = (Vector *)params[4];
	light->setPosition(*pos);
	Vector* dir = (Vector *)params[5];
	light->m_direction = (*dir);
	type = (real *)params[6];
	light->m_exponent = (int)(*type);
	type = (real *)params[7];
	light->m_cutoff = (int)(*type);

	// add to the materials list ready for future use
	m_members[*name] = light;

	return NULL;
}

void Lights::describe(SymbolTable* table)
{
	SymbolTableNode* newNode = table->enter(L"Lights", dtCollection);
	SymbolTable* newTable = new SymbolTable(table);
	newNode->symbols(newTable);
	newNode->object(this);

	// set the type of object this is a collection of
	newNode->type(Engine::instance()->globals().find(std::wstring(L"Light")));

	// add any other parameters or methods specific to this object
	newNode = newTable->enter(L"Add", dtMethod);
	newNode->object(this);
}

void Lights::init()
{
	GLuint id = GL_LIGHT0;
	std::map<framework::string, base *>::iterator it = m_members.begin();
	std::map<framework::string, base *>::iterator end = m_members.end();
	while (it != end)
	{
		Light* light = static_cast<Light *>(it->second);
		light->init(id++);
		if (Engine::instance()->editMode())
		{
			light->draw();
		}
		++it;
	}
}