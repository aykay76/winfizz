#include "../stdafx.h"
#include <windows.h>
#include ".\timers.h"
#include "timer.h"
#include <stack>
#include "../script/symboltablenode.h"
#include "../script/symboltable.h"
#include "../framework/string.h"
#include "../framework/integer.h"
#include "scriptobserver.h"
#include "engine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

Timers::Timers(void)
{
}

base* Timers::clone()
{
	return new Timers(*this);
}

Timers::~Timers(void)
{
}

void Timers::describe(SymbolTable* table)
{
	SymbolTableNode* newNode = table->enter(L"Timers", dtCollection);
	SymbolTable* newTable = new SymbolTable(table);
	newNode->symbols(newTable);
	newNode->object(this);

	// TODO: collection should be exported, so we should need to do this
	// add any other parameters or methods specific to this object
	newNode = newTable->enter(L"Add", dtMethod);
	newNode->object(this);
}

base* Timers::invoke(std::wstring& name, std::vector<base *>& params)
{
	if (name.compare(L"Add") == 0)
	{
		return add(params);
	}

	return NULL;
}

base* Timers::add(std::vector<base *>& params)
{
	framework::string* name = NULL;
	integer* interval = NULL;
	integer* count = NULL;
	
	name = static_cast<framework::string *>(params[0]);
	interval = (integer *)params[1];
	count = (integer *)params[2];

	Timer* timer = new Timer((long)(int)(*interval), (long)(int)(*count));
	m_members[*name] = timer;

	// also, create a script observer to run the script associated with this timer
	// update event
	SymbolTable& globals = Engine::instance()->globals();
	SymbolTableNode* node = globals.find((std::wstring)(*name));
	node = node->symbols()->find(std::wstring(L"tick"));

	ScriptObserver *so = new ScriptObserver(node, Engine::instance()->runtimeStack());
	timer->m_event.attach(so);

	return NULL;
}

void Timers::step(const double fractionSecond)
{
	std::vector<framework::string> toerase;

	std::map<framework::string, base *>::iterator it = m_members.begin();
	while (it != m_members.end())
	{
		Timer* timer = reinterpret_cast<Timer *>(it->second);
		if (timer->step(fractionSecond) == false)
		{
			delete it->second;
			toerase.push_back(it->first);
		}

		++it;
	}

	std::vector<framework::string>::iterator eit = toerase.begin();
	while (eit != toerase.end())
	{
		m_members.erase(*eit);
		++eit;
	}
}