#include "../stdafx.h"
#include "windows.h"
#include "../framework/base.h"
#include "../framework/real.h"
#include "../framework/integer.h"
#include "../framework/string.h"
#include "parser.h"
#include "icode.h"
#include "reader.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

Parser::Parser(Reader* reader)
	: m_reader(reader)
	, m_ch(reader->getChar()) // initialise
	, m_icode(NULL)
	, m_global(NULL)
	, m_node(NULL)
	, m_nesting(0)
{
	// seed the global symbol table
	m_table = &m_global;
	m_stStack.push(&m_global);
}

Parser::~Parser(void)
{
	if (m_reader) delete m_reader;
	std::vector<base *>::iterator it = m_localObjects.begin();
	while (it != m_localObjects.end())
	{
		std::vector<SymbolTable *>::iterator table_it = SymbolTable::m_tables.begin();
		while (table_it != SymbolTable::m_tables.end())
		{
			std::vector<SymbolTableNode *>::iterator node_it = (*table_it)->m_nodelist.begin();
			while (node_it != (*table_it)->m_nodelist.end())
			{
				if ((*node_it)->object() == *it)
				{
					(*node_it)->object(NULL);
				}
				++node_it;
			}
			++table_it;
		}

		delete *it;
		++it;
	}
}

bool Parser::parse()
{
	// essentially our script is a collection of blocks
	getToken();				// first token in a block must be an object name

	do
	{
		if (m_tokenType != End) 
			parseBlock();
	}
	while (m_tokenType != End);

	delete m_reader;
	m_reader = NULL;

	return true;
}

// we are dealing only with blocks of code for the moment
SymbolTableNode* Parser::parseBlock()
{
	std::wstring object;
	std::wstring event;

	// reset the icode
	m_icode = new Icode();

	if (m_tokenType != Identifier)
	{
		// TODO: syntax error
		return false;
	}

	// which object is this sciptlet associated with?
	object = m_token;

	// search the active table for the sub-table
	SymbolTableNode* objNode = m_table->find(object);
	if (objNode == NULL)
	{
		// object not found - this will be a dynamically created object
		objNode = m_table->enter(object, dtObject);
		objNode->symbols(new SymbolTable(m_table));
	}

	// make doubly sure we're dealing with an object here
	if (objNode->declaration() == dtObject)
	{
		m_stStack.push(m_table);
		m_table = objNode->symbols();
	}
	else
	{
		// object is not of correct type
	}

	// next token is the member access operator
	getToken();
	if (m_tokenType != MemberAccess)
	{
		// TODO: syntax error
		return false;
	}

	// event that triggers this block
	getToken();
	if (m_tokenType != Identifier)
	{
		// TODO: syntax error
		return false;
	}

	// store this for future reference
	event = m_token;

	// enter this into the symbol table
	SymbolTableNode* eventNode = m_table->enter(event, dtScriptlet);

	// create a new symbol table for this event block
	// this table will now be the active one - new definitions will be added to this one
	m_stStack.push(m_table);
	m_table = new SymbolTable(m_table);
	eventNode->symbols(m_table);

	// seed the token ready for the parsing
	getToken();

	// now parse the contents of the block
	parseStatement();

	// block is finished - associate the icode with the symbol table entry for this routine
	eventNode->icode(m_icode);
	m_icode = NULL;

	m_stStack.pop();			// pop off the event table
	m_stStack.pop();			// and the object table
	m_table = m_stStack.top();	// and point back to to global

	return eventNode;
}

bool Parser::parseStatement()
{
	// act based upon the token type of the current token in the buffer
	switch (m_tokenType)
	{
	case If:
		parseIf();
		break;
	case While:
		parseWhile();
		break;
	case Foreach:
		parseForeach();
		break;
	case BlockStart:
		m_nesting++;
		parseCompound();
		m_nesting--;
		break;
	default:
		parseExpression();
		break;
	}

	return true;
}

bool Parser::parseCompound()
{
	// put the open brace into the icode so that the executor will know this is a
	// compound statement
	m_icode->put(m_tokenType);

	// we know that this is a compound statement so we can move past the BlockStart
	// token and seed the token for the next parse step
	getToken();

	// keep parsing statements until we get to the end of the current block
	// or we get to the end of the script - which should raise an error
	do
	{
		parseStatement();
	}
	while (m_tokenType != End && m_tokenType != BlockEnd);

	m_icode->put(m_tokenType);
	getToken();

	return true;
}

