#include "../stdafx.h"
#include <windows.h>
#include ".\camera.h"
#include "../framework/string.h"
#include "../framework/real.h"
#include "../script/symboltable.h"
#include "vector.h"
#include "matrix.h"
#include "engine.h"
#include <gl/gl.h>
#include <gl/glu.h>
#include <math.h>
#include <float.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

wchar_t* Camera::m_className = L"Camera";

Camera::Camera(void)
	: m_fov(45.0f)
{
}

Camera::~Camera(void)
{
}

base* Camera::clone()
{
	return new Camera(*this);
}

void Camera::describe(SymbolTable* table)
{
	SymbolTableNode* node = table->enter(std::wstring(L"Camera"), dtTypeDeclaration);
	SymbolTableNode* typeNode = table->find(L"Entity");
	node->type(typeNode);

	// we need to add these parameters to the tree to aid syntax checking
	// these are just holding places for the names - the actual values are not
	// stored in the sript object model. if type checking is not needed this
	// doesn't need to happen
	SymbolTable* newTable = new SymbolTable(table);
	node->symbols(newTable);

	SymbolTableNode* newNode = newTable->enter(std::wstring(L"position"), dtParameter);
	newNode->type(Engine::instance()->globals().find(std::wstring(L"Vector")));
	newNode = newTable->enter(std::wstring(L"direction"), dtParameter);
	newNode->type(Engine::instance()->globals().find(std::wstring(L"Vector")));
	newNode = newTable->enter(std::wstring(L"up"), dtParameter);
	newNode->type(Engine::instance()->globals().find(std::wstring(L"Vector")));
}

// get a parameter by name or ordinal
base* Camera::get(base* param)
{
	framework::string* name = static_cast<framework::string *>(param);

	// TODO: finish this - still not sure it's the best way
//	if (wcscmp(L"position", (const wchar_t *)(*name)) == 0) return &m_position;
//	if (wcscmp(L"direction", (const wchar_t *)(*name)) == 0) return &m_direction;
//	if (wcscmp(L"up", (const wchar_t *)(*name)) == 0) return &m_up;
//	if (wcscmp(L"fov", (const wchar_t *)(*name)) == 0) return &m_fov;

	return NULL;
}

// set parameter to value, param is name or ordinal of parameter
void Camera::set(base* param, base* value)
{
}

// invoke a method on this object by name 
base* Camera::invoke(std::wstring& name, std::vector<base *>& params)
{
	return Entity::invoke(name, params);
}

// pre-draw initialisation
void Camera::init()
{
	// transform the entire world by the inverse of the camera matrix
	// to essentially put the camera at 0,0,0
	glMultMatrixf(m_matrix.inverse().m_values);
}

Matrix Camera::matrix()
{
	return m_matrix;
}

void Camera::drawSetup(int mode, bool fromParent)
{
	glMultMatrixf(m_matrix.m_values);
}