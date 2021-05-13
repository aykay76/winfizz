#include "../stdafx.h"
#include "../framework/string.h"
#include "Textures.h"
#include "engine.h"
#include <stack>

#include "../script/symboltable.h"
#include "../script/symboltablenode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

Textures::Textures()
{
}

base* Textures::clone()
{
	return new Textures(*this);
}

Textures::~Textures()
{
}

base* Textures::invoke(std::wstring& name, std::vector<base *>& params)
{
	if (name.compare(L"Add") == 0)
	{
		return add(params);
	}

	return NULL;
}

base* Textures::add(std::vector<base *>& params)
{
	std::stack<base *>& stack = Engine::instance()->runtimeStack();

	Texture* texture = new Texture;
	
	// name first
	framework::string* name = static_cast<framework::string *>(params[0]);

	// filename of the texture
	framework::string* type = static_cast<framework::string *>(params[1]);
	std::wstring filename = (std::wstring)(*type);

	// create a texture from the file
	texture->fromFile(filename);

	// add to the Textures list ready for future use
	m_members[*name] = texture;

	return NULL;
}

void Textures::describe(SymbolTable* table)
{
	SymbolTableNode* newNode = table->enter(L"Textures", dtCollection);
	SymbolTable* newTable = new SymbolTable(table);
	newNode->symbols(newTable);
	newNode->object(this);

	// add any other parameters or methods specific to this object
	newNode = newTable->enter(L"Add", dtMethod);
	newNode->object(this);
}