// if we are compiling this code we must include both if/else blocks to determine
// at runtime which branch to take. if we are interpreting we can evaluation inline
bool Parser::parseIf()
{
	m_icode->put(m_tokenType); // the If token
	int elseMarker = m_icode->putLocationMarker();

	// make sure the next token abides by syntax rules
	getToken();
	if (m_tokenType != OpenParenthesis)
	{
		// TODO: syntax error
	}
	else
	{
		m_icode->put(m_tokenType);
	}

	getToken();

	// parse expression - must be boolean
	base* type = parseExpression();
	if (m_tokenType != CloseParenthesis)
	{
		// TODO: syntax error
	}
	else
	{
		m_icode->put(m_tokenType);

		// move on
		getToken();
	}

	// parse the "if-true" statement
	parseStatement();

	// now fix-up the location marker for the else block
	m_icode->fixupLocationMarker(elseMarker);

	if (m_tokenType == Else)
	{
		m_icode->put(m_tokenType);

		int followMarker = m_icode->putLocationMarker();

		// we can move past the else token now
		getToken();

		// parse the else side of things
		parseStatement();

		// fixup the marker to jump to if the else block is not being used
		m_icode->fixupLocationMarker(followMarker);
	}

	// at this point if we have a BlockEnd it's because the if block
	// contained a compound statement, move past it
	if (m_tokenType == BlockEnd)
	{
	}

	return true;
}

void Parser::parseWhile()
{
	// put the while token into the icode
	m_icode->put(m_tokenType);

	int doneMarker = m_icode->putLocationMarker();

	// make sure the next token abides by syntax rules
	getToken();
	if (m_tokenType != OpenParenthesis)
	{
		// TODO: syntax error
	}
	else
	{
		m_icode->put(m_tokenType);
	}

	int whileMarker = m_icode->putLocationMarker();
	m_icode->fixupLocationMarker(whileMarker);
	getToken();

	// parse expression - must be boolean
	base* type = parseExpression();
	if (m_tokenType != CloseParenthesis)
	{
		// TODO: syntax error
	}
	else
	{
		m_icode->put(m_tokenType);

		// move on
		getToken();
	}

	// parse the "if-true" statement
	parseStatement();

	// fix up the marker to jump to when done
	m_icode->fixupLocationMarker(doneMarker);
}

// foreach Collection object <stmt>
bool Parser::parseForeach()
{
	// TODO: need to finish this!

	// we may have an expression here rather than a simple collection reference
	// however, the expression return type must be a collection
	getToken();
	
	base* type = parseExpression();

	// on the failure of parseExpression, the current token is the identifier for the loop
	// i guess we just add it to the symbol table - it will be updated on execution

	getToken();
	parseStatement();

	return true;
}

base* Parser::parseExpression()
{
	base* type = parseConditionalExpression();

	if (m_tokenType == Assign)
	{
		// lhs will be a variable or an object parameter
		// if its an object parameter we need to call set on the object
		m_icode->put(m_tokenType);

		getToken();

		// get the rhs
		parseExpression();
	}

	// sometimes an expression will be terminated, but in the case of an If
	// condition, it will not - only move on if we are a Terminator
	if (m_tokenType == Terminator)
	{
		m_icode->put(m_tokenType);
		getToken();
	}

	return type;
}

base* Parser::parseConditionalExpression()
{
	base* type = NULL;

	parseLogicalANDExpression();

	if (m_tokenType == BitwiseOR)
	{
		m_icode->put(m_tokenType);
		getToken();
		parseLogicalANDExpression();
	}

	return type;
}

base* Parser::parseLogicalANDExpression()
{
	base* type = NULL;

	parseInclusiveORExpression();

	if (m_tokenType == BitwiseAND)
	{
		m_icode->put(m_tokenType);
		getToken();
		parseInclusiveORExpression();
	}

	return type;
}

base* Parser::parseInclusiveORExpression()
{
	base* type = NULL;

	parseExclusiveORExpression();

	if (m_tokenType == InclusiveOR)
	{
		m_icode->put(m_tokenType);
		getToken();
		parseExclusiveORExpression();
	}

	return type;
}

base* Parser::parseExclusiveORExpression()
{
	base* type = NULL;

	parseANDExpression();

	if (m_tokenType == XOR)
	{
		m_icode->put(m_tokenType);
		getToken();
		parseANDExpression();
	}

	return type;
}

base* Parser::parseANDExpression()
{
	base* type = NULL;

	parseRelationalExpression();

	if (m_tokenType == AND)
	{
		m_icode->put(m_tokenType);
		getToken();
		parseRelationalExpression();
	}

	return type;
}

