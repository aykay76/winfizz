#pragma once

#include <string>
#include <stack>
#include "../framework/collection.h"
#include "symboltable.h"

class Reader;
class Icode;

class Parser
{
public:
	Parser(Reader* reader);
	virtual ~Parser(void);

	enum TokenType
	{
		Identifier, 
		Keyword, 
		Operator, 
		Punctuator, 
		Real, 
		Integer,
		String, 
		Assign,
		Equals,
		Multiply,
		Divide,
		Div,
		Mod,
		BitwiseAND,
		Not,
		Plus,
		Minus,
		PlusEquals,
		MinusEquals,
		BitwiseOR,
		InclusiveOR,
		AND,
		NotEqual,
		LessEqual,
		GreaterEqual,
		LessThan,
		GreaterThan,
		BlockStart, 
		BlockEnd, 
		OpenParenthesis,
		CloseParenthesis,
		MemberAccess, 
		While,
		Foreach,
		If,
		Else,
		End,
		Terminator,
		XOR,
		LeftSquare,
		RightSquare,
		Increment,
		Decrement,
		Comma
	};

public:
// parsing functions
	bool parse();
// obtain reference to global (root) symbol table
	SymbolTable& symbols() { return m_global; }

protected:
// protected parsing functions
	SymbolTableNode* parseBlock();
// TODO: we might want to expose this to handle single statement parsing?
	bool parseCompound();
	bool parseStatement();
	base* parseExpression();
	base* parseConditionalExpression();
	base* parseLogicalANDExpression();
	base* parseInclusiveORExpression();
	base* parseExclusiveORExpression();
	base* parseANDExpression();
	base* parseRelationalExpression();
	base* parseAdditiveExpression();
	base* parseSimpleExpression();
	base* parseMultiplicativeExpression();
	base* parseUnaryExpression();
	base* parsePostfixExpression();
	base* parseAtom();
	void parseExpressionList();
	bool parseIf();
	void parseWhile();
	bool parseForeach();

// token functions
protected:
	// skip past any character that constitute whitespace
	bool skipWhitespace();
	bool getToken();
	void ungetToken();
	bool getWordToken();
	bool getNumberToken();
	bool getStringToken();
	bool getSpecialToken();

// symbol table functions
protected:
	SymbolTableNode* findNode(const std::wstring& token);

protected:
	wchar_t m_ch;
	Reader* m_reader;
	std::wstring m_token;
	std::wstring m_prev;
	TokenType m_tokenType;
	SymbolTableNode* m_node;
	// need this in a type checking system
	SymbolTableNode* m_type;
	Icode* m_icode;
	// nesting level
	int m_nesting;

	// global symbol table, exposes built-in objects etc.
	SymbolTable m_global;
	// symbol table stack will contain the tables leading up to the current scope
	std::stack<SymbolTable *> m_stStack;
	// pointer to current table
	SymbolTable* m_table;

	// some built in data types
//	base m_intType;
//	base m_realType;
//	base m_stringType;
//	base m_booleanType;
	std::vector<base*> m_localObjects;
};
