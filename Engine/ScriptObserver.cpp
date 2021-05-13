#include "scriptobserver.h"
#include "../script/symboltable.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

ScriptObserver::ScriptObserver(SymbolTableNode* node, std::stack<base *>& runtimeStack)
	: m_node(node)
	, m_executor(node, runtimeStack)
{
}

ScriptObserver::~ScriptObserver(void)
{
}

void ScriptObserver::update()
{
	m_executor.execute();
}