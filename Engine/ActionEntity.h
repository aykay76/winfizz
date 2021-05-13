#pragma once

/******************************************************************************

this is the base class for all drawing objects (and maybe internal objects?)

******************************************************************************/

#include "entity.h"

class ActionEntity : public Entity
{
public:
	ActionEntity();
	virtual ~ActionEntity(void);

// action entities aren't scriptable so we don't need to implement the object interface
public:

// it's only really the interaction that needs to be overloaded here
public:
	virtual void leftButtonDown(int x, int y);
	virtual void leftButtonUp(int x, int y);
	virtual void mouseMove(int x, int y, bool leftButtonDown = false, bool rightButtonDown = false);
	virtual void rightButtonDown(int x, int y);
	virtual void rightButtonUp(int x, int y);
	virtual void keyPress(int key);

private:
};
