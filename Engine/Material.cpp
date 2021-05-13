#include "../stdafx.h"
#include ".\material.h"
#include "../framework/string.h"
#include "../framework/real.h"
#include "../framework/integer.h"
#include "../script/symboltable.h"
#include "texture.h"

#include "engine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

Material::Material(void)
	: m_texture(0)
{
	m_ambient.m_values[0] = m_ambient.m_values[1] = m_ambient.m_values[2] = 0.2f;
	m_ambient.m_values[3] = 1.0f;

	m_diffuse.m_values[0] = m_diffuse.m_values[1] = m_diffuse.m_values[2] = 0.8f;
	m_diffuse.m_values[3] = 1.0f;

	m_specular.m_values[0] = m_specular.m_values[1] = m_specular.m_values[2] = 0.0f;
	m_specular.m_values[3] = 1.0f;

	m_emission.m_values[0] = m_emission.m_values[1] = m_emission.m_values[2] = 0.0f;
	m_emission.m_values[3] = 1.0f;

	m_shininess = 0;
}

base* Material::clone()
{
	return new Material(*this);
}

Material::~Material(void)
{
}

void Material::describe(SymbolTable* table)
{
	SymbolTableNode* node = table->enter(std::wstring(L"Material"), dtTypeDeclaration);

	// we need to add these parameters to the tree to aid syntax checking
	// these are just holding places for the names - the actual values are not
	// stored in the sript object model. if type checking is not needed this
	// doesn't need to happen
	SymbolTable* materialTable = new SymbolTable(table);
	node->symbols(materialTable);
	materialTable->enter(std::wstring(L"ambient"), dtParameter);
	materialTable->enter(std::wstring(L"diffuse"), dtParameter);
	materialTable->enter(std::wstring(L"specular"), dtParameter);
	materialTable->enter(std::wstring(L"emission"), dtParameter);
	materialTable->enter(std::wstring(L"shininess"), dtParameter);
	materialTable->enter(std::wstring(L"texture"), dtReference);
}

// get a parameter by name or ordinal
base* Material::get(base* param)
{
	framework::string* name = static_cast<framework::string *>(param);

	if (wcscmp(L"ambient", (const wchar_t *)(*name)) == 0) return &m_ambient;
	if (wcscmp(L"diffuse", (const wchar_t *)(*name)) == 0) return &m_diffuse;
	if (wcscmp(L"specular", (const wchar_t *)(*name)) == 0) return &m_specular;
	if (wcscmp(L"emission", (const wchar_t *)(*name)) == 0) return &m_emission;
	if (wcscmp(L"shininess", (const wchar_t *)(*name)) == 0) return new integer(&m_shininess);
	if (wcscmp(L"texture", (const wchar_t *)(*name)) == 0) return new framework::string(m_texture->name());

	return NULL;
}

// set parameter to value, param is name or ordinal of parameter
void Material::set(base* param, base* value)
{
	framework::string* name = static_cast<framework::string *>(param);

	if (wcscmp(L"texture", (const wchar_t *)(*name)) == 0)
	{
		framework::string* tex = static_cast<framework::string *>(value);
		std::map<framework::string, base* >::iterator it = Engine::instance()->textures().members().find(*tex);
		if (it != Engine::instance()->textures().members().end())
		{
			m_texture = static_cast<Texture *>(it->second);
		}
	}
}

// invoke a method on this object by name 
base* Material::invoke(std::wstring& name, std::vector<base *>& params)
{
	return NULL;
}

