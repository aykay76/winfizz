#pragma once

#include "base.h"
#include "string.h"
#include <string>
#include <vector>

class object : public base
{
public:
	object();
	virtual ~object();

	static void describe(SymbolTable* table);

	virtual base& operator=(const base& rhs) { throw; }

public:
	// get a parameter by name or ordinal
	virtual base* get(base* param) = 0;
	// set parameter to value, param is name or ordinal of parameter
	virtual void set(base* param, base* value) = 0;
	// invoke a method on this object by name 
	virtual base* invoke(std::wstring& name, std::vector<base *>& params) = 0;

	framework::string name() { return m_name; }
	void name(framework::string& name) { m_name = name; }

private:
	framework::string m_name;

};