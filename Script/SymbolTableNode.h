#pragma once

#include <string>
#include "../framework/base.h"

enum DeclarationType
{
	dtUndefined,				// nodes with no explicit definition
	dtTypeDeclaration,			// a node which specifies a type name
	dtInteger,
	dtReal,
	dtString,
	dtConstant,					// unchangeable value with global scope
	dtVariable,					// variable with script scope
	dtScriptlet,				// piece of 'code' within the script
	dtObject,					// generic interface to exported object
	dtCollection,				// collections are special objects
	dtMethod,					// methods are exported from internal objects
	dtParameter,				// parameters are exported from internal objects
	dtReference					// exported parameter that needs to be 'set'
};

class SymbolTable;
class Icode;

class SymbolTableNode
{
public:
	SymbolTableNode(const std::wstring& string, DeclarationType dt, short table, short node);
	~SymbolTableNode();

public:
	SymbolTableNode** left() { return &m_left; }
	SymbolTableNode** right() { return &m_right; }
	short table() const { return m_table; }
	short node() const { return m_node; }
	base* object() const { return m_base; }
	void object(base* ob) { m_base = ob; }
	std::wstring& name() { return m_name; }
	Icode* icode() const { return m_icode; }
	void icode(Icode* code) { m_icode = code; }

	DeclarationType declaration() { return m_declaration; }
	void declaration(DeclarationType type) { m_declaration = type; }

	SymbolTable* symbols() { return m_symbolTable; }
	void symbols(SymbolTable* table) { m_symbolTable = table; }

	SymbolTableNode* type() { return m_type; }
	void type(SymbolTableNode* type) { m_type = type; }

protected:
	SymbolTableNode* m_left;			// pointer to branch of b-tree less than this node
	SymbolTableNode* m_right;			// pointer to branch of b-tree greater than this node
	std::wstring m_name;				// name for this node
	short m_table;						// ref to table
	short m_node;						// node index
	DeclarationType m_declaration;		// definition object tells us more about the definition
	base* m_base;						// abstract pointer to data type or exported object/collection
	SymbolTable* m_symbolTable;			// local symbol table for local variables
	Icode* m_icode;						// pointer to icode if scriptlet node
	SymbolTableNode* m_type;			// base type for types, type for variable
};

