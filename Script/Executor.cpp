#include "../stdafx.h"

#include "executor.h"
#include "icode.h"
#include "../framework/base.h"
#include "../framework/real.h"
#include "../framework/string.h"
#include "../framework/integer.h"
#include "../framework/object.h"
#include "../framework/collection.h"
#include "../framework/boolean.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

Executor::Executor(SymbolTableNode* node, std::stack<base *>& runtimeStack)
	: m_node(node)
	, m_stack(runtimeStack)
{
	// store the pointer to the icode
	m_icode = m_node->icode();
}

Executor::~Executor(void)
{
	std::vector<base *>::iterator it = m_localObjects.begin();
	while (it != m_localObjects.end())
	{
		delete *it;
		++it;
	}
}

void Executor::get()
{
	m_tt = m_icode->getTokenType();
	if (m_tt == Parser::Identifier || m_tt == Parser::String || m_tt == Parser::Real || m_tt == Parser::Integer)
	{
		m_node = m_icode->getNode();
	}
}

void Executor::execute()
{
	m_icode->reset();

	// we basically follow the same pattern as the parsing functions
	// but this time actually performing the actions instead of just storing the tokens
	get();

	// there will only be one statement - it may well be a compound block
	// but there will be just the one
	executeStatement();
}

void Executor::executeStatement()
{
	// do the do
	switch (m_tt)
	{
	case Parser::If:
		executeIf();
		break;
	case Parser::While:
		executeWhile();
		break;
	case Parser::Foreach:
		executeForeach();
		break;
	case Parser::BlockStart:
		executeCompound();
		break;
	default:
		// TODO: this will result in a variable. this must be delete
		base* result = executeExpression();
		if (result && result->m_temporary) delete result;
		get();
		break;
	}
}

void Executor::executeCompound()
{
	get();

	do
	{
		executeStatement();
	}
	while (m_tt != Parser::BlockEnd && m_tt != Parser::End);

	get();
}

void Executor::executeWhile()
{
	int donePos = m_icode->getLocationMarker();

	get();

	if (m_tt != Parser::OpenParenthesis)
	{
		return;
	}

	bool result = false;
	int whilePos = m_icode->getLocationMarker();
	framework::boolean* res = NULL;

	do
	{
		get();

		// execute the expression, make sure it results in a boolean value
		// TODO: don't forget to remove this return value
		res = dynamic_cast<framework::boolean *>(executeExpression());
		result = (bool)(*res);
		if (res && res->m_temporary) delete res;

		// final token should be close parenth.
		if (m_tt != Parser::CloseParenthesis)
		{
			return;
		}

		if (result == true)
		{
			get();
			executeStatement();

			m_icode->seek(whilePos);
		}
		else
		{
			m_icode->seek(donePos);
			get();
		}
	}
	while (result == true);
}

void Executor::executeIf()
{
	int elsePos = m_icode->getLocationMarker();

	get();
	if (m_tt != Parser::OpenParenthesis)
	{
		return;
	}

	get();

	// execute the expression, make sure it results in a boolean value
	// TODO: don't forget to remove this return value
	framework::boolean* res = (framework::boolean *)executeExpression();

	// final token should be close parenth.
	if (m_tt != Parser::CloseParenthesis)
	{
		return;
	}

	// now we check the result of the expression - and the type
	// (it should be boolean)
	if ((bool)(*res) == true)
	{
		get();

		executeStatement();

		if (m_tt == Parser::Else)
		{
			// ok we got the else token, next is the location marker to jump past it
			m_icode->seek(m_icode->getLocationMarker());
			get();
		}
	}
	else
	{
		m_icode->seek(elsePos);
		get();

		// this should now be the else position
		if (m_tt == Parser::Else)
		{
			m_icode->getLocationMarker();

			// get the next token, which will be a curly bracket or an expression
			get();
			executeStatement();
		}
	}

	if (res->m_temporary) delete res;

	// TODO: if we were in the 'true' section we must skip past the 'false' section
}

void Executor::executeForeach()
{
	// TODO: get the collection

	// TODO: get the identifier node

	// TODO: check if we are at the end, loop

		// execute the statement
		executeStatement();

		// TODO: this is a collection loop, move to the next one
}