base* Parser::parseRelationalExpression()
{
	base* type = NULL;

	parseAdditiveExpression();

	if (m_tokenType == Equals || m_tokenType == NotEqual || 
		m_tokenType == LessThan || m_tokenType == GreaterThan || 
		m_tokenType == LessEqual || m_tokenType == GreaterEqual)
	{
		m_icode->put(m_tokenType);
		getToken();
		parseAdditiveExpression();
	}

	return type;
}

base* Parser::parseAdditiveExpression()
{
	base* type = NULL;

	parseMultiplicativeExpression();

	if (m_tokenType == Plus || m_tokenType == Minus || m_tokenType == PlusEquals || m_tokenType == MinusEquals)
	{
		m_icode->put(m_tokenType);
		getToken();
		parseMultiplicativeExpression();
	}

	return type;
}

base* Parser::parseMultiplicativeExpression()
{
	base* type = NULL;

	parseUnaryExpression();

	if (m_tokenType == Multiply || m_tokenType == Divide || m_tokenType == Mod)
	{
		m_icode->put(m_tokenType);
		// get the next token ready for parsing
		getToken();
		parseUnaryExpression();
	}

	return type;
}

base* Parser::parseUnaryExpression()
{
	base* type = NULL;

	// first we apparently look for a unary operator, + or -
	if (m_tokenType == Plus || m_tokenType == Minus || m_tokenType == Not)
	{
        m_icode->put(m_tokenType);
		getToken();
		type = parseUnaryExpression();
	}

	return parsePostfixExpression();
}

base* Parser::parsePostfixExpression()
{
	base* type = NULL;
	std::wstring nameToken = m_token;

	// store this pointer to return to later
	SymbolTable* table = m_table;
	int pushes = 0;

	// the smallest building block - it's a literal or an identifier
	parseAtom();

	// we can only postifx an identifier
	bool cont = true;
	while (cont)
	{
		// TODO: This is ALL WRONG! - for the index operator to point to a node ready for member
		// access this means the entire collection must be in the symbol table
		// Internal collections may well not be in the symbol table, in which case index operators
		// and member access operators must be added to the icode
		// and member access operators ARE NOT

		// check for a postfix operator
		if (m_tokenType == LeftSquare)
		{
			SymbolTableNode* collectionNode = m_node;

			// store the node of the object/variable/array being accessed
			m_icode->put(m_tokenType);

			// push the symbol table onto the stack and move down a level
			m_stStack.push(m_table);
			pushes++;
			m_table = m_node->symbols();

			// now process the indexer
			getToken();
			parseExpression();
			if (m_tokenType != RightSquare)
			{
				// TODO: syntax error
			}

			// put the right square into the icode
			m_icode->put(m_tokenType);

			// done with that, get the next one
			getToken();
//			m_icode->put(m_tokenType);

			// we could have got an object from the collection, in which case we need to use its
			// symbol table to find its members if we come across another postfix operator
			m_node = collectionNode->type();
		}
		else if (m_tokenType == OpenParenthesis)
		{
			// store the node of the object/variable/array being accessed
			m_icode->put(m_tokenType);

			// now process the arguments list
			getToken();
			parseExpressionList();
			if (m_tokenType != CloseParenthesis)
			{
				// TODO: syntax error
			}
			m_icode->put(m_tokenType);

			// done with the close parenthesis, get the next token
			getToken();
		}
		else if (m_tokenType == MemberAccess)
		{
			// put the object into the icode as we will need that to know which object
			// we are using
			m_icode->put(m_tokenType);

			// TODO: type checking
			// check that m_node type is derived from object

			// push the symbol table onto the stack and move down a level
			m_stStack.push(m_table);
			pushes++;

			// if this node doesn't have any symbols, hopefully its type node will
			if (m_node->symbols())
			{
				m_table = m_node->symbols();
			}
			else
			{
				if (m_node->type())
				{
					m_table = m_node->type()->symbols();
				}
			}

			// get the next token here?
			getToken();
			parseAtom();
		}
		else if (m_tokenType == Increment)
		{
			m_icode->put(m_tokenType);
			getToken();
		}
		else if (m_tokenType == Decrement)
		{
			m_icode->put(m_tokenType);
			getToken();
		}
		else
		{
			cont = false;
		}
	}

	// backtrack our symbol table pointer by popping the stack
	while (pushes--)
	{
		// pop and check the top
		m_table = m_stStack.top();
		m_stStack.pop();
	}

	return type;
}

