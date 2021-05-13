#pragma once

#include <vector>
#include <string>

#include "symboltablenode.h"

class SymbolTableNode;

class SymbolTable
{
public:
	SymbolTable(SymbolTable* parent);
	~SymbolTable(void);

public:
	SymbolTableNode* enter(const std::wstring& string, DeclarationType dt = dtUndefined);
	SymbolTableNode* find(const std::wstring& string);

public:
	SymbolTable* m_parent;			// pointer to parent symbol table for determining types etc.
	int m_nodes;					// number of nodes in this table
	int m_index;					// self-incrementing index of this table
	SymbolTableNode* m_root;		// pointer to root node of table
	static int count;				// number of tables so far
public:
	static std::vector<SymbolTable *> m_tables; // semi-global, static list of tables
	std::vector<SymbolTableNode *> m_nodelist;
};