base* Executor::executeExpression()
{
	// execute the left-hand-side, keep any pointers we might need
	// because they won't be valid once we execute the right-hand-side
	base* lhs = executeConditionalExpression();
	SymbolTableNode* lhsNode = m_node;
	object* o = static_cast<object *>(lhsNode->object());

	if (m_tt == Parser::Assign)
	{
		// get the next token ready to execute the rhs
		get();
		base* rhs = executeExpression();

		// if node was previously undefined we can set the default type to its first assignment
		if (lhsNode->declaration() == dtUndefined)
		{
			lhsNode->declaration(dtParameter);
			if (rhs->m_temporary)
			{
				// TODO: this needs to get deleted, so far it is left floating
				// because nobody owns it
				lhs = rhs;
				lhs->m_local = false;
				m_localObjects.push_back(lhs);
			}
			else
			{
				lhs = rhs->clone();
			}
			lhs->m_temporary = false;
			lhsNode->object(lhs);
		}
		else if (lhsNode->declaration() == dtVariable || lhsNode->declaration() == dtParameter)
		{
			(*lhs) = (*rhs);
		}
		else if (lhsNode->declaration() == dtReference)
		{
			framework::string s(lhsNode->name());
			if (o == NULL) o = m_object;
			o->set(&s, rhs);
		}
		if (rhs->m_temporary) delete rhs;
	}
	else if (m_tt == Parser::Terminator)
	{
	}

	return lhs;
}

base* Executor::executeConditionalExpression()
{
	base* lhs = executeLogicalANDExpression();
	base* ret = lhs;

	if (m_tt == Parser::BitwiseOR)
	{
		Parser::TokenType tt = m_tt;
		framework::boolean* rhs = reinterpret_cast<framework::boolean *>(executeLogicalANDExpression());

		if (ret->m_temporary) delete ret;
		ret = new framework::boolean;
		ret->m_temporary = true;

		// TODO: do it, changing return type to boolean

		if (rhs->m_temporary) delete rhs;
	}

	return ret;
}

base* Executor::executeLogicalANDExpression()
{
	base* lhs = executeInclusiveORExpression();
	base* ret = lhs;

	if (m_tt == Parser::BitwiseAND)
	{
		Parser::TokenType tt = m_tt;
		framework::boolean* rhs = reinterpret_cast<framework::boolean *>(executeInclusiveORExpression());

		if (ret->m_temporary) delete ret;
		ret = new framework::boolean;
		ret->m_temporary = true;

		// TODO: do it, changing return type to boolean

		if (rhs->m_temporary) delete rhs;
	}

	return ret;
}

base* Executor::executeInclusiveORExpression()
{
	base* lhs = executeExclusiveORExpression();
	base* ret = lhs;

	if (m_tt == Parser::InclusiveOR)
	{
		// if we get to this point - rhs MUST be boolean
		// as must lhs, but we didn't know that until now
		Parser::TokenType tt = m_tt;
		framework::boolean* rhs = reinterpret_cast<framework::boolean *>(executeExclusiveORExpression());

		if (ret->m_temporary) delete ret;
		ret = new framework::boolean;
		ret->m_temporary = true;

		// TODO: do the operation - might need more operators on base

		if (rhs->m_temporary) delete rhs;
	}

	return ret;
}

base* Executor::executeExclusiveORExpression()
{
	base* lhs = executeANDExpression();

	// default passthrough
	base* ret = lhs;

	if (m_tt == Parser::XOR)
	{
		Parser::TokenType tt = m_tt;
		// if we get this far, rhs MUST be boolean
		framework::boolean* rhs = reinterpret_cast<framework::boolean *>(executeANDExpression());

		if (ret->m_temporary) delete ret;
		ret = new framework::boolean;
		ret->m_temporary = true;

		// TODO: Do the XOR
		(*(framework::boolean *)ret) = true;

		if (rhs->m_temporary) delete rhs;
	}

	return ret;
}

base* Executor::executeANDExpression()
{
	base* lhs = executeRelationalExpression();

	// again, the passthrough default
	base* ret = lhs;

	// if we have an AND operator we know we need to return bool
	// however, we could just be returning a primitive or variable
	// so we need to use base still
	if (m_tt == Parser::AND)
	{
		Parser::TokenType tt = m_tt;
		framework::boolean* rhs = reinterpret_cast<framework::boolean*>(executeRelationalExpression());

		if (ret->m_temporary) delete ret;
		ret = new framework::boolean;
		ret->m_temporary = true;

		// simply check the operands, keep lhs, discard rhs
		(*(framework::boolean *)ret) = (bool)(*(framework::boolean *)lhs) && (bool)(*(framework::boolean *)rhs);

		if (rhs->m_temporary) delete rhs;
	}

	return ret;
}

