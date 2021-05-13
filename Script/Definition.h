#pragma once

class SymbolTable;
class Icode;
class Engine;

enum DefinitionType
{
	dtUndefined,
	dtTypeDeclaration,
	dtScriptlet,
	dtObject,
	dtMethod,
	dtVariable,
	dtConstant,
	dtCollection
};

// Every symbol table node has a definition type associated with it
// which allows the parser and executor to understand what to do with
// the node it is currently processing
class Definition
{
public:
	Definition(DefinitionType dt);
	~Definition(void);

	SymbolTable* m_symbolTable;
	Icode* m_icode;
	Dispatcher* m_dispatcher;
	Collection* m_collection;

	union {
		int* integer;
		float* real;
		wchar_t* character;
		bool* boolean;
	} value_ptr;

	union {
		int integer;
		float real;
		wchar_t character;
		bool boolean;
	} value;
	std::wstring string;

	TypeCode m_tc;
	bool m_constant;

	DefinitionType m_dt;
};