base* Parser::parseAtom()
{
	base* type = NULL;
	base* operandType = NULL;

	switch (m_tokenType)
	{
	case Identifier:
		// find or add this identifier - if adding it's currently undefined
		// we need to look in ALL tables for the identifier because it could be in a higher scope
		m_node = findNode(m_token);
		if (m_node == NULL)
		{
			// TODO: check this - we don't want to add nodes that aren't valid
			m_node = m_global.enter(m_token);
		}
		m_icode->put(m_tokenType);
		m_icode->put(m_node);

		getToken();
		break;
	case Real:
	case Integer:
		{
		// create the node if necessary and add it to the icode
		SymbolTableNode* node = findNode(m_token);
		if (node == NULL)
		{
			wchar_t* p;
			if (m_tokenType == Real)
			{
				node = m_table->enter(m_token, dtObject);
				real* o = new real((float)wcstod(m_token.c_str(), &p));
				m_localObjects.push_back(o);
				node->object(o);
			}
			else
			{
				node = m_table->enter(m_token, dtObject);
				integer* i = new integer(wcstol(m_token.c_str(), &p, 10));
				m_localObjects.push_back(i);
				node->object(i);
			}
		}

		// append the node to the icode ready for whatever operations are due to be performed
		m_icode->put(m_tokenType);
		m_icode->put(node);

		type = node->object();

		getToken();
		}
		break;
	case String:
		{
		// create the node if necessary and add it to the icode
		SymbolTableNode* node = findNode(m_token);
		if (node == NULL)
		{
			node = m_table->enter(m_token, dtObject);
			framework::string* s = new framework::string(m_token);
			m_localObjects.push_back(s);
			node->object(s);
		}

		m_icode->put(m_tokenType);
		m_icode->put(node);

		type = node->object();

		getToken();
		}
		break;
	case OpenParenthesis: // nested expressions
		m_icode->put(m_tokenType);

		// position on next token
		getToken();
		type = parseExpression();
		// TODO: check for closing parenth.
		if (m_tokenType == CloseParenthesis) 
		{
			m_icode->put(m_tokenType);
		}
		break;
	default:
		// TODO: invalid expression
		break;
	}

	return type;
}

