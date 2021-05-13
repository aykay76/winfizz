#pragma once

#include "parser.h"
#include "../framework/boolean.h"

class base;
class SymbolTableNode;
class Icode;

class Executor
{
public:
	Executor(SymbolTableNode* node, std::stack<base *>& runtimeStack);
	~Executor(void);

public:
	void execute();
	void executeCompound();
	void executeStatement();
	void executeIf();
	void executeWhile();
	void executeForeach();
	base* executeExpression();
	base* executeConditionalExpression();
	base* executeLogicalANDExpression();
	base* executeInclusiveORExpression();
	base* executeExclusiveORExpression();
	base* executeANDExpression();
	base* executeRelationalExpression();
	base* executeAdditiveExpression();
	base* executeMultiplicativeExpression();
	base* executeUnaryExpression();
	base* executePostfixExpression();
	base* executeAtom();

	void get();

protected:
	Parser::TokenType m_tt;
	SymbolTableNode* m_node;
	Icode* m_icode;
	std::stack<base *>& m_stack;
	object* m_object;
	std::vector<base *> m_localObjects;
};
