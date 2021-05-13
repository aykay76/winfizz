#include "../stdafx.h"
#include "../framework/real.h"
#include "../framework/string.h"
#include "materials.h"
#include "engine.h"
#include <stack>

#include "../script/symboltable.h"
#include "../script/symboltablenode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

Materials::Materials()
{
}

base* Materials::clone()
{
	return new Materials(*this);
}

Materials::~Materials()
{
}

base* Materials::invoke(std::wstring& name, std::vector<base *>& params)
{
	if (name.compare(L"Add") == 0)
	{
		return add(params);
	}

	return NULL;
}

base* Materials::add(std::vector<base *>& params)
{
	Material* material = new Material;

	std::stack<base *>& stack = Engine::instance()->runtimeStack();
	real* type = NULL;
	Colour* c = NULL;

	framework::string* name = (framework::string *)params[0];
	c = (Colour *)params[1];
	material->m_ambient = *c;
	c = (Colour *)params[2];
	material->m_diffuse = *c;
	c = (Colour *)params[3];
	material->m_specular= *c;
	c = (Colour *)params[4];
	material->m_emission = *c;
	// the shininess
	type = (real *)params[5];
	material->m_shininess = (int)(*type);

	// how about a texture?
	if (params.size() > 6)
	{
		framework::string* texture = static_cast<framework::string *>(params[6]);
		
		std::map<framework::string, base *>::iterator it = Engine::instance()->textures().members().find(*texture);
		if (it != Engine::instance()->textures().members().end())
		{
			material->m_texture = static_cast<Texture *>(it->second);
		}
	}

	// add to the materials list ready for future use
	m_members[*name] = material;

	return NULL;
}


void Materials::describe(SymbolTable* table)
{
	SymbolTableNode* newNode = table->enter(L"Materials", dtCollection);
	SymbolTable* newTable = new SymbolTable(table);
	newNode->symbols(newTable);
	newNode->object(this);

	// set the type of object this is a collection of
	newNode->type(Engine::instance()->globals().find(std::wstring(L"Material")));

	// add any other parameters or methods specific to this object
	newNode = newTable->enter(L"Add", dtMethod);
	newNode->object(this);

}