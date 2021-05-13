#include "symboltablenode.h"
#include "symboltable.h"
#include "icode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

SymbolTableNode::SymbolTableNode(const std::wstring& string, DeclarationType dt, short table, short node)
	: m_name(string)
	, m_declaration(dt)
	, m_left(NULL)
	, m_right(NULL)
	, m_table(table)
	, m_node(node)
	, m_base(NULL)
	, m_symbolTable(NULL)
	, m_icode(NULL)
	, m_type(NULL)
{
}

SymbolTableNode::~SymbolTableNode()
{
	if (m_left) delete m_left;
	if (m_right) delete m_right;
	if (m_symbolTable) delete m_symbolTable;
	if (m_icode) delete m_icode;
}