base* Executor::executeRelationalExpression()
{
	base* lhs = executeAdditiveExpression();
	base* ret = NULL;

	// basically any operator that results in a boolean result, result!
	if (m_tt == Parser::Equals || m_tt == Parser::NotEqual || m_tt == Parser::LessThan || m_tt == Parser::GreaterThan || m_tt == Parser::LessEqual || m_tt == Parser::GreaterEqual)
	{
		Parser::TokenType tt = m_tt;

		get();

		base* rhs = executeAdditiveExpression();

		// at this point our return value is definitely boolean
		ret = new framework::boolean();
		ret->m_temporary = true;
		(*(framework::boolean *)ret) = false;

		if (tt == Parser::Equals)
		{
			if (*lhs == *rhs) (*(framework::boolean *)ret) = true;
		}
		else if (tt == Parser::NotEqual)
		{
			if (*lhs != *rhs) (*(framework::boolean *)ret) = true;
		}
		else if (tt == Parser::LessThan)
		{
			if (*lhs < *rhs) (*(framework::boolean *)ret) = true;
		}
		else if (tt == Parser::GreaterThan)
		{
			if (*lhs > *rhs) (*(framework::boolean *)ret) = true;
		}
		else if (tt == Parser::LessEqual)
		{
			if (*lhs <= *rhs) (*(framework::boolean *)ret) = true;
		}
		else if (tt == Parser::GreaterEqual)
		{
			if (*lhs >= *rhs) (*(framework::boolean *)ret) = true;
		}

		if (lhs && lhs->m_temporary) delete lhs;
		if (rhs && rhs->m_temporary) delete rhs;
	}
	else
	{
		ret = lhs;
	}

	return ret;
}

base* Executor::executeAdditiveExpression()
{
	base* lhs = executeMultiplicativeExpression();
	base* ret = lhs;

	// we might have a minus or plus as a unary operator - if there
	// is no lhs this is definitely not arithmetic
	if (lhs && (m_tt == Parser::Plus || m_tt == Parser::Minus || m_tt == Parser::PlusEquals || m_tt == Parser::MinusEquals))
	{
		Parser::TokenType tt = m_tt;

		get();

		if (tt == Parser::Plus || tt == Parser::Minus)
		{
			// we clone the lhs because we don't want to alter what's in it if its a variable
			ret = lhs->clone();
			// we will be deleting this
			ret->m_temporary = true;
			// if lhs was temporary we will delete it now
			if (lhs->m_temporary) delete lhs;
		}
		else
		{
			// if we are doing a += or -= we don't clone the object we simply update it
			ret = lhs;
		}

		// needs to be deleted
		base* operand = executeMultiplicativeExpression();
		if (tt == Parser::Plus || tt == Parser::PlusEquals)
		{
			(*ret) += (*operand);
		}
		else if (tt == Parser::Minus || tt == Parser::MinusEquals)
		{
			(*ret) -= (*operand);
		}

		// delete the rhs, keeping the lhs to return obviously
		if (operand->m_temporary) delete operand;
	}

	return ret;
}

base* Executor::executeMultiplicativeExpression()
{
	base* lhs = executeUnaryExpression();
	base* ret = lhs;

	if (m_tt == Parser::Multiply || m_tt == Parser::Divide || m_tt == Parser::Mod)
	{
		Parser::TokenType tt = m_tt;

		get();

		if (lhs == NULL)
		{
			DebugBreak();
		}

		(*ret) = (*lhs);

		base* operand = executeUnaryExpression();
		switch (tt)
		{
		case Parser::Multiply:
			// TODO: check types are compatible
			(*ret) *= (*operand);
			break;
		case Parser::Divide:
			// TODO: check types are compatible
			(*ret) /= (*operand);
			break;
		case Parser::Mod:
			// TODO: check types are compatible
//			(*ret) %= (*operand);
			break;
		}

		if (operand && operand->m_temporary) delete operand;
	}

	return ret;
}

