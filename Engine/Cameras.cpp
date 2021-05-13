#include "../stdafx.h"
#include "../framework/real.h"
#include "../framework/string.h"
#include "cameras.h"
#include "engine.h"
#include <stack>

#include "../script/symboltable.h"
#include "../script/symboltablenode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

Cameras::Cameras()
{
}

Cameras::~Cameras()
{
}

base* Cameras::clone()
{
	return new Cameras(*this);
}

base* Cameras::invoke(std::wstring& name, std::vector<base *>&params)
{
	if (name.compare(L"Add") == 0)
	{
		return add(params);
	}

	return NULL;
}

base* Cameras::add(std::vector<base *>& params)
{
	Camera* camera = new Camera;

	Vector* v = NULL;
	
	framework::string* name = static_cast<framework::string *>(params[0]);
	Vector position;
	v = (Vector *)params[1];
	real* type = (real *)params[2];
	camera->m_fov = (float)(*type);

	camera->translate(v->m_values[0], v->m_values[1], v->m_values[2], 0);

	// add to the list of cameras
	m_members[*name] = camera;
	camera->m_name = (*name);

	// also add to entities because they will be responsible for deleting this
	Engine::instance()->entities().members()[*name] = camera;

	return NULL;
}

void Cameras::describe(SymbolTable* table)
{
	SymbolTableNode* newNode = table->enter(L"Cameras", dtCollection);
	SymbolTable* newTable = new SymbolTable(table);
	newNode->symbols(newTable);
	newNode->object(this);

	// set the type of object this is a collection of
	newNode->type(Engine::instance()->globals().find(std::wstring(L"Camera")));

	// add any other parameters or methods specific to this object
	newNode = newTable->enter(L"Add", dtMethod);
	newNode->object(this);
}

void Cameras::init()
{
}

void Cameras::clear()
{
	// do nothing here because cameras are also entities and will be deleted by the entities class
}