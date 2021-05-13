#pragma once

#include "parser.h"
#include "symboltable.h"

class Icode
{
public:
	Icode(void);
	~Icode(void);

public:
	void put(Parser::TokenType tokenType);
	void put(const SymbolTableNode* node);
	int putLocationMarker();
	int getLocationMarker();
	void fixupLocationMarker(int pos);
	void seek(int pos);

	// points the cursor back to the beginning, that's all
	void reset() { m_cursor = m_code; }
	Parser::TokenType getTokenType();
	SymbolTableNode* getNode();

protected:
	void checkBounds(int size);

protected:
	unsigned char* m_code;
	int m_maxcode;
	unsigned char* m_cursor;
	int m_size;
};
