#pragma once

#include "observer.h"
#include "../script/executor.h"

#include <stack>

class SymbolTableNode;
class base;

class ScriptObserver : public Observer
{
public:
	ScriptObserver(SymbolTableNode* node, std::stack<base *>& runtimeStack);
	virtual ~ScriptObserver(void);

public:
	virtual void update();

protected:
	SymbolTableNode* m_node; // pointer to node containing icode to execute.
	Executor m_executor;
};
