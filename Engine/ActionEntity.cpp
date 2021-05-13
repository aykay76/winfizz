#include "../stdafx.h"
#include <windows.h>
#include ".\ActionEntity.h"
#include "engine.h"

#include "constants.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

ActionEntity::ActionEntity()
{
}

ActionEntity::~ActionEntity(void)
{
	// we don't delete sub-objects here because we don't own them
}

void ActionEntity::leftButtonDown(int x, int y)
{
	// quite simply as an action entity we are going to capture input
	// so that we control what happens when the mouse is dragged etc.
	Engine::instance()->setCapturingEntity(this);

	Template* t = new Template;
	Mesh m; m.fromTorus(6.0f, 8.0f, 10, 20);
	t->addMesh(m);
	Engine::instance()->templates().members()[framework::string(std::wstring(L"new template"))] = t;
	Entity* e = new Entity;
	e->attach(*m_observers.begin());
	e->rotate(90.0f * constants::pi / 180.0f, 1.0, 0.0, 0.0, 0);
	e->prototype(t);
	e->name(std::wstring(L"new item"));
	Engine::instance()->entities().members()[framework::string(std::wstring(L"new item"))] = e;
//	Engine::instance()->addSub(e);

	notify();
}

void ActionEntity::leftButtonUp(int x, int y)
{
	Engine::instance()->setCapturingEntity(NULL);
}

void ActionEntity::mouseMove(int x, int y, bool leftButtonDown, bool rightButtonDown)
{
	// we have got a captive audience - do something spectacular
}

void ActionEntity::rightButtonDown(int x, int y)
{
	// either way, i'm a selectable object so i'm going to make myself selected
	Engine::instance()->setCapturingEntity(this);
}

void ActionEntity::rightButtonUp(int x, int y)
{
	Engine::instance()->setCapturingEntity(NULL);
}

void ActionEntity::keyPress(int key)
{
}