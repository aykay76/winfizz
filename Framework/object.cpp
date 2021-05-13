#include "../stdafx.h"
#include "object.h"
#include "../script/symboltable.h"
#include "../script/symboltablenode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

object::object()
{
}

object::~object()
{
}

// export methods and parameters into the script object model so that
// this object will become scriptable
void object::describe(SymbolTable* table)
{
	SymbolTableNode* newNode = table->enter(std::wstring(L"object"), dtTypeDeclaration);
//	SymbolTable* newTable = new SymbolTable(table);
//	newNode->symbols(newTable);

}

// get a parameter by name or ordinal
base* object::get(base* param)
{
	return 0;
}

// set parameter to value, param is name or ordinal of parameter
void object::set(base* param, base* value)
{
}

// invoke a method on this object by name 
base* object::invoke(std::wstring& name, std::vector<base *>& params)
{
	return NULL;
}
