#include "../stdafx.h"
#include "collection.h"
#include "integer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

collection::collection()
{
}

collection::~collection()
{
}

void collection::clear()
{
	std::map<framework::string, base*>::iterator it = m_members.begin();
	while (it != m_members.end())
	{
		delete it->second;
		++it;
	}
}

base* collection::clone()
{
	// TODO: complete this
	return new collection();
}

// get the item from the collection at index
base* collection::get(base* index)
{
	framework::string* name = dynamic_cast<framework::string *>(index);
	if (name)
	{
		std::map<framework::string, base *>::iterator it = m_members.find(*reinterpret_cast<framework::string *>(index));
		if (it == m_members.end()) return NULL;
		else return it->second;
	}
	else
	{
		integer* ordinal = reinterpret_cast<integer *>(index);
		std::map<framework::string, base *>::iterator it = m_members.begin();
		for (int i = 0; i < *ordinal->m_value; i++)
		{
			++it;
		}
		return it->second;
	}

	return NULL;
}

// replace the item in the collection at index with value
void collection::set(base* index, base* value)
{
	m_members[*reinterpret_cast<framework::string *>(index)] = value;
}

void collection::add(base* index, base* value)
{
	m_members[*reinterpret_cast<framework::string *>(index)] = value;
}

base* collection::invoke(std::wstring& name, std::vector<base *>& params)
{
	return NULL;
}