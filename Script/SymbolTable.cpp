#include "../stdafx.h"
#include "symboltable.h"
#include "datatype.h"
#include "icode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

int SymbolTable::count = 0;
std::vector<SymbolTable*> SymbolTable::m_tables;

SymbolTable::SymbolTable(SymbolTable* parent)
	: m_index(count++)
	, m_nodes(0)
	, m_root(0)
	, m_parent(parent)
{
	SymbolTable::m_tables.push_back(this);
}

SymbolTable::~SymbolTable(void)
{
	delete m_root;
}

SymbolTableNode* SymbolTable::enter(const std::wstring& string, DeclarationType definition)
{
	// search for the entry, return if found
	SymbolTableNode* node;
	SymbolTableNode** nodePointer = &m_root;

	while ((node = *nodePointer) != NULL)
	{
		int c = string.compare(node->name());
		if (c == 0) return node;

		// no match, navigate the tree
		if (c < 0) nodePointer = node->left();
		if (c > 0) nodePointer = node->right();
	}

	node = new SymbolTableNode(string, definition, m_index, m_nodes++);
	*nodePointer = node;
	m_nodelist.push_back(node);

	return node;
}

SymbolTableNode* SymbolTable::find(const std::wstring& string)
{
	// search for the entry, return if found
	SymbolTableNode* node;
	SymbolTableNode** nodePointer = &m_root;

	while ((node = *nodePointer) != NULL)
	{
		int c = string.compare(node->name());
		if (c == 0) return node;

		// no match, navigate the tree
		if (c < 0) nodePointer = node->left();
		if (c > 0) nodePointer = node->right();
	}

	return node;
}