void Parser::parseExpressionList()
{
	while (m_tokenType != CloseParenthesis)
	{
		// store token
		parseExpression();

		// token will be comma or close parenthesis
		if (m_tokenType == Comma)
		{
			m_icode->put(m_tokenType);
			// get the next token - no point in wasting time parsing a comma!
			getToken();
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// tokeniser functions

void Parser::ungetToken()
{
	m_prev = m_token;
}

bool Parser::getToken()
{
	skipWhitespace();

	if (m_prev.size() > 0)
	{
		m_token = m_prev;
		m_prev.clear();
		return true;
	}

	m_token.clear();
	
	// determine the token type from the char
	if ((m_ch >= 'a' && m_ch <= 'z') || (m_ch >= 'A' && m_ch <= 'Z'))
	{
		// will be an identifier of some kind
		getWordToken();
	}
	// TODO: danger with the - could be a unary negative operator like in -sin(0.1)
	else if (m_ch >= '0' && m_ch <= '9')
	{
		// number token
		getNumberToken();
	}
	else if (m_ch == '\'' || m_ch == '\"')
	{
		// string literal
		getStringToken();
		m_tokenType = String;
	}
	else if (m_ch == 0 || m_ch == -1 || m_ch == 65535)
	{
		// end of file
		m_tokenType = End;
		return false;
	}
	else
	{
		// all other characters are special tokens
		getSpecialToken();

		// TODO: make a note of whether this is a punctuator or an operator
	}

	return true;
}

bool Parser::skipWhitespace()
{
	while (m_ch == ' ' || m_ch == '\t' || m_ch == '\r' || m_ch == '\n')
	{
		m_ch = m_reader->getChar();
	}

	return true;
}

bool Parser::getWordToken()
{
	while (iswalpha(m_ch) || iswdigit(m_ch))
	{
		m_token.push_back(m_ch);
		m_ch = m_reader->getChar();
	}

	// we have the word - check if it's reserved or not - if it's not reserved it must be an object identifier
	if (m_token.compare(L"foreach") == 0)
	{
		m_tokenType = Foreach;
	}
	else if (m_token.compare(L"if") == 0)
	{
		m_tokenType = If;
	}
	else if (m_token.compare(L"else") == 0)
	{
		m_tokenType = Else;
	}
	else if (m_token.compare(L"or") == 0)
	{
		m_tokenType = BitwiseOR;
	}
	else if (m_token.compare(L"and") == 0)
	{
		m_tokenType = BitwiseAND;
	}
	else if (m_token.compare(L"while") == 0)
	{
		m_tokenType = While;
	}
	else
	{
		m_tokenType = Identifier;
	}

	return true;
}

bool Parser::getNumberToken()
{
	m_tokenType = Integer;

	while (iswdigit(m_ch) || m_ch == '.' || m_ch == '-')
	{
		if (m_ch == '.') m_tokenType = Real;

		m_token.push_back(m_ch);
		m_ch = m_reader->getChar();
	}

	return true;
}

bool Parser::getStringToken()
{
	wchar_t quote = m_ch;
	m_ch = m_reader->getChar();
	do
	{
		// TODO: enable escaped quotes
		if (m_ch != quote) m_token.push_back(m_ch);

		m_ch = m_reader->getChar();
	}
	while (m_ch != quote);

	// stuff the buffer ready for the next token
	m_ch = m_reader->getChar();

	return true;
}

bool Parser::getSpecialToken()
{
	bool getNextChar = true;

	switch (m_ch)
	{
	case '[':
		m_tokenType = LeftSquare;
		break;
	case ']':
		m_tokenType = RightSquare;
		break;
	case '{':
		m_tokenType = BlockStart;
		break;
	case '}':
		m_tokenType = BlockEnd;
		break;
	case ':':
		m_tokenType = MemberAccess;
		break;
	case '=':
		m_tokenType = Assign;
		m_token.push_back(m_ch);
		m_ch = m_reader->getChar();
		if (m_ch == '=')
		{
			m_tokenType = Equals;
		}
		else
		{
			getNextChar = false;
		}
		break;
	case '<':
		m_tokenType = LessThan;
		m_token.push_back(m_ch);
		m_ch = m_reader->getChar();
		if (m_ch == '=')
		{
			m_tokenType = LessEqual;
		}
		else
		{
			getNextChar = false;
		}
		break;
	case '>':
		m_tokenType = GreaterThan;
		m_token.push_back(m_ch);
		m_ch = m_reader->getChar();
		if (m_ch == '=')
		{
			m_tokenType = GreaterEqual;
		}
		else
		{
			getNextChar = false;
		}
		break;
	case '!':
		m_tokenType = Not;
		m_token.push_back(m_ch);
		m_ch = m_reader->getChar();
		if (m_ch == '=')
		{
			m_tokenType = NotEqual;
		}
		else
		{
			getNextChar = false;
		}
		break;
	case ';':
		m_tokenType = Terminator;
		break;
	case '(':
		m_tokenType = OpenParenthesis;
		break;
	case ')':
		m_tokenType = CloseParenthesis;
		break;
	case '+':
		m_tokenType = Plus;
		m_token.push_back(m_ch);
		m_ch = m_reader->getChar();
		if (m_ch == '+')
		{
			m_tokenType = Increment;
		}
		else if (m_ch == '=')
		{
			m_tokenType = PlusEquals;
		}
		else
		{
			getNextChar = false;
		}
		break;
	case '-':
		m_tokenType = Minus;
		m_token.push_back(m_ch);
		m_ch = m_reader->getChar();
		if (m_ch == '-')
		{
			m_tokenType = Decrement;
		}
		else if (m_ch == '=')
		{
			m_tokenType = MinusEquals;
		}
		else
		{
			getNextChar = false;
		}
		break;
	case '|':
		m_tokenType = InclusiveOR;
		break;
	case '*':
		m_tokenType = Multiply;
		break;
	case ',':
		m_tokenType = Comma;
		break;
	}
	m_token.push_back(m_ch);

	// get the next character ready for the next token
	if (getNextChar)
	{
		m_ch = m_reader->getChar();
	}

	return true;
}

SymbolTableNode* Parser::findNode(const std::wstring& token)
{
	// we need to check all the symbol tables in this and the outer scopes
	// to see where the node is
	SymbolTable* table = m_table;
	SymbolTableNode* node = table->find(token);

	// traverse the type tree to see if this is a derived method or parameter
	SymbolTableNode* tempnode = m_node;
	while (node == NULL && tempnode && tempnode->type() != NULL)
	{
		tempnode = tempnode->type();
		node = tempnode->symbols()->find(token);
	}

	// traverse up the namespace tree checking each parent namespace
	while (node == NULL && table->m_parent != NULL)
	{
		table = table->m_parent;
		node = table->find(token);
	}

	return node;
}