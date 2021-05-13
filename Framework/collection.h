#pragma once

#include "string.h"
#include "object.h"

#include <map>

// this is the base class for any collections - it supports specific
// methods for indexing through an array of values
class collection : public object
{
public:
	collection();
	virtual ~collection();

	static void describe(SymbolTable* table);
	virtual void clear();

public:
	// get the item from the collection at index
	virtual base* get(base* index);
	// replace the item in the collection at index with value
	virtual void set(base* index, base* value);
	// add an item to the collection
	virtual void add(base* index, base* value);
	// invoke a method on this object by name 
	virtual base* invoke(std::wstring& name, std::vector<base *>& params);

	std::map<framework::string, base *>& members() { return m_members; }

public:
	// clone this specific type returning a polymorphic base pointer
	virtual base* clone();

protected:
	std::map<framework::string, base *> m_members;
};