base* Executor::executeUnaryExpression()
{
	Parser::TokenType tt = m_tt;

	if (tt == Parser::Plus || tt == Parser::Minus || tt == Parser::Not)
	{
		get();
	}

	// ok, we have the token but we don't know what we're operating on yet
	base* operand = executePostfixExpression();
	base* ret = operand;

	// now do the operation and return the operated on datum
	if (tt == Parser::Plus || tt == Parser::Minus || tt == Parser::Not && operand)
	{
		ret = operand;

		if (tt == Parser::Plus)
		{
			(*ret) = +(*ret);
		}
		else if (tt == Parser::Minus)
		{
			(*ret) = -(*ret);
		}
		else if (tt == Parser::Not)
		{
			(*ret) = !(*ret);
		}
	}

	return ret;
}

base* Executor::executePostfixExpression()
{
	base* ret = executeAtom();

	// clone because we could be dealing with an integer or string or something
//	base* ret = type;

	bool cont = true;
	while (cont)
	{
		if (m_tt == Parser::LeftSquare)
		{
			// retain pointer to collection node because it could change in the indexer expression
			SymbolTableNode* collectionNode = m_node;

			get();
			base* index = executeExpression();

			// execute the get function
			collection* c = reinterpret_cast<collection *>(collectionNode->object());

			// pop the index off the stack
			ret = c->get(index);
			if (index->m_temporary) delete index;

			// right square? get over it!
			get();
		}
		else if (m_tt == Parser::OpenParenthesis)
		{
			SymbolTableNode* node = m_node;

			std::vector<base *> params;

			// parse the function call
			get();
			while (m_tt != Parser::CloseParenthesis)
			{
				base* param = executeExpression();

				// good sanity checking point
				if (m_tt == Parser::Comma) get();

				// add to the stack
				if (param) 
				{
					params.push_back(param);
				}
			}

			// we're done with the close parenth. get the next token ready
			get();

			// if m_node points to a type declaration, this is in fact a constructor
			// not a regular function
			if (node->declaration() == dtTypeDeclaration)
			{
				ret = node->object()->clone();

				// now we must construct the object with the values we have gathered
				ret->construct(params);
			}
			else if (node->declaration() == dtMethod)
			{
				base* retval = static_cast<object *>(ret)->invoke(node->name(), params);

				// if the stack size is greater than it was before we put the parameters on
				// we know a return value has been pushed onto the stack
				// assuming of course that the function wasn't naughty and took all its parameters off
				if (retval)
				{
					if (ret && ret->m_temporary) delete ret;
					if (retval->m_temporary) ret = retval;
					else ret = retval->clone();
				}
			}

			// delete the parameters here
			std::vector<base *>::iterator it = params.begin();
			while (it != params.end())
			{
				if ((*it)->m_temporary) delete *it;
				++it;
			}
		}
		else if (m_tt == Parser::Increment)
		{
			// this is all very well if the atom is a variable but what if it's an object member
			// check type and increment if valid
			(*ret)++;
			get();
		}
		else if (m_tt == Parser::Decrement)
		{
			(*ret)--;
			get();
		}
		else if (m_tt == Parser::MemberAccess)
		{
			// node currently points to the object being accessed
			// add it to the stack because node will soon point
			// to the method or parameter being accessed
			if (ret == NULL)
			{
				ret = m_node->object();
			}

			// get the next item ready
			get();

			// check the node - if it's a parameter we need to get it from the object
			if (m_node->declaration() == dtParameter || m_node->declaration() == dtReference)
			{
				framework::string* s = new framework::string(m_node->name());
				object* o = reinterpret_cast<object *>(ret);
				if (o) 
				{
					m_object = o;
					ret = o->get(s);
				}
				delete s;

				// get the next token ready
				get();
			}
			else
			{
				// method or collection, we have the object ready in 'type'

				// get the next token ready
				get();
			}
		}
		else
		{
			cont = false;
		}
	}

	return ret;
}

base* Executor::executeAtom()
{
	base* type = NULL;

	switch (m_tt)
	{
	case Parser::Real:
	case Parser::Integer:
	case Parser::String:
		if (m_node->object()->m_temporary)
		{
			type = m_node->object()->clone();
			type->m_temporary = true;
		}
		else
		{
			type = m_node->object();
		}
		get();
		break;
// TODO: need this for boolean literal tokens
//	case Parser::Boolean:
//		break;
	case Parser::Identifier:
		get();
		if (m_node->object()) type = m_node->object();
		break;
	case Parser::OpenParenthesis:
		get();
		type = executeExpression();
		if (m_tt != Parser::CloseParenthesis)
		{
			// TODO: error
			int a = 0;
		}
		get();
		break;
	case Parser::CloseParenthesis:
		break;
	}

	return type;
}